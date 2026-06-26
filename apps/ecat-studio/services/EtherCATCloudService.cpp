#include "EtherCATCloudService.h"

EtherCATCloudService::EtherCATCloudService(QObject *parent)
    : QObject(parent)
{
}

bool EtherCATCloudService::connectToCloud(const CloudConfig &config)
{
    if (config.endpoint.isEmpty() || config.apiKey.isEmpty())
        return false;

    config_ = config;
    status_.connected = false;
    status_.syncing = false;
    status_.backing = false;
    status_.error.clear();
    return false;
}

bool EtherCATCloudService::syncToCloud()
{
    status_.syncing = false;
    return false;
}

bool EtherCATCloudService::backupToCloud()
{
    status_.backing = false;
    return false;
}

CloudStatus EtherCATCloudService::monitorCloud() const
{
    return status_;
}
