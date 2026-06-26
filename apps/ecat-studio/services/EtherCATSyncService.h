#pragma once

// EtherCATSyncService — synchronization request facade.
//
// Time, data, state, and configuration sync operations fail closed until the
// service is connected to a live EtherCAT backend.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QDateTime>

class EventBus;
class EcatClient;

struct SyncStatus {
  QDateTime lastSync;
  QDateTime nextSync;
  int syncCount = 0;
  int errorCount = 0;
  double averageDuration = 0.0;
  double successRate = 0.0;
};

class EtherCATSyncService : public QObject {
  Q_OBJECT
public:
  explicit EtherCATSyncService(EventBus *bus, EcatClient *client,
                                QObject *parent = nullptr);

  bool syncTime();
  bool syncData();
  bool syncState();
  bool syncConfiguration();
  SyncStatus syncStatus() const;

signals:
  void timeSynced(const QDateTime &timestamp);
  void dataSynced(int recordCount);

private:
  EventBus *bus_;
  EcatClient *client_;
  SyncStatus status_;

  bool rejectSync() const;
};
