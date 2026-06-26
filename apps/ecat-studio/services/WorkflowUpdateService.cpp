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
    Q_UNUSED(update);
    return false;
}

bool WorkflowUpdateService::installUpdate(const WfUpdateInfo &update)
{
    Q_UNUSED(update);
    return false;
}

bool WorkflowUpdateService::rollbackUpdate(const WfUpdateInfo &update)
{
    Q_UNUSED(update);
    return false;
}
