#pragma once

// EtherCATUpdateService — update request facade for EtherCAT slaves.
//
// Firmware/software update actions fail closed. Rejected requests return
// traceable result IDs but do not synthesize progress, completion signals, or
// update history.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QByteArray>
#include <QDateTime>
#include <QVector>
#include <QString>

class EcatClient;
class EventBus;

struct UpdateResult {
  QString id;
  int position = 0;
  QString version;
  QString status;
  int progress = 0;
  QString timestamp;
  QString log;
};

struct UpdateInfo {
  QString type;
  QString version;
  QString description;
  qint64 size = 0;
  QByteArray checksum;
  QDateTime releaseDate;
  QString downloadUrl;
};

class EtherCATUpdateService : public QObject {
  Q_OBJECT
public:
  explicit EtherCATUpdateService(EventBus *bus, EcatClient *client,
                                 QObject *parent = nullptr);

  UpdateResult checkForUpdates(int position);
  UpdateResult startUpdate(int position, const QString &version);
  bool cancelUpdate();
  QVector<UpdateResult> getUpdateHistory();

  QVector<UpdateInfo> checkForUpdates();
  bool downloadUpdate(const UpdateInfo &update);
  bool installUpdate(const UpdateInfo &update);
  bool rollbackUpdate(const UpdateInfo &update);

signals:
  void updateProgressChanged(int progress, const QString &status);
  void updateAvailable(const UpdateInfo &update);
  void updateDownloaded(const UpdateInfo &update);
  void updateInstalled(const UpdateInfo &update);

private:
  UpdateResult makeResult(int position, const QString &version,
                          const QString &status, int progress,
                          const QString &log);

  EventBus *bus_;
  EcatClient *client_;
  QVector<UpdateResult> history_;
  bool updating_ = false;
  int nextId_ = 1;
};
