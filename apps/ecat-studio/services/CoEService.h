#pragma once

// CoEService — CANopen over EtherCAT protocol support.
//
// Provides SDO information service, dictionary upload, segment transfer,
// and handles emergency and timestamp objects from slaves.
//
// This service provides CANopen over EtherCAT (CoE) protocol support.
// It handles:
//   - SDO information service (vendor, product, revision, serial)
//   - SDO dictionary upload from slaves
//   - Segment transfer for large SDOs
//   - Emergency object handling
//   - Timestamp object handling
//
// Usage:
//   ServiceContainer *container = ...;
//   CoEService *coe = container->coe();
//   coe->uploadSdoInfo(0);  // Get SDO info for slave 0
//   coe->uploadDictionary(0);  // Get SDO dictionary for slave 0
//   coe->uploadSegment(0, "0x6000", 0, 1024);  // Upload segment
//   coe->downloadSegment(0, "0x6000", 0, data);  // Download segment
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   marshals daemon communication to the main thread internally.
//
// Performance:
//   - SDO info upload is O(1)
//   - Dictionary upload is O(n) where n is number of SDOs
//   - Segment transfer is O(n) where n is segment size

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QDateTime>
#include <QJsonObject>
#include <QStringList>

class EcatClient;

// SDO information structure.
struct SdoInfo {
    quint32 vendorId = 0;           // Vendor ID
    quint32 productCode = 0;        // Product code
    quint32 revisionNumber = 0;     // Revision number
    quint32 serialNumber = 0;       // Serial number
    QStringList supportedCoEObjects; // Supported CoE objects
};
Q_DECLARE_METATYPE(SdoInfo)

// CoE SDO dictionary entry.
struct CoESdoDictionary {
    QString index;      // SDO index in hex format
    QString name;       // SDO name
    QString type;       // Data type
    int bitSize = 0;    // Bit size
    int accessType = 0; // Access type (ro, wo, rw)
};
Q_DECLARE_METATYPE(CoESdoDictionary)

class CoEService : public QObject {
    Q_OBJECT
public:
    explicit CoEService(EcatClient *client, QObject *parent = nullptr);

    // Upload SDO information from a slave.
    // @param position  Slave position
    void uploadSdoInfo(int position);

    // Upload SDO dictionary from a slave.
    // @param position  Slave position
    void uploadDictionary(int position);

    // Upload a segment of an SDO.
    // @param position  Slave position
    // @param index     SDO index in hex format
    // @param offset    Segment offset in bytes
    // @param size      Segment size in bytes
    void uploadSegment(int position, const QString &index, int offset, int size);

    // Download a segment of an SDO.
    // @param position  Slave position
    // @param index     SDO index in hex format
    // @param offset    Segment offset in bytes
    // @param data      Segment data
    void downloadSegment(int position, const QString &index, int offset,
                         const QByteArray &data);

    // Read Error Register (0x1001) — standard CANopen error register.
    // Returns a JSON object with "success", "value" (uint8), and decoded
    // bit flags: genericError, current, voltage, temperature,
    // communicationError, deviceProfileSpecific, manufacturerSpecific.
    // On failure returns {"success": false, "error": "..."}.
    QJsonObject readErrorRegister(int slavePosition);

    // Read Pre-defined Error Field (0x1003) — error history.
    // Subindex 0x00 = number of stored errors; 0x01..N = error codes (uint32).
    // @param slavePosition  Slave position on the bus
    // @param maxEntries     Maximum error history entries to read (default 10)
    // Returns {"success": true, "errorCount": N, "errors": [...]} on success.
    QJsonObject readErrorHistory(int slavePosition, int maxEntries = 10);

    // Read all emergency info for a slave (error register + error history).
    // Combines readErrorRegister() and readErrorHistory() into a single response.
    // Returns {"success": true, "errorRegister": {...}, "errorHistory": {...}}.
    QJsonObject readEmergencyInfo(int slavePosition);

signals:
    // Emitted when SDO information is received.
    // @param position  Slave position
    // @param info      SdoInfo structure
    void sdoInfoReceived(int position, const SdoInfo &info);

    // Emitted when SDO dictionary is received.
    // @param position  Slave position
    // @param entries   List of CoESdoDictionary entries
    void dictionaryReceived(int position, const QList<CoESdoDictionary> &entries);

    // Emitted when a segment is received.
    // @param position  Slave position
    // @param index     SDO index
    // @param data      Segment data
    void segmentReceived(int position, const QString &index, const QByteArray &data);

    // Emitted when a segment download completes.
    // @param position  Slave position
    // @param index     SDO index
    // @param success   Whether download was successful
    void segmentDownloaded(int position, const QString &index, bool success);

    // Emitted when an emergency object is received.
    // @param position   Slave position
    // @param errorCode  Emergency error code
    // @param data       Emergency data
    void emergencyReceived(int position, int errorCode, const QByteArray &data);

    // Emitted when a timestamp object is received.
    // @param position   Slave position
    // @param timestamp  Timestamp value
    void timestampReceived(int position, const QDateTime &timestamp);

    // Emitted when an error occurs.
    // @param message  Human-readable error message
    void error(const QString &message);

private:
    EcatClient *client_;  // TCP client to ecatd daemon
};
