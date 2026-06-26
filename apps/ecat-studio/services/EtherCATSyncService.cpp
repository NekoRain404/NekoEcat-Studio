#include "EtherCATSyncService.h"

// EtherCATSyncService.cpp — Time, data, state, and configuration sync facade
//
// Implementation notes:
//   - Uses EventBus and EcatClient for sync orchestration
//   - Tracks sync count and last sync timestamp in SyncStatus struct
//   - Rejects offline sync attempts instead of synthesizing success

EtherCATSyncService::EtherCATSyncService(EventBus *bus, EcatClient *client,
                                          QObject *parent)
    : QObject(parent), bus_(bus), client_(client)
{
}

bool EtherCATSyncService::syncTime()
{
    if (!backendReady())
        return false;

    QDateTime now = QDateTime::currentDateTime();
    status_.lastSync = now;
    status_.syncCount++;
    emit timeSynced(now);
    return true;
}

bool EtherCATSyncService::syncData()
{
    if (!backendReady())
        return false;

    status_.lastSync = QDateTime::currentDateTime();
    status_.syncCount++;
    emit dataSynced(0);
    return true;
}

bool EtherCATSyncService::syncState()
{
    if (!backendReady())
        return false;

    status_.lastSync = QDateTime::currentDateTime();
    status_.syncCount++;
    return true;
}

bool EtherCATSyncService::syncConfiguration()
{
    if (!backendReady())
        return false;

    status_.lastSync = QDateTime::currentDateTime();
    status_.syncCount++;
    return true;
}

SyncStatus EtherCATSyncService::syncStatus() const
{
    return status_;
}

bool EtherCATSyncService::backendReady() const
{
    // No real sync backend is wired yet; keep success paths unreachable.
    return false;
}
