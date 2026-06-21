#include "WorkflowUpdateService.h"

WorkflowUpdateService::WorkflowUpdateService(QObject *parent)
    : QObject(parent)
{
}

QVector<WfUpdateInfo> WorkflowUpdateService::checkForUpdates()
{
    for (const auto &update : availableUpdates_)
        emit updateAvailable(update);
    return availableUpdates_;
}

bool WorkflowUpdateService::downloadUpdate(const WfUpdateInfo &update)
{
    if (update.downloadUrl.isEmpty() || update.version.isEmpty())
        return false;
    emit updateDownloaded(update);
    return true;
}

bool WorkflowUpdateService::installUpdate(const WfUpdateInfo &update)
{
    if (update.version.isEmpty())
        return false;
    emit updateInstalled(update);
    return true;
}

bool WorkflowUpdateService::rollbackUpdate(const WfUpdateInfo &update)
{
    if (update.version.isEmpty())
        return false;
    return true;
}
