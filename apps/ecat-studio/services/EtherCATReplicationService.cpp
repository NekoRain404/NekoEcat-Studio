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
    if (targets.isEmpty())
        return true;
    if (!backendReady())
        return false;

    for (const auto &target : targets) {
        emit replicationStarted(target);
        ReplicationStatus s;
        s.target = target;
        s.status = QStringLiteral("Success");
        s.startTime = QDateTime::currentDateTime();
        s.endTime = s.startTime;
        history_.append(s);
        emit replicationCompleted(target, true);
    }
    return true;
}

bool EtherCATReplicationService::replicateData(const QStringList &targets)
{
    if (targets.isEmpty())
        return true;
    if (!backendReady())
        return false;

    for (const auto &target : targets) {
        emit replicationStarted(target);
        ReplicationStatus s;
        s.target = target;
        s.status = QStringLiteral("Success");
        s.startTime = QDateTime::currentDateTime();
        s.endTime = s.startTime;
        history_.append(s);
        emit replicationCompleted(target, true);
    }
    return true;
}

bool EtherCATReplicationService::replicateState(const QStringList &targets)
{
    if (targets.isEmpty())
        return true;
    if (!backendReady())
        return false;

    for (const auto &target : targets) {
        emit replicationStarted(target);
        ReplicationStatus s;
        s.target = target;
        s.status = QStringLiteral("Success");
        s.startTime = QDateTime::currentDateTime();
        s.endTime = s.startTime;
        history_.append(s);
        emit replicationCompleted(target, true);
    }
    return true;
}

bool EtherCATReplicationService::replicateBackup(const QStringList &targets)
{
    if (targets.isEmpty())
        return true;
    if (!backendReady())
        return false;

    for (const auto &target : targets) {
        emit replicationStarted(target);
        ReplicationStatus s;
        s.target = target;
        s.status = QStringLiteral("Success");
        s.startTime = QDateTime::currentDateTime();
        s.endTime = s.startTime;
        history_.append(s);
        emit replicationCompleted(target, true);
    }
    return true;
}

QVector<ReplicationStatus> EtherCATReplicationService::replicationHistory() const
{
    return history_;
}

bool EtherCATReplicationService::backendReady() const
{
    // No real replication backend is wired yet; keep success paths unreachable.
    return false;
}
