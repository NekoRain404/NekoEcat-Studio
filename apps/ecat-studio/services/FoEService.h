#pragma once

// FoEService — File over EtherCAT protocol support.
//
// Provides file read/write operations, file listing, and file info
// retrieval for EtherCAT slaves that support FoE.
//
// This service provides File over EtherCAT (FoE) protocol support.
// It handles:
//   - File read operations from slaves
//   - File write operations to slaves
//   - File listing from slaves
//   - File information retrieval
//   - Progress tracking for read/write operations
//
// Usage:
//   ServiceContainer *container = ...;
//   FoEService *foe = container->foe();
//   foe->readFile(0, "firmware.bin");  // Read file from slave 0
//   foe->writeFile(0, "config.bin", data);  // Write file to slave 0
//   foe->listFiles(0);  // List files on slave 0
//   foe->fileInfo(0, "firmware.bin");  // Get file info
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   marshals daemon communication to the main thread internally.
//
// Performance:
//   - File read/write operations are O(n) where n is file size
//   - File listing is O(1) for request, O(n) for response
//   - Progress tracking is O(1) per update

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QDateTime>
#include <QJsonObject>
#include <QStringList>

class EcatClient;

// File information structure.
struct FoEFileInfo {
    QString fileName;        // File name
    qint64 fileSize = 0;     // File size in bytes
    QDateTime lastModified;  // Last modification timestamp
    quint32 checksum = 0;    // File checksum
};
Q_DECLARE_METATYPE(FoEFileInfo)

class FoEService : public QObject {
    Q_OBJECT
public:
    explicit FoEService(EcatClient *client, QObject *parent = nullptr);

    // Read a file from a slave.
    // @param position   Slave position
    // @param fileName   File name to read
    void readFile(int position, const QString &fileName);

    // Write a file to a slave.
    // @param position   Slave position
    // @param fileName   File name to write
    // @param data       File data
    // @return true if write was initiated successfully
    bool writeFile(int position, const QString &fileName, const QByteArray &data);

    // List files on a slave.
    // @param position  Slave position
    void listFiles(int position);

    // Get file information from a slave.
    // @param position  Slave position
    // @param fileName  File name to get info for
    void fileInfo(int position, const QString &fileName);

    // Read firmware from a slave using FoE protocol.
    // Saves the firmware file to outputPath on the daemon host.
    // Emits firmwareReadComplete() on success, error() on failure.
    // @param position    Slave position on the bus (0-based)
    // @param outputPath  Absolute path where the firmware file will be saved on the daemon host
    void readFirmware(int position, const QString &outputPath);

    // Write firmware to a slave using FoE protocol.
    // Reads the firmware file from inputPath on the daemon host.
    // Emits firmwareWriteComplete() on success, error() on failure.
    // @param position   Slave position on the bus (0-based)
    // @param inputPath  Absolute path to the firmware file on the daemon host
    // @param password   FoE bootstrap password (0 = no password)
    void writeFirmware(int position, const QString &inputPath, uint32_t password = 0);

signals:
    // Emitted when a file read completes.
    // @param position  Slave position
    // @param fileName  File name
    // @param data      File data
    void fileReadComplete(int position, const QString &fileName,
                          const QByteArray &data);

    // Emitted when a file write completes.
    // @param position  Slave position
    // @param fileName  File name
    // @param success   Whether write was successful
    void fileWriteComplete(int position, const QString &fileName, bool success);

    // Emitted when file list is received.
    // @param position  Slave position
    // @param files     List of file names
    void fileListReceived(int position, const QStringList &files);

    // Emitted when file info is received.
    // @param position  Slave position
    // @param info      FoEFileInfo structure
    void fileInfoReceived(int position, const FoEFileInfo &info);

    // Emitted when read progress updates.
    // @param position  Slave position
    // @param progress  Progress percentage (0-100)
    void readProgress(int position, int progress);

    // Emitted when write progress updates.
    // @param position  Slave position
    // @param progress  Progress percentage (0-100)
    void writeProgress(int position, int progress);

    // Emitted when firmware read (FoE) completes successfully.
    // @param position  Slave position
    // @param filePath  Path where the firmware was saved on the daemon host
    // @param fileSize  Size of the firmware file in bytes
    void firmwareReadComplete(int position, const QString &filePath, qint64 fileSize);

    // Emitted when firmware write (FoE) completes successfully.
    // @param position      Slave position
    // @param bytesWritten  Number of bytes written to the slave
    void firmwareWriteComplete(int position, qint64 bytesWritten);

    // Emitted when an error occurs.
    // @param message  Human-readable error message
    void error(const QString &message);

private:
    EcatClient *client_;  // TCP client to ecatd daemon
};
