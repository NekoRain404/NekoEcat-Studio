#include "EthercatCliBackend.h"

#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QProcess>
#include <QRegularExpression>

#include <cerrno>
#include <cstring>
#include <unistd.h>

namespace {

struct ProcessResult {
  bool started = false;
  int exitCode = -1;
  QString stdOut;
  QString stdErr;
};

ProcessResult runProgram(const QString &program,
                         const QStringList &arguments = {},
                         int timeoutMs = 5000) {
  QProcess process;
  process.setProgram(program);
  process.setArguments(arguments);
  process.start();

  ProcessResult result;
  if (!process.waitForStarted(3000)) {
    result.stdErr = QString("Failed to start %1").arg(program);
    return result;
  }

  result.started = true;
  if (!process.waitForFinished(timeoutMs)) {
    process.kill();
    process.waitForFinished(1000);
    result.stdErr = QString("%1 timed out").arg(program);
    return result;
  }

  result.exitCode = process.exitCode();
  result.stdOut =
      QString::fromLocal8Bit(process.readAllStandardOutput()).trimmed();
  result.stdErr =
      QString::fromLocal8Bit(process.readAllStandardError()).trimmed();
  return result;
}

QJsonObject diagnosticItem(const QString &level, const QString &source,
                           const QString &message, const QString &detail = {},
                           const QString &hint = {},
                           const QString &command = {}) {
  return QJsonObject{
      {"level", level},   {"source", source}, {"message", message},
      {"detail", detail}, {"hint", hint},     {"command", command},
  };
}

QString configValue(const QString &config, const QString &key) {
  const QRegularExpression re(QString("^\\s*%1\\s*=\\s*\"?([^\"\\n#]*)\"?")
                                  .arg(QRegularExpression::escape(key)),
                              QRegularExpression::MultilineOption);
  const auto match = re.match(config);
  return match.hasMatch() ? match.captured(1).trimmed() : QString();
}

bool isMacAddress(const QString &value) {
  static const QRegularExpression re(R"(^([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2}$)");
  return re.match(value.trimmed()).hasMatch();
}

QStringList matchingBlacklistLines() {
  QStringList matches;
  for (const QString &root : {QStringLiteral("/etc/modprobe.d"),
                              QStringLiteral("/usr/lib/modprobe.d")}) {
    QDirIterator it(root, QStringList{"*.conf"}, QDir::Files,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
      const QString path = it.next();
      QFile file(path);
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        continue;
      }
      int lineNo = 0;
      while (!file.atEnd()) {
        ++lineNo;
        const QString line = QString::fromLocal8Bit(file.readLine()).trimmed();
        if (line.startsWith('#')) {
          continue;
        }
        if (line.contains("blacklist") &&
            (line.contains("r8152") || line.contains("rt8152") ||
             line.contains("rtl815"))) {
          matches << QString("%1:%2 %3").arg(path).arg(lineNo).arg(line);
        }
      }
    }
  }
  return matches;
}

} // namespace

EthercatCliBackend::EthercatCliBackend(QObject *parent) : QObject(parent) {}

QString EthercatCliBackend::masterText(const QString &master,
                                       QString *error) const {
  int exitCode = 0;
  QString stdErr;
  const QString text = run(master, {"master"}, &exitCode, &stdErr);
  if (exitCode != 0 && error) {
    *error = stdErr.isEmpty() ? text : stdErr;
  }
  return text;
}

QVector<SlaveInfo> EthercatCliBackend::scanSlaves(const QString &master,
                                                  QString *error) const {
  int exitCode = 0;
  QString stdErr;
  const QString text = run(master, {"slaves"}, &exitCode, &stdErr);
  if (exitCode != 0 && error) {
    *error = stdErr.isEmpty() ? text : stdErr;
  }
  return exitCode == 0 ? parseSlaves(text) : QVector<SlaveInfo>{};
}

QString EthercatCliBackend::slaveInfo(const QString &master, int position,
                                      QString *error) const {
  int exitCode = 0;
  QString stdErr;
  const QString text =
      run(master, {"slaves", "-p", QString::number(position), "-v"}, &exitCode,
          &stdErr);
  if (exitCode != 0 && error) {
    *error = stdErr.isEmpty() ? text : stdErr;
  }
  return text;
}

QString EthercatCliBackend::slaveXml(const QString &master, int position,
                                     QString *error) const {
  int exitCode = 0;
  QString stdErr;
  const QString text =
      run(master, {"xml", "-p", QString::number(position)}, &exitCode, &stdErr);
  if (exitCode != 0 && error) {
    *error = stdErr.isEmpty() ? text : stdErr;
  }
  return text;
}

QString EthercatCliBackend::pdos(const QString &master, int position,
                                 QString *error) const {
  int exitCode = 0;
  QString stdErr;
  const QString text = run(master, {"pdos", "-p", QString::number(position)},
                           &exitCode, &stdErr);
  if (exitCode != 0 && error) {
    *error = stdErr.isEmpty() ? text : stdErr;
  }
  return text;
}

QString EthercatCliBackend::sdos(const QString &master, int position,
                                 QString *error) const {
  int exitCode = 0;
  QString stdErr;
  const QString text = run(master, {"sdos", "-p", QString::number(position)},
                           &exitCode, &stdErr);
  if (exitCode != 0 && error) {
    *error = stdErr.isEmpty() ? text : stdErr;
  }
  return text;
}

QString EthercatCliBackend::upload(const QString &master, int position,
                                   const QString &index,
                                   const QString &subIndex,
                                   QString *error) const {
  int exitCode = 0;
  QString stdErr;
  const QString text =
      run(master, {"upload", "-p", QString::number(position), index, subIndex},
          &exitCode, &stdErr);
  if (exitCode != 0 && error) {
    *error = stdErr.isEmpty() ? text : stdErr;
  }
  return text.trimmed();
}

bool EthercatCliBackend::download(const QString &master, int position,
                                  const QString &index, const QString &subIndex,
                                  const QString &value, const QString &type,
                                  QString *error) const {
  int exitCode = 0;
  QString stdErr;
  QStringList args = {"download", "-p", QString::number(position)};
  if (!type.trimmed().isEmpty()) {
    args << "-t" << type.trimmed();
  }
  args << index << subIndex << value;
  run(master, args, &exitCode, &stdErr);
  if (exitCode != 0 && error) {
    *error = stdErr;
  }
  return exitCode == 0;
}

bool EthercatCliBackend::setState(const QString &master, int position,
                                  const QString &state, QString *error) const {
  int exitCode = 0;
  QString stdErr;
  run(master, {"states", "-p", QString::number(position), state}, &exitCode,
      &stdErr);
  if (exitCode != 0 && error) {
    *error = stdErr;
  }
  return exitCode == 0;
}

bool EthercatCliBackend::setAllStates(const QString &master,
                                      const QString &state,
                                      QString *error) const {
  int exitCode = 0;
  QString stdErr;
  run(master, {"states", state}, &exitCode, &stdErr);
  if (exitCode != 0 && error) {
    *error = stdErr;
  }
  return exitCode == 0;
}

bool EthercatCliBackend::rescan(const QString &master, QString *error) const {
  int exitCode = 0;
  QString stdErr;
  run(master, {"rescan"}, &exitCode, &stdErr);
  if (exitCode != 0 && error) {
    *error = stdErr;
  }
  return exitCode == 0;
}

QJsonArray EthercatCliBackend::hostDiagnostics(QString *error) const {
  Q_UNUSED(error)

  QJsonArray checks;

  const auto kernel = runProgram("uname", {"-r"});
  if (kernel.started && kernel.exitCode == 0) {
    checks.append(diagnosticItem(
        "Info", "Host", QString("Kernel: %1").arg(kernel.stdOut), {},
        "Keep DKMS modules installed for every active kernel."));
  } else {
    checks.append(diagnosticItem("Warning", "Host",
                                 "Unable to read running kernel", kernel.stdErr,
                                 "Run uname -r in a terminal and verify "
                                 "matching kernel headers are installed."));
  }

  const auto ecMaster = runProgram("modinfo", {"ec_master"});
  if (ecMaster.started && ecMaster.exitCode == 0) {
    const QString detail = ecMaster.stdOut.split('\n').value(0).trimmed();
    checks.append(
        diagnosticItem("Info", "Kernel Module",
                       "ec_master is available for the running kernel", detail,
                       "No action required."));
  } else {
    checks.append(diagnosticItem(
        "Error", "Kernel Module",
        "ec_master is not available for the running kernel",
        "Rebuild or reinstall etherlab-ethercat after a kernel update.",
        "Install etherlab-ethercat-dkms, verify linux headers, then restart "
        "ethercat.service.",
        "yay -S etherlab-ethercat-dkms && sudo systemctl restart ethercat"));
  }

  QFile configFile("/etc/ethercat.conf");
  QString master0Device;
  QString updownInterfaces;
  QString deviceModules;
  if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    const QString config = QString::fromLocal8Bit(configFile.readAll());
    master0Device = configValue(config, "MASTER0_DEVICE");
    updownInterfaces = configValue(config, "UPDOWN_INTERFACES");
    deviceModules = configValue(config, "DEVICE_MODULES");
    checks.append(diagnosticItem(
        "Info", "Config",
        QString("MASTER0_DEVICE=%1, DEVICE_MODULES=%2")
            .arg(master0Device, deviceModules),
        QString("UPDOWN_INTERFACES=%1").arg(updownInterfaces),
        "Keep MASTER0_DEVICE aligned with the dedicated EtherCAT NIC."));
  } else {
    checks.append(diagnosticItem(
        "Error", "Config", "/etc/ethercat.conf is not readable", {},
        "Create or restore /etc/ethercat.conf and set MASTER0_DEVICE.",
        "sudoedit /etc/ethercat.conf && sudo systemctl restart ethercat"));
  }

  if (!master0Device.isEmpty() && !isMacAddress(master0Device)) {
    const auto links = runProgram("ip", {"-br", "link"});
    if (links.started && links.exitCode == 0) {
      bool found = false;
      for (const auto &line : links.stdOut.split('\n', Qt::SkipEmptyParts)) {
        if (line.startsWith(master0Device + ' ')) {
          found = true;
          checks.append(diagnosticItem(
              "Info", "Network", QString("%1 is present").arg(master0Device),
              line.trimmed(), "No action required."));
          break;
        }
      }
      if (!found) {
        checks.append(diagnosticItem(
            "Error", "Network",
            QString("%1 is not present in current network interfaces")
                .arg(master0Device),
            links.stdOut,
            "Update MASTER0_DEVICE or reconnect/enable the EtherCAT NIC, then "
            "restart ethercat.service.",
            "ip -br link && sudoedit /etc/ethercat.conf && sudo systemctl "
            "restart ethercat"));
      }
    } else {
      checks.append(diagnosticItem("Warning", "Network",
                                   "Unable to list network interfaces",
                                   links.stdErr,
                                   "Run ip -br link manually and confirm the "
                                   "configured EtherCAT NIC exists."));
    }
  }

  const QFileInfo ethercatDevice("/dev/EtherCAT0");
  const auto deviceStat = runProgram(
      "stat", {"-Lc", "%A %U:%G major=%t minor=%T", "/dev/EtherCAT0"});
  const QString deviceDetail = deviceStat.started && deviceStat.exitCode == 0
                                   ? deviceStat.stdOut
                                   : deviceStat.stdErr;
  if (ethercatDevice.exists()) {
    errno = 0;
    const bool canAccessDevice = ::access("/dev/EtherCAT0", R_OK | W_OK) == 0;
    if (canAccessDevice) {
      checks.append(diagnosticItem(
          "Info", "Device Node",
          "/dev/EtherCAT0 exists and is readable/writable by the current user",
          deviceDetail, "No action required."));
    } else {
      checks.append(diagnosticItem(
          "Warning", "Device Node",
          "/dev/EtherCAT0 exists but current user may not have read/write "
          "access",
          QString("%1%2%3")
              .arg(deviceDetail)
              .arg(deviceDetail.isEmpty() ? QString() : QStringLiteral("\n"))
              .arg(QString::fromLocal8Bit(std::strerror(errno))),
          "Fix the EtherCAT device udev rule or add the user to the ethercat "
          "group, then re-login or restart the service.",
          "sudo chgrp ethercat /dev/EtherCAT0 && sudo chmod 660 "
          "/dev/EtherCAT0"));
    }
  } else {
    checks.append(diagnosticItem(
        "Error", "Device Node", "/dev/EtherCAT0 is missing", deviceDetail,
        "Restart ethercat.service and verify ec_master loaded for the running "
        "kernel.",
        "sudo systemctl restart ethercat && ls -l /dev/EtherCAT0"));
  }

  const QStringList blacklistLines = matchingBlacklistLines();
  if (blacklistLines.isEmpty()) {
    checks.append(diagnosticItem(
        "Info", "Driver",
        "No r8152/rt8152/rtl815x firmware/module blacklist entry found", {},
        "No action required."));
  } else {
    checks.append(diagnosticItem(
        "Warning", "Driver",
        "r8152/rt8152/rtl815x appears in firmware/module blacklist",
        blacklistLines.join('\n'),
        "Remove the blacklist entry if your EtherCAT NIC is "
        "RTL8152/RTL8153/RTL8156.",
        "sudoedit /etc/modprobe.d/<file>.conf && sudo mkinitcpio -P"));
  }

  const auto dkms = runProgram("dkms", {"status"});
  if (dkms.started && dkms.exitCode == 0) {
    const bool hasR8152 = dkms.stdOut.contains("r8152", Qt::CaseInsensitive) ||
                          dkms.stdOut.contains("rt8152", Qt::CaseInsensitive) ||
                          dkms.stdOut.contains("rtl815", Qt::CaseInsensitive);
    const bool hasR8125 = dkms.stdOut.contains("r8125", Qt::CaseInsensitive);
    if (hasR8152) {
      checks.append(diagnosticItem(
          "Info", "DKMS", "r8152/rt8152/rtl815x DKMS driver is installed",
          dkms.stdOut,
          "Use only if this host relies on a Realtek USB Ethernet adapter."));
    } else {
      checks.append(diagnosticItem(
          "Info", "DKMS", "No r8152/rt8152/rtl815x DKMS driver is installed",
          dkms.stdOut,
          "No action required unless the EtherCAT NIC is a Realtek USB "
          "adapter."));
    }
    if (hasR8125) {
      checks.append(diagnosticItem(
          "Info", "DKMS", "r8125 DKMS driver is installed", dkms.stdOut,
          "Keep r8125 DKMS if the EtherCAT NIC is RTL8125."));
    }
  } else {
    checks.append(diagnosticItem("Warning", "DKMS",
                                 "Unable to query DKMS status", dkms.stdErr,
                                 "Install dkms or run dkms status manually to "
                                 "verify kernel module coverage.",
                                 "sudo pacman -S dkms && dkms status"));
  }

  const auto status = runProgram("ethercatctl", {"status"});
  if (status.started && status.exitCode == 0) {
    checks.append(diagnosticItem(
        "Info", "EtherCAT", "ethercatctl reports master running", status.stdOut,
        "Ready for online scan and commissioning."));
  } else {
    checks.append(diagnosticItem(
        "Error", "EtherCAT", "ethercatctl reports master not running",
        status.stdErr.isEmpty() ? status.stdOut : status.stdErr,
        "Run sudo systemctl restart ethercat; if it still fails, inspect "
        "ec_master, /dev/EtherCAT0, and /etc/ethercat.conf.",
        "sudo systemctl restart ethercat && ethercatctl status"));
  }

  return checks;
}

QString EthercatCliBackend::run(const QString &master,
                                const QStringList &arguments, int *exitCode,
                                QString *stdErr) const {
  QProcess process;
  QStringList scopedArguments;
  const QString trimmedMaster = master.trimmed();
  if (!trimmedMaster.isEmpty()) {
    scopedArguments << "-m" << trimmedMaster;
  }
  scopedArguments << arguments;

  process.setProgram("ethercat");
  process.setArguments(scopedArguments);
  process.start();
  if (!process.waitForStarted(3000)) {
    if (exitCode) {
      *exitCode = -1;
    }
    if (stdErr) {
      *stdErr = "Failed to start ethercat CLI. Is IgH installed and in PATH?";
    }
    return {};
  }
  process.waitForFinished(10000);
  if (exitCode) {
    *exitCode = process.exitCode();
  }
  if (stdErr) {
    *stdErr = QString::fromLocal8Bit(process.readAllStandardError()).trimmed();
  }
  return QString::fromLocal8Bit(process.readAllStandardOutput());
}

QVector<SlaveInfo> EthercatCliBackend::parseSlaves(const QString &text) const {
  QVector<SlaveInfo> slaves;
  const auto lines = text.split('\n', Qt::SkipEmptyParts);
  const QRegularExpression re(R"(^\s*(\d+)\s+\S+\s+(\S+)\s+(\S+)\s+(.+)$)");
  for (const auto &line : lines) {
    const auto match = re.match(line);
    if (!match.hasMatch()) {
      continue;
    }
    SlaveInfo slave;
    slave.position = match.captured(1).toInt();
    slave.state = match.captured(2);
    slave.flags = match.captured(3);
    slave.name = match.captured(4).trimmed();
    slave.rawLine = line;
    slaves.append(slave);
  }
  return slaves;
}
