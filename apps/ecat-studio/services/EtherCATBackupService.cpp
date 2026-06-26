#include "EtherCATBackupService.h"

#include <QFile>
#include <QJsonDocument>

// EtherCATBackupService.cpp — backup/restore request facade.
//
// Implementation notes:
//   - Requests fail closed until a live backend can collect real EtherCAT state
//   - Local JSON validation alone is not treated as a successful restore

EtherCATBackupService::EtherCATBackupService(QObject *parent)
    : QObject(parent)
{
    backupDir_ = QStringLiteral("./backups");
}

EcatBackupResult EtherCATBackupService::createFullBackup()
{
    return rejectBackup(
        QStringLiteral("No live EtherCAT backup source is available"));
}

EcatBackupResult EtherCATBackupService::createIncrementalBackup()
{
    return rejectBackup(
        QStringLiteral("No live EtherCAT incremental backup source is available"));
}

EcatBackupResult EtherCATBackupService::createDifferentialBackup()
{
    return rejectBackup(
        QStringLiteral("No live EtherCAT differential backup source is available"));
}

EcatBackupResult EtherCATBackupService::createSelectiveBackup(const QStringList &items)
{
    Q_UNUSED(items)
    return rejectBackup(
        QStringLiteral("No live EtherCAT selective backup source is available"));
}

bool EtherCATBackupService::restoreBackup(const QString &backupPath)
{
    QFile f(backupPath);
    if (!f.open(QIODevice::ReadOnly)) {
        emit restoreCompleted(false);
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isObject()) {
        emit restoreCompleted(false);
        return false;
    }

    emit restoreCompleted(false);
    return false;
}

void EtherCATBackupService::setBackupDirectory(const QString &dir)
{
    backupDir_ = dir;
}

EcatBackupResult EtherCATBackupService::rejectBackup(const QString &err)
{
    return makeResult(QString(), 0, QStringList(), false, err);
}

EcatBackupResult EtherCATBackupService::makeResult(const QString &path, qint64 size,
                                                   const QStringList &items, bool ok,
                                                   const QString &err)
{
    EcatBackupResult r;
    r.backupPath = path;
    r.backupSize = size;
    r.timestamp = QDateTime::currentDateTime();
    r.items = items;
    r.success = ok;
    r.error = err;
    return r;
}
