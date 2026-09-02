#pragma once

// EoEHandler — Ethernet over EtherCAT (EoE) protocol operations.
//
// Handles EoE-related commands for the ecatd daemon:
//   - EoE status query (check slave EoE mailbox support)
//   - IP configuration via SDO writes to standard EoE objects (0x8000+)
//   - EoE statistics and state monitoring
//
// EoE enables Ethernet frame tunneling through the EtherCAT bus, allowing
// standard IP communication with EtherCAT slaves that have Ethernet ports.
//
// Thread safety:
//   Each handler invocation is called from the daemon's event loop thread.
//   SDO operations use the backend's thread-safe upload/download methods.

#include <QJsonObject>
#include <QString>

class EcatService;

class EoEHandler {
public:
    explicit EoEHandler(EcatService* backend);

    // Query EoE support status for a slave.
    // Checks slave ESI/XML for EoE mailbox protocol capability.
    // params: { "master": string, "position": int }
    // Returns: { "supported": bool, "hasIpConfig": bool, "macAddress": string }
    QJsonObject handleEoeStatus(const QString& id, const QJsonObject& params);

    // Configure IP address for an EoE-capable slave via SDO.
    // Writes to standard EoE IP configuration objects (0x8000+).
    // params: { "master": string, "position": int, "ip": string, "subnet": string,
    //           "gateway": string (optional), "dns": string (optional) }
    // Returns: { "success": true, "ip": string, "subnet": string }
    QJsonObject handleEoeConfigureIp(const QString& id, const QJsonObject& params);

    // Read current IP configuration from an EoE slave via SDO.
    // params: { "master": string, "position": int }
    // Returns: { "ip": string, "subnet": string, "gateway": string, "dns": string }
    QJsonObject handleEoeGetIp(const QString& id, const QJsonObject& params);

    // Get EoE statistics (frame counts, error counts) for a slave.
    // params: { "master": string, "position": int }
    // Returns: { "txFrames": int, "rxFrames": int, "txErrors": int, "rxErrors": int }
    QJsonObject handleEoeStats(const QString& id, const QJsonObject& params);

private:
    EcatService* backend_;

    // Parse an IP address string into 4 bytes. Returns false if invalid.
    static bool parseIp(const QString& ip, uint8_t out[4]);

    // Encode 4 bytes into an IP address string.
    static QString formatIp(const uint8_t data[4]);

    // Read an EoE SDO (uint32) from a slave. Returns the raw value.
    bool readEoeSdo(const QString& master, int position, const QString& index, const QString& subIndex, uint32_t* value,
                    QString* error) const;

    // Write an EoE SDO (uint32) to a slave.
    bool writeEoeSdo(const QString& master, int position, const QString& index, const QString& subIndex, uint32_t value,
                     QString* error) const;

    // Check if a slave supports EoE by examining its ESI/XML description.
    bool slaveHasEoe(const QString& master, int position, QString* error) const;
};
