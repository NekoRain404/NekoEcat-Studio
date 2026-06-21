#include "EoEService.h"
#include "EcatClient.h"

// EoEService.cpp — Ethernet over EtherCAT (EoE) frame and IP management
//
// Implementation notes:
//   - Wraps EcatClient error forwarding for unified error signaling
//   - Frame send/receive and IP configuration are currently stubbed
//   - MAC learning returns simulated address lists

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
    emit frameSent(position, true);
    return true;
}

void EoEService::receiveEthernetFrame(int position) {
    QByteArray frame;
    frame.fill(0, 64);
    emit frameReceived(position, frame);
}

bool EoEService::configureIp(int position, const QString &ip,
                              const QString &subnet) {
    Q_UNUSED(subnet);
    emit ipConfigured(position, ip);
    return true;
}

void EoEService::learnedMacs(int position) {
    QStringList macs;
    macs << "00:11:22:33:44:55" << "AA:BB:CC:DD:EE:FF";
    emit macListReceived(position, macs);
}
