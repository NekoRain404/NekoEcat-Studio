#include "MultiMasterService.h"
#include "EventBus.h"
#include "infra/EcatClient.h"

MultiMasterService::MultiMasterService(EcatClient* client, EventBus* eventBus, QObject* parent)
    : QObject(parent), client_(client), eventBus_(eventBus) {
    if (client_) {
        connect(client_, &EcatClient::connected, this, [this]() { refresh(); });
        connect(client_, &EcatClient::disconnected, this, [this]() {
            for (auto& m : masters_) {
                m.state = MultiMasterState::Unknown;
            }
        });
    }
}

QVector<MmMasterInfo> MultiMasterService::discoverMasters() {
    QVector<MmMasterInfo> discovered;

    for (const auto& m : masters_) {
        discovered.append(m);
    }

    return discovered;
}

bool MultiMasterService::configureMaster(int masterId, const MmMasterConfig& config) {
    for (auto& m : masters_) {
        if (m.masterId == masterId) {
            if (!config.adapterName.isEmpty()) {
                m.adapterName = config.adapterName;
            }
            m.state = MultiMasterState::Idle;
            return true;
        }
    }
    return false;
}

MmMasterStatus MultiMasterService::monitorMaster(int masterId) {
    MmMasterStatus status;
    status.masterId = masterId;

    for (const auto& m : masters_) {
        if (m.masterId == masterId) {
            status.state = m.state;
            status.slaveCount = m.slaveCount;
            status.summary = QString("Master %1: %2 slaves, state=%3")
                                 .arg(masterId)
                                 .arg(m.slaveCount)
                                 .arg(static_cast<int>(m.state));
            emit masterStatusChanged(masterId, status);
            return status;
        }
    }

    status.state = MultiMasterState::Unknown;
    status.summary = "Master not found";
    return status;
}

bool MultiMasterService::synchronizeMasters(int sourceId, int targetId) {
    bool hasSource = false;
    bool hasTarget = false;
    for (const auto& m : masters_) {
        if (m.masterId == sourceId)
            hasSource = true;
        if (m.masterId == targetId)
            hasTarget = true;
    }

    if (!hasSource || !hasTarget) {
        MmMasterSyncResult result;
        result.success = false;
        result.sourceId = sourceId;
        result.targetId = targetId;
        result.message = "Source or target master not found";
        emit masterSyncCompleted(result);
        return false;
    }

    MmMasterSyncResult result;
    result.success = false;
    result.sourceId = sourceId;
    result.targetId = targetId;
    result.recordsSynced = 0;
    result.message = "Multi-master synchronization requires a connected EtherCAT backend";

    emit masterSyncCompleted(result);
    return false;
}

bool MultiMasterService::addMaster(const MmMasterInfo& info) {
    for (const auto& m : masters_) {
        if (m.masterId == info.masterId)
            return false;
    }

    MmMasterInfo newMaster = info;
    if (newMaster.masterId < 0) {
        newMaster.masterId = nextMasterId_++;
    }
    masters_.append(newMaster);
    emit masterDiscovered(newMaster);
    return true;
}

bool MultiMasterService::removeMaster(int masterId) {
    for (int i = 0; i < masters_.size(); ++i) {
        if (masters_[i].masterId == masterId) {
            masters_.removeAt(i);
            return true;
        }
    }
    return false;
}

MmMasterInfo MultiMasterService::masterInfo(int masterId) const {
    for (const auto& m : masters_) {
        if (m.masterId == masterId)
            return m;
    }
    return {};
}

QVector<MmMasterInfo> MultiMasterService::allMasters() const {
    return masters_;
}

int MultiMasterService::masterCount() const {
    return masters_.size();
}

void MultiMasterService::refresh() {
    discoverMasters();
}
