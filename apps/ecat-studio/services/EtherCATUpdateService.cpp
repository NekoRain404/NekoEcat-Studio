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
    if (!backendReady()) {
        return makeResult(position, QString(),
                          QStringLiteral("Rejected"), 0,
                          QStringLiteral("Update check requires a connected EtherCAT update backend"));
    }

    auto result = makeResult(position, QStringLiteral("2.1.0"),
                             QStringLiteral("Available"), 0,
                             QStringLiteral("Update available for slave at position %1")
                                 .arg(position));
    history_.append(result);
    emit updateProgressChanged(0, QStringLiteral("Available"));
    return result;
}

UpdateResult EtherCATUpdateService::startUpdate(int position,
                                                const QString &version)
{
    if (!backendReady()) {
        return makeResult(position, version,
                          QStringLiteral("Rejected"), 0,
                          QStringLiteral("Firmware update requires a connected EtherCAT update backend"));
    }

    updating_ = true;
    auto result = makeResult(position, version,
                             QStringLiteral("Completed"), 100,
                             QStringLiteral("Firmware update to '%1' for slave at position %2 completed")
                                 .arg(version).arg(position));
    history_.append(result);
    updating_ = false;
    emit updateProgressChanged(100, QStringLiteral("Completed"));
    return result;
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
    if (update.downloadUrl.isEmpty())
        return false;
    if (!backendReady())
        return false;

    emit updateDownloaded(update);
    return true;
}

bool EtherCATUpdateService::installUpdate(const UpdateInfo &update)
{
    if (update.version.isEmpty())
        return false;
    if (!backendReady())
        return false;

    emit updateInstalled(update);
    return true;
}

bool EtherCATUpdateService::rollbackUpdate(const UpdateInfo &update)
{
    Q_UNUSED(update);
    return backendReady();
}

bool EtherCATUpdateService::backendReady() const
{
    // No real update backend is wired yet; keep success paths unreachable.
    return false;
}
