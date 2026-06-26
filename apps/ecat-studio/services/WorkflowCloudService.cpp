#include "WorkflowCloudService.h"

WorkflowCloudService::WorkflowCloudService(QObject *parent)
    : QObject(parent)
{
}

bool WorkflowCloudService::connectToCloud(const WfCloudConfig &config)
{
    Q_UNUSED(config);
    return false;
}

bool WorkflowCloudService::syncToCloud()
{
    return false;
}

bool WorkflowCloudService::backupToCloud()
{
    return false;
}

WfCloudStatus WorkflowCloudService::monitorCloud() const
{
    return status_;
}
