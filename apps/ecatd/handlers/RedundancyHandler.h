#pragma once

// RedundancyHandler — EtherCAT cable redundancy operations.
//
// Handles redundancy-related commands for the ecatd daemon:
//   - Redundancy status query (link state, active path)
//   - Enable/disable redundancy
//   - Failover to secondary path
//   - Failback to primary path
//   - Redundancy event history
//
// IgH EtherCAT master supports cable redundancy natively when configured.
// The handler queries master state for link status and provides failover
// control through the ecrt API or CLI.

#include <QJsonObject>
#include <QString>
#include <QVector>
#include <QDateTime>

class EcatService;

// Redundancy event record.
struct RedundancyEventRecord {
    QString type;           // "failover", "failback", "link_down", "link_up"
    int pathId = -1;        // Affected path (0=primary, 1=secondary)
    QString reason;         // Human-readable reason
    QDateTime timestamp;    // When the event occurred
    bool success = false;   // Whether the operation succeeded
};

class RedundancyHandler {
public:
    explicit RedundancyHandler(EcatService *backend);

    // Query redundancy status.
    // Returns current state, path info, and whether redundancy is active.
    // params: { "master": string }
    // Returns: { "enabled": bool, "state": string, "primaryPath": {...}, "secondaryPath": {...} }
    QJsonObject handleStatus(const QString &id, const QJsonObject &params);

    // Enable cable redundancy.
    // params: { "master": string }
    // Returns: { "success": true, "message": string }
    QJsonObject handleEnable(const QString &id, const QJsonObject &params);

    // Disable cable redundancy.
    // params: { "master": string }
    // Returns: { "success": true, "message": string }
    QJsonObject handleDisable(const QString &id, const QJsonObject &params);

    // Perform failover to secondary path.
    // params: { "master": string }
    // Returns: { "success": true, "fromPath": int, "toPath": int }
    QJsonObject handleFailover(const QString &id, const QJsonObject &params);

    // Perform failback to primary path.
    // params: { "master": string }
    // Returns: { "success": true, "fromPath": int, "toPath": int }
    QJsonObject handleFailback(const QString &id, const QJsonObject &params);

    // Get redundancy event history.
    // params: { "master": string, "limit": int (optional, default 100) }
    // Returns: { "events": [...] }
    QJsonObject handleHistory(const QString &id, const QJsonObject &params);

private:
    EcatService *backend_;

    // Event history (capped at kMaxHistory).
    QVector<RedundancyEventRecord> history_;
    static constexpr int kMaxHistory = 500;

    // Add an event to history.
    void addEvent(const QString &type, int pathId, const QString &reason, bool success);

    // Run a CLI command and capture output.
    QString runCliCommand(const QString &master, const QStringList &args, QString *error) const;

    // Parse link status from master info output.
    bool parseLinkStatus(const QString &masterOutput, bool *primaryUp, bool *secondaryUp) const;
};
