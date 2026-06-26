#pragma once

// WorkflowSyncService -- request facade for workflow time, data, state,
// and configuration synchronization. No synchronization backend is wired yet,
// so requests fail closed instead of publishing synthetic success state.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QDateTime>

struct WorkflowSyncStatus {
  QDateTime lastSync;
  QDateTime nextSync;
  int syncCount = 0;
  int errorCount = 0;
  double averageDuration = 0.0;
  double successRate = 0.0;
};

class WorkflowSyncService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowSyncService(QObject *parent = nullptr);

  bool syncTime();
  bool syncData();
  bool syncState();
  bool syncConfiguration();
  WorkflowSyncStatus syncStatus() const;

signals:
  void timeSynced(const QDateTime &timestamp);
  void dataSynced(int recordCount);

private:
  WorkflowSyncStatus status_;
};
