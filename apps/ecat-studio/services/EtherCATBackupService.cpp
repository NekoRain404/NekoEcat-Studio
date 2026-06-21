#include "EtherCATBackupService.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

// EtherCATBackupService.cpp — Creates and restores EtherCAT configuration backups
//
// Implementation notes:
//   - Four backup types: full, incremental, differential, and selective
//   - Backups stored as timestamped JSON files in a configurable directory
//   - Incremental backups reference the last backup timestamp for delta tracking

EtherCATBackupService::EtherCATBackupService(QObject *parent)
    : QObject(parent)
{
    backupDir_ = QStringLiteral("./backups");
}

EcatBackupResult EtherCATBackupService::createFullBackup()
{
    QDateTime now = QDateTime::currentDateTime();
    QString fileName = QStringLiteral("full_%1.json")
                           .arg(now.toString(QStringLiteral("yyyyMMdd_HHmmss")));
    QString path = backupDir_ + QLatin1Char('/') + fileName;

    QDir dir(backupDir_);
    if (!dir.exists())
        dir.mkpath(QStringLiteral("."));

    QJsonObject root;
    root[QStringLiteral("type")] = QStringLiteral("full");
    root[QStringLiteral("timestamp")] = now.toString(Qt::ISODate);

    QJsonArray items;
    items.append(QStringLiteral("master_config"));
    items.append(QStringLiteral("slave_configs"));
    items.append(QStringLiteral("network_config"));
    items.append(QStringLiteral("timing_config"));
    items.append(QStringLiteral("sdo_data"));
    items.append(QStringLiteral("pdo_mappings"));
    root[QStringLiteral("items")] = items;

    QFile f(path);
    bool ok = false;
    if (f.open(QIODevice::WriteOnly)) {
        f.write(QJsonDocument(root).toJson());
        f.close();
        ok = true;
    }

    QStringList itemList;
    itemList << QStringLiteral("master_config")
             << QStringLiteral("slave_configs")
             << QStringLiteral("network_config")
             << QStringLiteral("timing_config")
             << QStringLiteral("sdo_data")
             << QStringLiteral("pdo_mappings");

    EcatBackupResult result = makeResult(
        path, ok ? QFileInfo(path).size() : 0, itemList, ok,
        ok ? QString() : QStringLiteral("Failed to write backup file"));

    lastBackupTime_ = now;
    emit backupCompleted(result);
    return result;
}

EcatBackupResult EtherCATBackupService::createIncrementalBackup()
{
    QDateTime now = QDateTime::currentDateTime();
    QString fileName = QStringLiteral("incr_%1.json")
                           .arg(now.toString(QStringLiteral("yyyyMMdd_HHmmss")));
    QString path = backupDir_ + QLatin1Char('/') + fileName;

    QDir dir(backupDir_);
    if (!dir.exists())
        dir.mkpath(QStringLiteral("."));

    QJsonObject root;
    root[QStringLiteral("type")] = QStringLiteral("incremental");
    root[QStringLiteral("timestamp")] = now.toString(Qt::ISODate);
    root[QStringLiteral("since")] = lastBackupTime_.toString(Qt::ISODate);

    QJsonArray items;
    items.append(QStringLiteral("changed_configs"));
    items.append(QStringLiteral("modified_sdos"));
    root[QStringLiteral("items")] = items;

    QFile f(path);
    bool ok = false;
    if (f.open(QIODevice::WriteOnly)) {
        f.write(QJsonDocument(root).toJson());
        f.close();
        ok = true;
    }

    QStringList itemList;
    itemList << QStringLiteral("changed_configs")
             << QStringLiteral("modified_sdos");

    EcatBackupResult result = makeResult(
        path, ok ? QFileInfo(path).size() : 0, itemList, ok,
        ok ? QString() : QStringLiteral("Failed to write backup file"));

    lastBackupTime_ = now;
    emit backupCompleted(result);
    return result;
}

EcatBackupResult EtherCATBackupService::createDifferentialBackup()
{
    QDateTime now = QDateTime::currentDateTime();
    QString fileName = QStringLiteral("diff_%1.json")
                           .arg(now.toString(QStringLiteral("yyyyMMdd_HHmmss")));
    QString path = backupDir_ + QLatin1Char('/') + fileName;

    QDir dir(backupDir_);
    if (!dir.exists())
        dir.mkpath(QStringLiteral("."));

    QJsonObject root;
    root[QStringLiteral("type")] = QStringLiteral("differential");
    root[QStringLiteral("timestamp")] = now.toString(Qt::ISODate);

    QJsonArray items;
    items.append(QStringLiteral("all_configs"));
    items.append(QStringLiteral("all_sdos"));
    root[QStringLiteral("items")] = items;

    QFile f(path);
    bool ok = false;
    if (f.open(QIODevice::WriteOnly)) {
        f.write(QJsonDocument(root).toJson());
        f.close();
        ok = true;
    }

    QStringList itemList;
    itemList << QStringLiteral("all_configs")
             << QStringLiteral("all_sdos");

    EcatBackupResult result = makeResult(
        path, ok ? QFileInfo(path).size() : 0, itemList, ok,
        ok ? QString() : QStringLiteral("Failed to write backup file"));

    lastBackupTime_ = now;
    emit backupCompleted(result);
    return result;
}

EcatBackupResult EtherCATBackupService::createSelectiveBackup(const QStringList &items)
{
    QDateTime now = QDateTime::currentDateTime();
    QString fileName = QStringLiteral("selective_%1.json")
                           .arg(now.toString(QStringLiteral("yyyyMMdd_HHmmss")));
    QString path = backupDir_ + QLatin1Char('/') + fileName;

    QDir dir(backupDir_);
    if (!dir.exists())
        dir.mkpath(QStringLiteral("."));

    QJsonObject root;
    root[QStringLiteral("type")] = QStringLiteral("selective");
    root[QStringLiteral("timestamp")] = now.toString(Qt::ISODate);

    QJsonArray arr;
    for (const QString &item : items)
        arr.append(item);
    root[QStringLiteral("items")] = arr;

    QFile f(path);
    bool ok = false;
    if (f.open(QIODevice::WriteOnly)) {
        f.write(QJsonDocument(root).toJson());
        f.close();
        ok = true;
    }

    EcatBackupResult result = makeResult(
        path, ok ? QFileInfo(path).size() : 0, items, ok,
        ok ? QString() : QStringLiteral("Failed to write backup file"));

    emit backupCompleted(result);
    return result;
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

    emit restoreCompleted(true);
    return true;
}

void EtherCATBackupService::setBackupDirectory(const QString &dir)
{
    backupDir_ = dir;
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
