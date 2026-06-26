#pragma once

// WorkflowCloudService -- request facade for workflow cloud connectivity,
// synchronization, backup, and monitoring. No cloud provider backend is wired
// yet, so mutating requests fail closed instead of publishing synthetic
// connected/synced/backup success state.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>

struct WfCloudConfig {
  QString endpoint;
  QString apiKey;
  QString region;
  QString bucket;
  QString prefix;
  bool encryption = false;
  bool compression = false;
};

struct WfCloudStatus {
  bool connected = false;
  bool syncing = false;
  bool backing = false;
  qint64 lastSyncTime = 0;
  qint64 lastBackupTime = 0;
  int recordCount = 0;
  QString error;
};

class WorkflowCloudService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowCloudService(QObject *parent = nullptr);

  bool connectToCloud(const WfCloudConfig &config);
  bool syncToCloud();
  bool backupToCloud();
  WfCloudStatus monitorCloud() const;

  bool isConnected() const { return status_.connected; }
  WfCloudStatus status() const { return status_; }
  WfCloudConfig config() const { return config_; }

signals:
  void cloudConnected();
  void cloudSynced(int recordCount);
  void cloudBackupCompleted(bool success);

private:
  WfCloudConfig config_;
  WfCloudStatus status_;
};
