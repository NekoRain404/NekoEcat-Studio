#include "EoEService.h"
#include "EcatClient.h"

// EoEService.cpp — Ethernet over EtherCAT (EoE) frame and IP management
//
// Implementation notes:
//   - Delegates to EcatClient for daemon communication
//   - IP configuration uses standard EoE SDO objects (0x8000+)
//   - Status and statistics queries via daemon JSON-RPC commands
//   - Frame send/receive require daemon backend support (TAP/TUN integration)

EoEService::EoEService(EcatClient *client, QObject *parent)
    : QObject(parent), client_(client) {
    // Forward EoE-specific signals from the client.
    connect(client_, &EcatClient::eoeStatusResult, this,
            [this](int pos, const QJsonObject &data) {
                emit statusReceived(pos, data);
            });
    connect(client_, &EcatClient::eoeIpConfigured, this,
            [this](int pos, const QString &ip) {
                emit ipConfigured(pos, ip);
            });
    connect(client_, &EcatClient::eoeIpResult, this,
            [this](int pos, const QJsonObject &data) {
                emit ipReadback(pos, data);
            });
    connect(client_, &EcatClient::eoeStatsResult, this,
            [this](int pos, const QJsonObject &data) {
                emit statsReceived(pos, data);
            });
    connect(client_, &EcatClient::errorMessage, this,
            [this](const QString &msg) { emit error(msg); });
}

bool EoEService::sendEthernetFrame(int position, const QByteArray &frame) {
    if (frame.isEmpty()) {
        emit error("Cannot send empty Ethernet frame");
        return false;
    }
    if (!client_ || !client_->isConnected()) {
        emit error("Cannot send EoE frame: EtherCAT daemon is not connected");
        emit frameSent(position, false);
        return false;
    }
    // Frame forwarding requires TAP/TUN integration in the daemon.
    // This is a planned enhancement; currently returns an error.
    emit error("EoE frame send requires TAP/TUN daemon backend support");
    emit frameSent(position, false);
    return false;
}

void EoEService::receiveEthernetFrame(int position) {
    Q_UNUSED(position);
    if (!client_ || !client_->isConnected()) {
        emit error("Cannot receive EoE frame: EtherCAT daemon is not connected");
        return;
    }
    emit error("EoE frame receive requires TAP/TUN daemon backend support");
}

bool EoEService::configureIp(int position, const QString &ip,
                              const QString &subnet) {
    if (!client_ || !client_->isConnected()) {
        emit error("Cannot configure EoE IP: EtherCAT daemon is not connected");
        return false;
    }
    client_->eoeConfigureIp(position, ip, subnet);
    return true;
}

void EoEService::learnedMacs(int position) {
    if (!client_ || !client_->isConnected()) {
        emit error("Cannot query EoE MAC list: EtherCAT daemon is not connected");
        return;
    }
    // MAC learning is not yet implemented in the daemon.
    emit error("EoE MAC learning is not yet implemented");
}

void EoEService::queryStatus(int position) {
    if (!client_ || !client_->isConnected()) {
        emit error("Cannot query EoE status: EtherCAT daemon is not connected");
        return;
    }
    client_->eoeStatus(position);
}

void EoEService::queryIp(int position) {
    if (!client_ || !client_->isConnected()) {
        emit error("Cannot query EoE IP: EtherCAT daemon is not connected");
        return;
    }
    client_->eoeGetIp(position);
}

void EoEService::queryStats(int position) {
    if (!client_ || !client_->isConnected()) {
        emit error("Cannot query EoE stats: EtherCAT daemon is not connected");
        return;
    }
    client_->eoeStats(position);
}
