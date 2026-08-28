// FoEHandler — File over EtherCAT firmware read/write via IgH CLI.

#include "FoEHandler.h"
#include "CommandDispatcher.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>

// Read firmware from a slave using `ethercat foe_read -p N --output FILE`.
// The file is saved locally on the daemon host.
QJsonObject FoEHandler::handleFoeRead(const QString &id, const QJsonObject &params)
{
    const int position = params.value("position").toInt(-1);
    const QString filePath = params.value("filePath").toString().trimmed();

    // Validate required parameters.
    if (position < 0) {
        return CommandDispatcher::failure(id, "Missing or invalid 'position' parameter.");
    }
    if (filePath.isEmpty()) {
        return CommandDispatcher::failure(id, "Missing 'filePath' parameter.");
    }

    // Validate the output file path.
    QString pathError;
    if (!validateFilePath(filePath, &pathError)) {
        return CommandDispatcher::failure(id, pathError);
    }

    // Build and execute the ethercat foe_read command.
    const QStringList args = {
        "foe_read",
        "-p", QString::number(position),
        "--output", filePath
    };

    QString stdoutData, stderrData;
    const int exitCode = runEthercatCommand(args, &stdoutData, &stderrData);

    if (exitCode != 0) {
        const QString errorMsg = stderrData.trimmed().isEmpty()
            ? QString("FoE read failed with exit code %1").arg(exitCode)
            : stderrData.trimmed();
        return CommandDispatcher::failure(id, errorMsg);
    }

    // Determine the size of the output file.
    const qint64 size = fileSizeBytes(filePath);

    QJsonObject result;
    result["success"] = true;
    result["filePath"] = filePath;
    result["fileSize"] = static_cast<qint64>(size >= 0 ? size : 0);
    result["message"] = QString("Firmware read from slave %1 to '%2' (%3 bytes)")
                            .arg(position)
                            .arg(filePath)
                            .arg(size >= 0 ? size : 0);
    return CommandDispatcher::success(id, result);
}

// Write firmware to a slave using `ethercat foe_write -p N --input FILE`.
// The file must be accessible on the daemon host.
QJsonObject FoEHandler::handleFoeWrite(const QString &id, const QJsonObject &params)
{
    const int position = params.value("position").toInt(-1);
    const QString filePath = params.value("filePath").toString().trimmed();
    const double pwd = params.value("password").toDouble(0);
    const quint32 password = (pwd >= 0 && pwd <= 0xFFFFFFFF) ? static_cast<quint32>(pwd) : 0;

    // Validate required parameters.
    if (position < 0) {
        return CommandDispatcher::failure(id, "Missing or invalid 'position' parameter.");
    }
    if (filePath.isEmpty()) {
        return CommandDispatcher::failure(id, "Missing 'filePath' parameter.");
    }

    // Validate the input file exists, is readable, and is inside the allowed
    // firmware base (blocks uploading sensitive files from arbitrary paths).
    QString pathError;
    if (!validateFilePath(filePath, &pathError)) {
        return CommandDispatcher::failure(id, pathError);
    }
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        return CommandDispatcher::failure(id,
            QString("Input file '%1' does not exist.").arg(filePath));
    }
    if (!fileInfo.isReadable()) {
        return CommandDispatcher::failure(id,
            QString("Input file '%1' is not readable.").arg(filePath));
    }

    // Build the ethercat foe_write command.
    QStringList args = {
        "foe_write",
        "-p", QString::number(position),
        "--input", filePath
    };

    // Append password if non-zero (FoE bootstrap password).
    if (password != 0) {
        args << "--password" << QString::number(password);
    }

    QString stdoutData, stderrData;
    const int exitCode = runEthercatCommand(args, &stdoutData, &stderrData);

    if (exitCode != 0) {
        const QString errorMsg = stderrData.trimmed().isEmpty()
            ? QString("FoE write failed with exit code %1").arg(exitCode)
            : stderrData.trimmed();
        return CommandDispatcher::failure(id, errorMsg);
    }

    // Report the size of the file that was written.
    const qint64 fileSize = fileInfo.size();

    QJsonObject result;
    result["success"] = true;
    result["bytesWritten"] = static_cast<qint64>(fileSize);
    result["message"] = QString("Firmware '%1' written to slave %2 (%3 bytes)")
                            .arg(filePath)
                            .arg(position)
                            .arg(fileSize);
    return CommandDispatcher::success(id, result);
}

// Run an ethercat CLI command synchronously via QProcess.
// Returns the process exit code; captures stdout and stderr.
int FoEHandler::runEthercatCommand(const QStringList &args, QString *output,
                                   QString *errorOutput, int timeoutMs) const
{
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

// Validate that a path is absolute, free of traversal sequences, resolves
// without symlinks escaping, and stays inside an allowed firmware base directory.
bool FoEHandler::validateFilePath(const QString &path, QString *error) const
{
    QFileInfo info(path);
    if (!info.isAbsolute()) {
        if (error) *error = QString("File path '%1' must be absolute.").arg(path);
        return false;
    }
    // Reject path traversal sequences
    if (path.contains("/../") || path.endsWith("/..") || path == QLatin1String("..") || path.contains("/..")) {
        if (error) *error = QString("File path '%1' must not contain '..' components.").arg(path);
        return false;
    }
    if (!info.dir().exists()) {
        if (error) *error = QString("Directory '%1' does not exist.").arg(info.path());
        return false;
    }

    // Resolve symlinks and normalize. For a not-yet-created file canonicalFilePath
    // is empty, so resolve the parent directory instead (it must exist).
    QString resolved = info.canonicalFilePath();
    if (resolved.isEmpty()) {
        const QString parent = QFileInfo(info.path()).canonicalFilePath();
        if (parent.isEmpty()) {
            if (error) *error = QString("Cannot resolve '%1'.").arg(info.path());
            return false;
        }
        resolved = parent;
    }

    // The resolved target must stay inside one of the allowed firmware bases.
    // This defeats arbitrary-path writes (e.g. /etc/cron.d/...) and symlink
    // traversal that points outside the base.
    const QStringList bases = firmwareBaseDirs();
    for (const QString &base : bases) {
        const QString baseCanonical = QFileInfo(base).canonicalFilePath();
        if (baseCanonical.isEmpty()) {
            continue;
        }
        if (resolved == baseCanonical || resolved.startsWith(baseCanonical + QLatin1Char('/'))) {
            return true;
        }
    }
    if (error) {
        *error = QString("File path '%1' resolves to '%2', outside the allowed firmware directory.").arg(path, resolved);
    }
    return false;
}

// Allowed base directories for FoE file transfers. Set NEKOECAT_FIRMWARE_DIR to
// restrict to a single directory; otherwise /tmp and the user's home are allowed.
QStringList FoEHandler::firmwareBaseDirs() const
{
    const QByteArray env = qgetenv("NEKOECAT_FIRMWARE_DIR");
    if (!env.isEmpty()) {
        return {QString::fromLocal8Bit(env)};
    }
    QStringList bases;
    bases << QStringLiteral("/tmp");
    const QString home = QDir::homePath();
    if (!home.isEmpty() && home != QStringLiteral("/tmp")) {
        bases << home;
    }
    return bases;
}

// Get the size of a file in bytes. Returns -1 if the file doesn't exist.
qint64 FoEHandler::fileSizeBytes(const QString &path) const
{
    QFileInfo info(path);
    return info.exists() ? info.size() : -1;
}
