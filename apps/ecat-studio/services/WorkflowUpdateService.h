#pragma once

// WorkflowUpdateService -- request facade for workflow update checks,
// downloads, installs, and rollbacks. No updater backend is wired yet, so
// mutating requests fail closed instead of emitting synthetic success signals.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>
#include <QVector>
#include <QDateTime>

enum class WfUpdateType { Configuration, Firmware, Software, System };

struct WfUpdateInfo {
  WfUpdateType type;
  QString version;
  QString description;
  qint64 size = 0;
  QString checksum;
  QDateTime releaseDate;
  QString downloadUrl;
};

Q_DECLARE_METATYPE(WfUpdateInfo)

class WorkflowUpdateService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowUpdateService(QObject *parent = nullptr);

  QVector<WfUpdateInfo> checkForUpdates();
  bool downloadUpdate(const WfUpdateInfo &update);
  bool installUpdate(const WfUpdateInfo &update);
  bool rollbackUpdate(const WfUpdateInfo &update);

signals:
  void updateAvailable(const WfUpdateInfo &update);
  void updateDownloaded(const WfUpdateInfo &update);
  void updateInstalled(const WfUpdateInfo &update);

private:
  QVector<WfUpdateInfo> availableUpdates_;
};
