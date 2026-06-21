#include "FoEService.h"
#include "EcatClient.h"

// FoEService.cpp — File over EtherCAT (FoE) file transfer protocol
//
// Implementation notes:
//   - readFirmware/writeFirmware delegate to EcatClient for daemon communication
//   - readFile/writeFile/listFiles/fileInfo are stubs for future FoE mailbox support
//   - Progress signals emitted at 0/50/100% for UI progress bars

FoEService::FoEService(EcatClient *client, QObject *parent)
    : QObject(parent), client_(client) {
    connect(client_, &EcatClient::errorMessage, this,
            [this](const QString &msg) { emit error(msg); });

    // Connect to FoE-specific daemon response signals.
    connect(client_, &EcatClient::foeReadResult, this,
            [this](int position, const QString &filePath, qint64 fileSize) {
                emit firmwareReadComplete(position, filePath, fileSize);
            });
    connect(client_, &EcatClient::foeWriteResult, this,
            [this](int position, qint64 bytesWritten) {
                emit firmwareWriteComplete(position, bytesWritten);
            });
}

void FoEService::readFile(int position, const QString &fileName) {
    emit readProgress(position, 0);
    QByteArray data;
    data.fill(0, 256);
    emit readProgress(position, 100);
    emit fileReadComplete(position, fileName, data);
}

bool FoEService::writeFile(int position, const QString &fileName,
                            const QByteArray &data) {
    Q_UNUSED(data);
    emit writeProgress(position, 0);
    emit writeProgress(position, 50);
    emit writeProgress(position, 100);
    emit fileWriteComplete(position, fileName, true);
    return true;
}

void FoEService::listFiles(int position) {
    QStringList files;
    files << "firmware.bin" << "config.dat" << "bootloader.bin";
    emit fileListReceived(position, files);
}

void FoEService::fileInfo(int position, const QString &fileName) {
    FoEFileInfo info;
    info.fileName = fileName;
    info.fileSize = 131072;
    info.lastModified = QDateTime::currentDateTime();
    info.checksum = 0xABCD1234;
    emit fileInfoReceived(position, info);
}

// Read firmware from a slave using FoE protocol via the daemon.
void FoEService::readFirmware(int position, const QString &outputPath) {
    client_->foeRead(position, outputPath);
}

// Write firmware to a slave using FoE protocol via the daemon.
void FoEService::writeFirmware(int position, const QString &inputPath, uint32_t password) {
    client_->foeWrite(position, inputPath, password);
}
