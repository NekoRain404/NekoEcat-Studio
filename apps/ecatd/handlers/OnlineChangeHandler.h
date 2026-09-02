#pragma once

// OnlineChangeHandler — runtime configuration changes without full bus restart.
//
// TwinCAT-style "online change" applies configuration changes (startup SDOs,
// PDO remapping, slave parameters) to a running bus with minimal disruption.
//
// IgH does not support true hot-reconfiguration of the process image at runtime,
// but this handler provides a targeted partial-reconfiguration workflow:
//   1. Identify which slaves are affected by the change
//   2. Transition only affected slaves to PREOP (others stay in OP)
//   3. Apply SDO/parameter changes while in PREOP
//   4. Bring affected slaves back to OP
//
// This minimizes downtime compared to a full INIT→OP cycle of the entire bus.
//
// Operations:
//   - preview: compute the change plan (affected slaves, operations) without applying
//   - apply: execute the change plan
//   - status: report whether an online change is in progress
//
// Thread safety:
//   Called from the daemon event loop thread. State transitions and SDO writes
//   block until complete or timeout.

#include <QJsonArray>
#include <QJsonObject>
#include <QString>

class EcatService;

class OnlineChangeHandler {
public:
    explicit OnlineChangeHandler(EcatService* backend);

    // Preview an online change: compute which slaves are affected and what
    // operations would be performed, without applying anything.
    // params: { "master": string, "changes": [ {position, index, subIndex, value, type} ] }
    // Returns: { "affectedSlaves": [int], "operations": [...], "estimatedDowntimeMs": int }
    QJsonObject handlePreview(const QString& id, const QJsonObject& params);

    // Apply an online change: transition affected slaves to PREOP, write the
    // SDO changes, then return them to OP.
    // params: { "master": string, "changes": [...], "targetState": string (optional, default "OP") }
    // Returns: { "success": bool, "applied": int, "failed": int, "results": [...] }
    QJsonObject handleApply(const QString& id, const QJsonObject& params);

    // Query whether an online change is currently in progress.
    // params: { "master": string }
    // Returns: { "inProgress": bool, "phase": string }
    QJsonObject handleStatus(const QString& id, const QJsonObject& params);

private:
    EcatService* backend_;

    // Tracks whether a change is mid-flight (guards against concurrent applies).
    bool inProgress_ = false;
    QString currentPhase_ = "idle";

    // Extract the unique set of affected slave positions from a changes array.
    QJsonArray affectedSlaves(const QJsonArray& changes) const;
};
