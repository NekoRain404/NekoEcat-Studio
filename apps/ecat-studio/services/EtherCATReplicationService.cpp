#include "EtherCATReplicationService.h"

// EtherCATReplicationService.cpp — Configuration, data, and state replication facade
//
// Implementation notes:
//   - Uses EventBus and EcatClient for replication orchestration
//   - Future backend success paths emit replicationStarted/replicationCompleted per target
//   - Rejects offline replication attempts instead of synthesizing success history

EtherCATReplicationService::EtherCATReplicationService(EventBus *bus,
                                                        EcatClient *client,
                                                        QObject *parent)
    : QObject(parent), bus_(bus), client_(client)
{
}

bool EtherCATReplicationService::replicateConfiguration(const QStringList &targets)
{
    return rejectReplication(targets);
}

bool EtherCATReplicationService::replicateData(const QStringList &targets)
{
    return rejectReplication(targets);
}

bool EtherCATReplicationService::replicateState(const QStringList &targets)
{
    return rejectReplication(targets);
}

bool EtherCATReplicationService::replicateBackup(const QStringList &targets)
{
    return rejectReplication(targets);
}

QVector<ReplicationStatus> EtherCATReplicationService::replicationHistory() const
{
    return history_;
}

bool EtherCATReplicationService::rejectReplication(
    const QStringList &targets) const
{
    return targets.isEmpty();
}
