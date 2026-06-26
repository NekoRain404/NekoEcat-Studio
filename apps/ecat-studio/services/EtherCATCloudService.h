#pragma once

// EtherCATCloudService -- request facade for cloud connectivity,
// synchronization, backup, and monitoring for EtherCAT systems. No cloud
// client/backend is wired yet, so mutating requests fail closed instead of
// publishing synthetic connected, synced, or backed-up state.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QVector>
#include <QString>

struct CloudConfig {
  QString endpoint;
  QString apiKey;
  QString region;
  QString bucket;
  QString prefix;
  bool encryption = false;
  bool compression = false;
};

struct CloudStatus {
  bool connected = false;
  bool syncing = false;
  bool backing = false;
  qint64 lastSyncTime = 0;
  qint64 lastBackupTime = 0;
  int recordCount = 0;
  QString error;
};

class EtherCATCloudService : public QObject {
  Q_OBJECT
public:
  explicit EtherCATCloudService(QObject *parent = nullptr);

  bool connectToCloud(const CloudConfig &config);
  bool syncToCloud();
  bool backupToCloud();
  CloudStatus monitorCloud() const;

  bool isConnected() const { return status_.connected; }
  CloudStatus status() const { return status_; }
  CloudConfig config() const { return config_; }

signals:
  void cloudConnected();
  void cloudSynced(int recordCount);
  void cloudBackupCompleted(bool success);

private:
  CloudConfig config_;
  CloudStatus status_;
};
