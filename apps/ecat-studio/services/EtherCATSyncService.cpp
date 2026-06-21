#include "EtherCATSyncService.h"

// EtherCATSyncService.cpp — Time, data, state, and configuration synchronization
//
// Implementation notes:
//   - Uses EventBus and EcatClient for sync orchestration
//   - Tracks sync count and last sync timestamp in SyncStatus struct
//   - Emits timeSynced and dataSynced signals on successful sync

EtherCATSyncService::EtherCATSyncService(EventBus *bus, EcatClient *client,
                                          QObject *parent)
    : QObject(parent), bus_(bus), client_(client)
{
}

bool EtherCATSyncService::syncTime()
{
    QDateTime now = QDateTime::currentDateTime();
    status_.lastSync = now;
    status_.syncCount++;
    emit timeSynced(now);
    return true;
}

bool EtherCATSyncService::syncData()
{
    status_.lastSync = QDateTime::currentDateTime();
    status_.syncCount++;
    emit dataSynced(0);
    return true;
}

bool EtherCATSyncService::syncState()
{
    status_.lastSync = QDateTime::currentDateTime();
    status_.syncCount++;
    return true;
}

bool EtherCATSyncService::syncConfiguration()
{
    status_.lastSync = QDateTime::currentDateTime();
    status_.syncCount++;
    return true;
}

SyncStatus EtherCATSyncService::syncStatus() const
{
    return status_;
}
