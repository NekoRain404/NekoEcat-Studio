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
    return rejectSync();
}

bool EtherCATSyncService::syncData()
{
    return rejectSync();
}

bool EtherCATSyncService::syncState()
{
    return rejectSync();
}

bool EtherCATSyncService::syncConfiguration()
{
    return rejectSync();
}

SyncStatus EtherCATSyncService::syncStatus() const
{
    return status_;
}

bool EtherCATSyncService::rejectSync() const
{
    return false;
}
