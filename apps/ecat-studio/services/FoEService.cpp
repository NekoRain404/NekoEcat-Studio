#include "FoEService.h"
#include "EcatClient.h"

// FoEService.cpp — File over EtherCAT (FoE) file transfer protocol
//
// Implementation notes:
//   - readFirmware/writeFirmware delegate to EcatClient for daemon communication
//   - readFile/writeFile/listFiles/fileInfo require daemon-backed FoE mailbox support
//   - Operations do not emit success/progress signals until real daemon support exists

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
    Q_UNUSED(position);
    Q_UNUSED(fileName);
    if (!client_ || !client_->isConnected()) {
        emit error("Cannot read FoE file: EtherCAT daemon is not connected");
        return;
    }
    emit error("FoE generic file read is not implemented by the daemon backend");
}

bool FoEService::writeFile(int position, const QString &fileName,
                            const QByteArray &data) {
    Q_UNUSED(position);
    Q_UNUSED(fileName);
    Q_UNUSED(data);
    if (!client_ || !client_->isConnected()) {
        emit error("Cannot write FoE file: EtherCAT daemon is not connected");
        return false;
    }
    emit error("FoE generic file write is not implemented by the daemon backend");
    return false;
}

void FoEService::listFiles(int position) {
    Q_UNUSED(position);
    if (!client_ || !client_->isConnected()) {
        emit error("Cannot list FoE files: EtherCAT daemon is not connected");
        return;
    }
    emit error("FoE file listing is not implemented by the daemon backend");
}

void FoEService::fileInfo(int position, const QString &fileName) {
    Q_UNUSED(position);
    Q_UNUSED(fileName);
    if (!client_ || !client_->isConnected()) {
        emit error("Cannot query FoE file info: EtherCAT daemon is not connected");
        return;
    }
    emit error("FoE file info query is not implemented by the daemon backend");
}

// Read firmware from a slave using FoE protocol via the daemon.
void FoEService::readFirmware(int position, const QString &outputPath) {
    client_->foeRead(position, outputPath);
}

// Write firmware to a slave using FoE protocol via the daemon.
void FoEService::writeFirmware(int position, const QString &inputPath, uint32_t password) {
    client_->foeWrite(position, inputPath, password);
}
