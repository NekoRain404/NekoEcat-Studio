#pragma once
// DcSyncHandler — queries EtherCAT Distributed Clock synchronization status.
//
// Primary data comes from IgH CLI (`ethercat master` / `ethercat slaves -v`).
// When a real-time master handle is available (e.g. from FreeRunController),
// the handler can be enriched with ecrt API calls for tighter sync monitoring.

#include <QJsonObject>
#include <QVector>
#include <QString>

#include <cstdint>

// Forward declarations — avoids pulling ecrt.h into every translation unit.
struct ec_master;
typedef struct ec_master ec_master_t;

class EcatService;

// Per-slave DC sync information parsed from IgH CLI output.
struct DcSyncSlaveInfo {
    int position = -1;
    bool dcCapable = false;
    bool syncing = false;
    int64_t driftNs = 0;
    int64_t jitterMinNs = 0;
    int64_t jitterMaxNs = 0;
    int64_t jitterAvgNs = 0;
    QString refClockName;
};

class DcSyncHandler {
public:
    // Construct with an optional CLI backend for DC info queries.
    explicit DcSyncHandler(EcatService *backend = nullptr);

    // Set or replace the CLI backend after construction.
    void setBackend(EcatService *backend);

    // Handle the "dcSyncStatus" JSON-RPC request.  Returns a JSON object with
    // DC status fields.  Always returns valid JSON, even on error.
    QJsonObject handle(const QString &id, const QJsonObject &params);

    // Optional: enrich cached state from an active ecrt master.
    // Call periodically from a timer when FreeRunController is running.
    void update(ec_master_t *master, int slaveCount);

    // Parse ethercat slaves -v output for DC info (exposed for testing).
    QVector<DcSyncSlaveInfo> queryDcStatus(const QString &slaveVerboseOutput) const;

    // Detect reference clock slave index from ethercat master output (exposed for testing).
    int detectRefClock(const QString &masterOutput) const;

private:
    // Run a CLI command via the EcatService backend, or fall back to direct exec.
    QString runCliCommand(const QString &master, const QStringList &args) const;

    // Convert a slave info struct to a JSON object.
    QJsonObject slaveInfoToJson(const DcSyncSlaveInfo &info) const;

    EcatService *backend_ = nullptr;
};
