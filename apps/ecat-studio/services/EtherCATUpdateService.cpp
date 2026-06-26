#include "EtherCATUpdateService.h"
#include <QDateTime>

// EtherCATUpdateService.cpp — Firmware/software update request facade
//
// Implementation notes:
//   - Uses EventBus and EcatClient for update orchestration
//   - Tracks update history with position, version, and progress
//   - Rejects offline update actions instead of synthesizing success

EtherCATUpdateService::EtherCATUpdateService(EventBus *bus, EcatClient *client,
                                             QObject *parent)
    : QObject(parent), bus_(bus), client_(client)
{
}

UpdateResult EtherCATUpdateService::makeResult(int position,
                                               const QString &version,
                                               const QString &status,
                                               int progress,
                                               const QString &log)
{
    UpdateResult r;
    r.id = QStringLiteral("update_%1").arg(nextId_++);
    r.position = position;
    r.version = version;
    r.status = status;
    r.progress = progress;
    r.timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    r.log = log;
    return r;
}

UpdateResult EtherCATUpdateService::checkForUpdates(int position)
{
    return makeResult(position, QString(),
                      QStringLiteral("Rejected"), 0,
                      QStringLiteral("Update check requires a connected EtherCAT update backend"));
}

UpdateResult EtherCATUpdateService::startUpdate(int position,
                                                const QString &version)
{
    return makeResult(position, version,
                      QStringLiteral("Rejected"), 0,
                      QStringLiteral("Firmware update requires a connected EtherCAT update backend"));
}

bool EtherCATUpdateService::cancelUpdate()
{
    if (updating_) {
        updating_ = false;
        emit updateProgressChanged(0, QStringLiteral("Cancelled"));
        return true;
    }
    return false;
}

QVector<UpdateResult> EtherCATUpdateService::getUpdateHistory()
{
    return history_;
}

QVector<UpdateInfo> EtherCATUpdateService::checkForUpdates()
{
    return {};
}

bool EtherCATUpdateService::downloadUpdate(const UpdateInfo &update)
{
    Q_UNUSED(update);
    return false;
}

bool EtherCATUpdateService::installUpdate(const UpdateInfo &update)
{
    Q_UNUSED(update);
    return false;
}

bool EtherCATUpdateService::rollbackUpdate(const UpdateInfo &update)
{
    Q_UNUSED(update);
    return false;
}
