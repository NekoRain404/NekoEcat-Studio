#pragma once

// EtherCATBackupService — backup request facade for EtherCAT system state.
//
// Full, incremental, differential, selective backup, and restore requests fail
// closed until a live backup/restore backend can collect and apply real
// EtherCAT state.
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
  EcatBackupResult rejectBackup(const QString &err);
  EcatBackupResult makeResult(const QString &path, qint64 size,
                              const QStringList &items, bool ok,
                              const QString &err);

  QString backupDir_;
};
