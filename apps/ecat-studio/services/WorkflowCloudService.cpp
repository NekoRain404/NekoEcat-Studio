#include "WorkflowCloudService.h"
#include <QDateTime>

WorkflowCloudService::WorkflowCloudService(QObject *parent)
    : QObject(parent)
{
}

bool WorkflowCloudService::connectToCloud(const WfCloudConfig &config)
{
    if (config.endpoint.isEmpty() || config.apiKey.isEmpty())
        return false;

    config_ = config;
    status_.connected = true;
    status_.error.clear();
    emit cloudConnected();
    return true;
}

bool WorkflowCloudService::syncToCloud()
{
    if (!status_.connected)
        return false;

    status_.syncing = true;
    status_.recordCount += 10;
    status_.lastSyncTime = QDateTime::currentMSecsSinceEpoch();
    status_.syncing = false;
    emit cloudSynced(status_.recordCount);
    return true;
}

bool WorkflowCloudService::backupToCloud()
{
    if (!status_.connected)
        return false;

    status_.backing = true;
    status_.lastBackupTime = QDateTime::currentMSecsSinceEpoch();
    status_.backing = false;
    emit cloudBackupCompleted(true);
    return true;
}

WfCloudStatus WorkflowCloudService::monitorCloud() const
{
    return status_;
}
