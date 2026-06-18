#pragma once

// DcSyncHandler — queries Distributed Clock sync status for slaves.
//
// Primary data comes from parsing `ethercat master` CLI output, which includes
// DC reference clock, application time, and sync state.  When a real-time
// master handle is available (e.g. from FreeRunController), the handler can be
// enriched with ecrt API calls for tighter sync monitoring.

#include <QJsonObject>
#include <QString>
#include <QVector>

#include <cstdint>

// Forward declarations — avoids pulling ecrt.h into every translation unit.
struct ec_master;
typedef struct ec_master ec_master_t;

class EcatService;

class DcSyncHandler {
public:
    // Construct with a CLI backend for DC info queries.
    explicit DcSyncHandler(EcatService *backend = nullptr);

    // Handle the "dcSyncStatus" JSON-RPC request.  Returns a JSON object with
    // DC status fields.  Always returns valid JSON, even on error.
    QJsonObject handle(const QString &id, const QJsonObject &params);

    // Optional: enrich cached state from an active ecrt master.
    // Call periodically from a timer when FreeRunController is running.
    void update(ec_master_t *master, int slaveCount);

private:
    struct DcSlaveInfo {
        int position = -1;
        bool dcCapable = false;
        bool syncing = false;
        int64_t driftNs = 0;
        int64_t jitterNs = 0;
    };

    QJsonObject buildResponse(const QString &id) const;
    bool parseDcFromMasterText(const QString &text);
    void resetCache();

    EcatService *backend_ = nullptr;

    // Cached DC state.
    bool dcAvailable_ = false;
    int refClockPosition_ = -1;
    bool hasReferenceClock_ = false;
    QString applicationTime_;
    QString systemTimeDiff_;
    QString rawMasterText_;
    QVector<DcSlaveInfo> slaveInfo_;
};
