// SoEHandler — Servo over EtherCAT (SoE) IDN read/write via IgH CLI.

#include "SoEHandler.h"
#include "CommandDispatcher.h"

#include <QProcess>
#include <QRegularExpression>

// Read an SoE IDN using `ethercat soe_read -p N [drive] <IDN> [--type T]`.
QJsonObject SoEHandler::handleSoeRead(const QString& id, const QJsonObject& params) {
    const int position = params.value("position").toInt(-1);
    const QString idn = params.value("idn").toString().trimmed();
    const int drive = params.value("drive").toInt(0);
    const QString type = params.value("type").toString().trimmed();

    if (position < 0) {
        return CommandDispatcher::failure(id, "Missing or invalid 'position' parameter.");
    }
    QString idnError;
    if (!validateIdn(idn, &idnError)) {
        return CommandDispatcher::failure(id, idnError);
    }
    if (drive < 0 || drive > 7) {
        return CommandDispatcher::failure(id, "Drive number must be 0-7.");
    }

    QStringList args = {"soe_read", "-p", QString::number(position)};
    if (!type.isEmpty()) {
        args << "--type" << type;
    }
    // Drive number is positional and optional; include it explicitly.
    args << QString::number(drive) << idn;

    QString stdoutData, stderrData;
    const int exitCode = runEthercatCommand(args, &stdoutData, &stderrData);
    if (exitCode != 0) {
        const QString msg = stderrData.trimmed().isEmpty() ? QString("SoE read failed with exit code %1").arg(exitCode)
                                                           : stderrData.trimmed();
        return CommandDispatcher::failure(id, msg);
    }

    QJsonObject result;
    result["value"] = stdoutData.trimmed();
    result["idn"] = idn;
    result["type"] = type;
    result["drive"] = drive;
    return CommandDispatcher::success(id, result);
}

// Write an SoE IDN using `ethercat soe_write -p N [--type T] [drive] <IDN> <value>`.
QJsonObject SoEHandler::handleSoeWrite(const QString& id, const QJsonObject& params) {
    const int position = params.value("position").toInt(-1);
    const QString idn = params.value("idn").toString().trimmed();
    const QString value = params.value("value").toString();
    const int drive = params.value("drive").toInt(0);
    const QString type = params.value("type").toString().trimmed();

    if (position < 0) {
        return CommandDispatcher::failure(id, "Missing or invalid 'position' parameter.");
    }
    QString idnError;
    if (!validateIdn(idn, &idnError)) {
        return CommandDispatcher::failure(id, idnError);
    }
    if (value.isEmpty()) {
        return CommandDispatcher::failure(id, "Missing 'value' parameter.");
    }
    if (drive < 0 || drive > 7) {
        return CommandDispatcher::failure(id, "Drive number must be 0-7.");
    }

    QStringList args = {"soe_write", "-p", QString::number(position)};
    if (!type.isEmpty()) {
        args << "--type" << type;
    }
    args << QString::number(drive) << idn << value;

    QString stdoutData, stderrData;
    const int exitCode = runEthercatCommand(args, &stdoutData, &stderrData);
    if (exitCode != 0) {
        const QString msg = stderrData.trimmed().isEmpty() ? QString("SoE write failed with exit code %1").arg(exitCode)
                                                           : stderrData.trimmed();
        return CommandDispatcher::failure(id, msg);
    }

    QJsonObject result;
    result["success"] = true;
    result["idn"] = idn;
    result["drive"] = drive;
    result["message"] = QString("IDN %1 written to slave %2 (drive %3)").arg(idn).arg(position).arg(drive);
    return CommandDispatcher::success(id, result);
}

// Validate an IDN: either S-x-yyyy / P-x-yyyy form, or a numeric (dec/hex) value.
bool SoEHandler::validateIdn(const QString& idn, QString* error) const {
    if (idn.isEmpty()) {
        if (error)
            *error = "Missing 'idn' parameter.";
        return false;
    }

    // String form: [SP]-<paramset 0-7>-<datablock> e.g. "P-0-0150", "S-0-1000".
    static QRegularExpression strRe(R"(^[SsPp]-[0-7]-\d{1,5}$)");
    if (strRe.match(idn).hasMatch()) {
        return true;
    }

    // Numeric form: decimal, octal (0..), or hex (0x..).
    bool ok = false;
    if (idn.startsWith("0x", Qt::CaseInsensitive)) {
        idn.mid(2).toUInt(&ok, 16);
    } else {
        idn.toUInt(&ok, 0);
    }
    if (ok)
        return true;

    if (error) {
        *error = QString("Invalid IDN '%1'. Use S-x-yyyy, P-x-yyyy, or a numeric value.").arg(idn);
    }
    return false;
}

int SoEHandler::runEthercatCommand(const QStringList& args, QString* output, QString* errorOutput,
                                   int timeoutMs) const {
    QProcess proc;
    proc.start("ethercat", args);
    if (!proc.waitForFinished(timeoutMs)) {
        proc.kill();
        proc.waitForFinished(1000);
        if (errorOutput) {
            *errorOutput = QString("ethercat command timed out after %1ms").arg(timeoutMs);
        }
        return -1;
    }
    if (output) {
        *output = QString::fromUtf8(proc.readAllStandardOutput());
    }
    if (errorOutput) {
        *errorOutput = QString::fromUtf8(proc.readAllStandardError());
    }
    return proc.exitCode();
}
