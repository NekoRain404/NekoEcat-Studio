#include "EoEService.h"
#include "EcatClient.h"

// EoEService.cpp — Ethernet over EtherCAT (EoE) frame and IP management
//
// Implementation notes:
//   - Wraps EcatClient error forwarding for unified error signaling
//   - Frame send/receive and IP configuration require a connected daemon
//   - MAC learning is unavailable until daemon-backed EoE support is wired in

EoEService::EoEService(EcatClient *client, QObject *parent)
    : QObject(parent), client_(client) {
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
    emit error("EoE frame send requires daemon backend support");
    emit frameSent(position, false);
    return false;
}

void EoEService::receiveEthernetFrame(int position) {
    Q_UNUSED(position);
    if (!client_ || !client_->isConnected()) {
        emit error("Cannot receive EoE frame: EtherCAT daemon is not connected");
        return;
    }
    emit error("EoE frame receive requires daemon backend support");
}

bool EoEService::configureIp(int position, const QString &ip,
                              const QString &subnet) {
    Q_UNUSED(position);
    Q_UNUSED(ip);
    Q_UNUSED(subnet);
    if (!client_ || !client_->isConnected()) {
        emit error("Cannot configure EoE IP: EtherCAT daemon is not connected");
        return false;
    }
    emit error("EoE IP configuration requires daemon backend support");
    return false;
}

void EoEService::learnedMacs(int position) {
    Q_UNUSED(position);
    if (!client_ || !client_->isConnected()) {
        emit error("Cannot query EoE MAC list: EtherCAT daemon is not connected");
        return;
    }
    emit error("EoE MAC learning requires daemon backend support");
}
