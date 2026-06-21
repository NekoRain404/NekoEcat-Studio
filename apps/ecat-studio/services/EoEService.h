#pragma once

// EoEService — Ethernet over EtherCAT protocol support.
//
// Provides Ethernet frame tunneling, IP address configuration,
// and MAC address learning for EtherCAT slaves that support EoE.
//
// This service provides Ethernet over EtherCAT (EoE) protocol support.
// It handles:
//   - Ethernet frame tunneling through EtherCAT
//   - IP address configuration for slaves
//   - MAC address learning from slaves
//   - Frame send/receive operations
//
// Usage:
//   ServiceContainer *container = ...;
//   EoEService *eoe = container->eoe();
//   eoe->sendEthernetFrame(0, frameData);  // Send frame to slave 0
//   eoe->receiveEthernetFrame(0);  // Receive frame from slave 0
//   eoe->configureIp(0, "192.168.1.100", "255.255.255.0");  // Configure IP
//   eoe->learnedMacs(0);  // Get learned MACs from slave 0
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   marshals daemon communication to the main thread internally.
//
// Performance:
//   - Frame send/receive is O(n) where n is frame size
//   - IP configuration is O(1)
//   - MAC learning is O(1) for request, O(n) for response

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QStringList>

class EcatClient;

class EoEService : public QObject {
    Q_OBJECT
public:
    explicit EoEService(EcatClient *client, QObject *parent = nullptr);

    // Send an Ethernet frame to a slave.
    // @param position  Slave position
    // @param frame     Ethernet frame data
    // @return true if frame was sent successfully
    bool sendEthernetFrame(int position, const QByteArray &frame);

    // Receive an Ethernet frame from a slave.
    // @param position  Slave position
    void receiveEthernetFrame(int position);

    // Configure IP address for a slave.
    // @param position  Slave position
    // @param ip        IP address
    // @param subnet    Subnet mask
    // @return true if configuration was successful
    bool configureIp(int position, const QString &ip, const QString &subnet);

    // Get learned MAC addresses from a slave.
    // @param position  Slave position
    void learnedMacs(int position);

signals:
    // Emitted when a frame is sent.
    // @param position  Slave position
    // @param success   Whether send was successful
    void frameSent(int position, bool success);

    // Emitted when a frame is received.
    // @param position  Slave position
    // @param frame     Ethernet frame data
    void frameReceived(int position, const QByteArray &frame);

    // Emitted when IP is configured.
    // @param position  Slave position
    // @param ip        Configured IP address
    void ipConfigured(int position, const QString &ip);

    // Emitted when MAC list is received.
    // @param position  Slave position
    // @param macs      List of learned MAC addresses
    void macListReceived(int position, const QStringList &macs);

    // Emitted when an error occurs.
    // @param message  Human-readable error message
    void error(const QString &message);

private:
    EcatClient *client_;  // TCP client to ecatd daemon
};
