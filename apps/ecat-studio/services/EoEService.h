#pragma once

// EoEService — Ethernet over EtherCAT protocol support.
//
// Provides Ethernet frame tunneling, IP address configuration,
// and status monitoring for EtherCAT slaves that support EoE.
//
// This service handles:
//   - EoE status queries (slave EoE capability detection)
//   - IP address configuration via standard EoE SDO objects (0x8000+)
//   - IP address readback
//   - EoE frame statistics monitoring
//   - Frame send/receive (requires TAP/TUN daemon backend — planned)
//
// Usage:
//   ServiceContainer *container = ...;
//   EoEService *eoe = container->eoe();
//   eoe->queryStatus(0);           // Check slave 0 EoE support
//   eoe->configureIp(0, "192.168.1.100", "255.255.255.0");
//   eoe->queryIp(0);               // Read back current IP
//   eoe->queryStats(0);            // Get frame statistics
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   marshals daemon communication to the main thread internally.
//
// Performance:
//   - Status/IP queries are O(1) round-trip to daemon
//   - IP configuration is O(1) SDO writes
//   - Frame send/receive is O(n) where n is frame size (when implemented)

#include <QByteArray>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QStringList>

class EcatClient;

class EoEService : public QObject {
    Q_OBJECT
public:
    explicit EoEService(EcatClient* client, QObject* parent = nullptr);

    // Send an Ethernet frame to a slave.
    // @param position  Slave position
    // @param frame     Ethernet frame data
    // @return true if frame was sent successfully (currently always false)
    bool sendEthernetFrame(int position, const QByteArray& frame);

    // Receive an Ethernet frame from a slave.
    // @param position  Slave position
    void receiveEthernetFrame(int position);

    // Configure IP address for a slave via EoE SDO.
    // @param position  Slave position
    // @param ip        IP address (e.g. "192.168.1.100")
    // @param subnet    Subnet mask (e.g. "255.255.255.0")
    // @return true if request was sent successfully
    bool configureIp(int position, const QString& ip, const QString& subnet);

    // Get learned MAC addresses from a slave.
    // @param position  Slave position
    void learnedMacs(int position);

    // Query EoE support status for a slave.
    // Emits statusReceived() with the result.
    // @param position  Slave position
    void queryStatus(int position);

    // Read current IP configuration from a slave.
    // Emits ipReadback() with the result.
    // @param position  Slave position
    void queryIp(int position);

    // Query EoE frame statistics for a slave.
    // Emits statsReceived() with the result.
    // @param position  Slave position
    void queryStats(int position);

signals:
    // Emitted when EoE status query completes.
    // @param position  Slave position
    // @param data      Status JSON: { supported, hasIpConfig, currentIp }
    void statusReceived(int position, const QJsonObject& data);

    // Emitted when IP configuration succeeds.
    // @param position  Slave position
    // @param ip        Configured IP address
    void ipConfigured(int position, const QString& ip);

    // Emitted when IP readback query completes.
    // @param position  Slave position
    // @param data      IP config JSON: { ip, subnet, gateway, dns }
    void ipReadback(int position, const QJsonObject& data);

    // Emitted when EoE statistics query completes.
    // @param position  Slave position
    // @param data      Stats JSON: { txFrames, rxFrames, txErrors, rxErrors }
    void statsReceived(int position, const QJsonObject& data);

    // Emitted when a frame is sent.
    // @param position  Slave position
    // @param success   Whether send was successful
    void frameSent(int position, bool success);

    // Emitted when a frame is received.
    // @param position  Slave position
    // @param frame     Ethernet frame data
    void frameReceived(int position, const QByteArray& frame);

    // Emitted when an error occurs.
    // @param message  Human-readable error message
    void error(const QString& message);

private:
    EcatClient* client_; // TCP client to ecatd daemon
};
