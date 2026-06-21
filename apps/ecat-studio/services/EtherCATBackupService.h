#pragma once

// EtherCATBackupService — backup management for EtherCAT system state.
//
// Supports full, incremental, differential, and selective backups.
// Each backup operation returns a BackupResult with path, size, and status.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>

struct EcatBackupResult {
  QString backupPath;
  qint64 backupSize = 0;
  QDateTime timestamp;
  QStringList items;
  bool success = false;
  QString error;
};

class EtherCATBackupService : public QObject {
  Q_OBJECT
public:
  explicit EtherCATBackupService(QObject *parent = nullptr);

  EcatBackupResult createFullBackup();
  EcatBackupResult createIncrementalBackup();
  EcatBackupResult createDifferentialBackup();
  EcatBackupResult createSelectiveBackup(const QStringList &items);
  bool restoreBackup(const QString &backupPath);

  void setBackupDirectory(const QString &dir);
  QString backupDirectory() const { return backupDir_; }

signals:
  void backupCompleted(const EcatBackupResult &result);
  void restoreCompleted(bool success);

private:
  EcatBackupResult makeResult(const QString &path, qint64 size,
                              const QStringList &items, bool ok,
                              const QString &err);

  QString backupDir_;
  QDateTime lastBackupTime_;
};
