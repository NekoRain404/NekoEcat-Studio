#pragma once
// DcSyncHandler — queries EtherCAT Distributed Clock synchronization status.
//
// Primary data comes from IgH CLI (`ethercat master` / `ethercat slaves -v`).
// When a real-time master handle is available (e.g. from FreeRunController),
// the handler can be enriched with ecrt API calls for tighter sync monitoring.

#include <QJsonObject>
#include <QString>
#include <QVector>

#include <cstdint>
#include <mutex>

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

// Parsed DC configuration from an ESI XML descriptor.
struct DcConfig {
    uint32_t assignActivate = 0; // DC assign-activate word (e.g. 0x0300 for SYNC0+SYNC1).
    uint32_t sync0CycleNs = 0;   // SYNC0 cycle time in nanoseconds.
    int32_t sync0ShiftNs = 0;    // SYNC0 shift in nanoseconds.
    uint32_t sync1CycleNs = 0;   // SYNC1 cycle time in nanoseconds.
    int32_t sync1ShiftNs = 0;    // SYNC1 shift in nanoseconds.
};

class DcSyncHandler {
public:
    // Construct with an optional CLI backend for DC info queries.
    explicit DcSyncHandler(EcatService* backend = nullptr);

    // Set or replace the CLI backend after construction.
    void setBackend(EcatService* backend);

    // Handle the "dcSyncStatus" JSON-RPC request.  Returns a JSON object with
    // DC status fields.  Always returns valid JSON, even on error.
    QJsonObject handle(const QString& id, const QJsonObject& params);

    // Handle the "dcConfigure" JSON-RPC request.  Queries ESI XML to extract
    // DC assign-activate and cycle parameters for the specified slave.
    QJsonObject handleDcConfigure(const QString& id, const QJsonObject& params);

    // Handle the "dcActivate" JSON-RPC request.  Activates DC synchronization
    // by selecting a reference clock and enabling distributed clocks.
    QJsonObject handleDcActivate(const QString& id, const QJsonObject& params);

    // Handle the "dcDeactivate" JSON-RPC request.  Resets DC activation state.
    QJsonObject handleDcDeactivate(const QString& id, const QJsonObject& params);

    // Optional: enrich cached state from an active ecrt master.
    // Call periodically from a timer when FreeRunController is running.
    void update(ec_master_t* master, int slaveCount);

    // Parse ethercat slaves -v output for DC info (exposed for testing).
    QVector<DcSyncSlaveInfo> queryDcStatus(const QString& slaveVerboseOutput) const;

    // Detect reference clock slave index from ethercat master output (exposed for testing).
    int detectRefClock(const QString& masterOutput) const;

    // Parse DC configuration from ESI XML text (exposed for testing).
    DcConfig parseDcConfigFromXml(const QString& xmlText) const;

private:
    // Run a CLI command via the EcatService backend, or fall back to direct exec.
    QString runCliCommand(const QString& master, const QStringList& args) const;

    // Convert a slave info struct to a JSON object.
    QJsonObject slaveInfoToJson(const DcSyncSlaveInfo& info) const;

    // Convert a DcConfig struct to a JSON object.
    QJsonObject dcConfigToJson(const DcConfig& config) const;

    EcatService* backend_ = nullptr;

    // Real-time DC data from ecrt API (updated by update(), read by handle()).
    mutable std::mutex rtMutex_;
    uint32_t refClockTime_ = 0;
    uint32_t syncMaxDiff_ = 0;
    bool rtDataValid_ = false;

    // DC activation state.
    bool dcActivated_ = false;
    int dcRefClockSlave_ = -1; // Slave selected as reference clock.
    DcConfig activeDcConfig_;  // Last-applied DC configuration.
};
