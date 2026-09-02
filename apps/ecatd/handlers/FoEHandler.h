#pragma once
// FoEHandler — File over EtherCAT firmware read/write via IgH CLI.
//
// Wraps `ethercat foe_read` and `ethercat foe_write` commands to transfer
// firmware files to/from EtherCAT slaves. Progress is parsed from CLI
// stderr output when available.
//
// Thread safety:
//   Each handler invocation runs a blocking QProcess. Call from the daemon's
//   event loop thread only; long transfers will block the event loop.
//   (Future: move to QProcess async mode for non-blocking operation.)

#include <QJsonObject>
#include <QString>
#include <QStringList>

class FoEHandler {
public:
    // Read firmware from a slave (save to local file).
    // params: { "position": int, "filePath": string }
    // Returns: { "success": true, "filePath": string, "fileSize": int, "message": string }
    QJsonObject handleFoeRead(const QString& id, const QJsonObject& params);

    // Write firmware to a slave (upload local file).
    // params: { "position": int, "filePath": string, "password": uint32 (optional) }
    // Returns: { "success": true, "bytesWritten": int, "message": string }
    QJsonObject handleFoeWrite(const QString& id, const QJsonObject& params);

    // Validate that a file path is absolute, free of traversal sequences,
    // resolves within an allowed firmware base directory, and its parent
    // directory exists. Exposed for testing.
    bool validateFilePath(const QString& path, QString* error) const;

private:
    // Allowed base directories for FoE transfers (default: /tmp + $HOME, or
    // the NEKOECAT_FIRMWARE_DIR env var when set).
    QStringList firmwareBaseDirs() const;

    // Reject any existing final path component that is a symlink, a hard link
    // (st_nlink > 1), or anything that is not a regular file. A missing file is
    // acceptable (it will be created). Blocks exfiltration via hard links.
    bool rejectUnsafeExistingFile(const QString& path, QString* error) const;

    // Re-run the base-directory + unsafe-link checks on a file that the CLI may
    // have just created, closing the validate-then-use TOCTOU for foe_read.
    bool revalidateCreatedFile(const QString& path, QString* error) const;

    // Run an ethercat CLI command and capture stdout/stderr.
    // Returns the process exit code. stdout -> output, stderr -> errorOutput.
    int runEthercatCommand(const QStringList& args, QString* output, QString* errorOutput,
                           int timeoutMs = 120000) const;

    // Get file size in bytes, or -1 if the file doesn't exist.
    qint64 fileSizeBytes(const QString& path) const;
};
