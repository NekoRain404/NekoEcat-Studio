#pragma once

// MultiMasterService — manages offline multi-master inventory drafts.
//
// Provides local master discovery/configuration/monitoring metadata. Runtime
// synchronization is rejected until this service is wired to a live multi-master
// EtherCAT backend.

#include <QObject>
#include <QString>
#include <QVector>

class EcatClient;
class EventBus;

enum class MultiMasterState {
    Unknown,
    Idle,
    Active,
    Error,
    Syncing,
};

struct MmMasterInfo {
    int masterId = -1;
    QString adapterName;
    int slaveCount = 0;
    MultiMasterState state = MultiMasterState::Unknown;
    QString ipAddress;
    QString macAddress;
};

struct MmMasterConfig {
    QString adapterName;
    int cycleTime = 1000;
    int sync0Time = 0;
    int watchdogTimeout = 1000;
    int debugLevel = 0;
};

struct MmMasterStatus {
    int masterId = -1;
    MultiMasterState state = MultiMasterState::Unknown;
    int slaveCount = 0;
    int errorCount = 0;
    double cycleTimeUs = 0.0;
    double jitterUs = 0.0;
    QString summary;
};

struct MmMasterSyncResult {
    bool success = false;
    int sourceId = -1;
    int targetId = -1;
    QString message;
    int recordsSynced = 0;
};

class MultiMasterService : public QObject {
    Q_OBJECT
public:
    explicit MultiMasterService(EcatClient* client, EventBus* eventBus, QObject* parent = nullptr);

    QVector<MmMasterInfo> discoverMasters();
    bool configureMaster(int masterId, const MmMasterConfig& config);
    MmMasterStatus monitorMaster(int masterId);
    // Returns false until real backend synchronization is implemented.
    bool synchronizeMasters(int sourceId, int targetId);
    bool addMaster(const MmMasterInfo& info);
    bool removeMaster(int masterId);
    MmMasterInfo masterInfo(int masterId) const;
    QVector<MmMasterInfo> allMasters() const;
    int masterCount() const;
    void refresh();

signals:
    void masterDiscovered(const MmMasterInfo& info);
    void masterStatusChanged(int masterId, const MmMasterStatus& status);
    void masterSyncCompleted(const MmMasterSyncResult& result);
    void masterError(int masterId, const QString& error);

private:
    EcatClient* client_;
    EventBus* eventBus_;
    QVector<MmMasterInfo> masters_;
    int nextMasterId_ = 0;
};
