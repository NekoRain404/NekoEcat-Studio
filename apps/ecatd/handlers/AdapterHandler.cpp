// AdapterHandler — discovers and manages network adapters for EtherCAT.

#include "AdapterHandler.h"

#include "CommandDispatcher.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>

// List all network adapters as a JSON response.
QJsonObject AdapterHandler::handleList(const QString &id, const QJsonObject &params)
{
  Q_UNUSED(params);
  const auto adapters = enumerateAdapters();
  const QString current = currentAdapter();

  QJsonArray arr;
  for (const auto &a : adapters) {
    QJsonObject obj;
    obj["name"] = a.name;
    obj["mac"] = a.mac;
    obj["driver"] = a.driver;
    obj["pciSlot"] = a.pciSlot;
    obj["linkUp"] = a.linkUp;
    obj["isEthercat"] = a.isEthercat;
    arr.append(obj);
  }

  QJsonObject result;
  result["adapters"] = arr;
  result["current"] = current;
  return CommandDispatcher::success(id, result);
}

// Set the IgH master adapter by rewriting the master0_device line in /etc/ethercat.conf.
QJsonObject AdapterHandler::handleSet(const QString &id, const QJsonObject &params)
{
  const QString adapter = params.value("adapter").toString().trimmed();
  if (adapter.isEmpty()) {
    return CommandDispatcher::failure(id, "Missing 'adapter' parameter.");
  }

  // Validate that the requested adapter exists on the host.
  const auto adapters = enumerateAdapters();
  bool found = false;
  for (const auto &a : adapters) {
    if (a.name == adapter) {
      found = true;
      break;
    }
  }
  if (!found) {
    return CommandDispatcher::failure(id,
        QStringLiteral("Adapter '%1' not found.").arg(adapter));
  }

  // Rewrite /etc/ethercat.conf, replacing the master0_device line.
  const QString confPath = QStringLiteral("/etc/ethercat.conf");
  QFile file(confPath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return CommandDispatcher::failure(id,
        QStringLiteral("Cannot read %1: %2").arg(confPath, file.errorString()));
  }

  const QString content = QString::fromUtf8(file.readAll());
  file.close();

  // Match: MASTER0_DEVICE="anything"
  const QRegularExpression re(
      QStringLiteral("^MASTER0_DEVICE\\s*=\\s*\"[^\"]*\""),
      QRegularExpression::MultilineOption);
  const QString replacement = QStringLiteral("MASTER0_DEVICE=\"%1\"").arg(adapter);

  QString updated = content;
  if (re.match(content).hasMatch()) {
    updated.replace(re, replacement);
  } else {
    // Line not present — append it.
    if (!content.endsWith('\n')) {
      updated += '\n';
    }
    updated += replacement + '\n';
  }

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
    return CommandDispatcher::failure(id,
        QStringLiteral("Cannot write %1: %2").arg(confPath, file.errorString()));
  }
  file.write(updated.toUtf8());
  file.close();

  QJsonObject result;
  result["adapter"] = adapter;
  result["message"] = QStringLiteral("Adapter set to '%1'. Restart ethercat service to apply.").arg(adapter);
  return CommandDispatcher::success(id, result);
}

// Enumerate all network interfaces under /sys/class/net/.
// Skips the loopback device.
QVector<NetworkAdapter> AdapterHandler::enumerateAdapters() const
{
  QVector<NetworkAdapter> result;
  const QString basePath = QStringLiteral("/sys/class/net");
  const QDir dir(basePath);
  const QStringList entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

  const QString ethercat = currentAdapter();

  for (const QString &iface : entries) {
    if (iface == "lo")
      continue;

    NetworkAdapter adapter;
    adapter.name = iface;
    adapter.isEthercat = (iface == ethercat);

    // MAC address.
    adapter.mac = readSysfs(basePath + '/' + iface + "/address");

    // PCI slot name from device uevent.
    const QString uevent = readSysfs(basePath + '/' + iface + "/device/uevent");
    if (!uevent.isEmpty()) {
      static const QRegularExpression slotRe(QStringLiteral("PCI_SLOT_NAME=(\\S+)"));
      const auto match = slotRe.match(uevent);
      adapter.pciSlot = match.hasMatch() ? match.captured(1) : QString();
    }

    // Kernel driver from the symlink target.
    const QString driverLink = basePath + '/' + iface + "/device/driver";
    const QFileInfo driverInfo(driverLink);
    if (driverInfo.isSymLink()) {
      adapter.driver = driverInfo.symLinkTarget().section('/', -1);
    }

    // Link status via ethtool.
    const QString ethtoolOut = runCommand(QStringLiteral("ethtool %1").arg(iface));
    adapter.linkUp = ethtoolOut.contains("Link detected: yes");

    result.append(adapter);
  }

  return result;
}

// Read a sysfs file and return its content trimmed.
QString AdapterHandler::readSysfs(const QString &path) const
{
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    return {};
  return QString::fromUtf8(f.readAll()).trimmed();
}

// Run a shell command synchronously and return stdout.
QString AdapterHandler::runCommand(const QString &cmd) const
{
  QProcess proc;
  proc.start("sh", {"-c", cmd});
  proc.waitForFinished(5000);
  return QString::fromUtf8(proc.readAllStandardOutput());
}

// Read the current IgH master0_device setting from /etc/ethercat.conf.
QString AdapterHandler::currentAdapter() const
{
  const QString content = readSysfs("/etc/ethercat.conf");
  // Match: MASTER0_DEVICE="<value>"
  static const QRegularExpression re(
      QStringLiteral("MASTER0_DEVICE\\s*=\\s*\"([^\"]*)\""));
  const auto match = re.match(content);
  return match.hasMatch() ? match.captured(1) : QString();
}
