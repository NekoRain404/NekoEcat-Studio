#include "WorkflowDeploymentService.h"
#include <QCryptographicHash>

WorkflowDeploymentService::WorkflowDeploymentService(QObject *parent)
    : QObject(parent)
{
}

bool WorkflowDeploymentService::deployConfiguration(int position, const WfConfigData &data)
{
    if (data.configuration.isEmpty() || data.version.isEmpty())
        return false;
    if (!validateChecksum(data.configuration, data.checksum))
        return false;
    return executeDeployment(position, QStringLiteral("configuration"),
                             data.configuration, data.version);
}

bool WorkflowDeploymentService::deployFirmware(int position, const WfFirmwareData &data)
{
    if (data.firmware.isEmpty() || data.version.isEmpty())
        return false;
    if (!validateChecksum(data.firmware, data.checksum))
        return false;
    return executeDeployment(position, QStringLiteral("firmware"),
                             data.firmware, data.version);
}

bool WorkflowDeploymentService::deploySoftware(int position, const WfSoftwareData &data)
{
    if (data.software.isEmpty() || data.version.isEmpty())
        return false;
    if (!validateChecksum(data.software, data.checksum))
        return false;
    return executeDeployment(position, QStringLiteral("software"),
                             data.software, data.version);
}

bool WorkflowDeploymentService::deploySystem(int position, const WfSystemData &data)
{
    if (data.system.isEmpty() || data.version.isEmpty())
        return false;
    if (!validateChecksum(data.system, data.checksum))
        return false;
    return executeDeployment(position, QStringLiteral("system"),
                             data.system, data.version);
}

bool WorkflowDeploymentService::validateChecksum(const QByteArray &payload,
                                                  const QString &checksum)
{
    if (checksum.isEmpty())
        return true;
    QByteArray hash = QCryptographicHash::hash(payload, QCryptographicHash::Sha256);
    return hash.toHex() == checksum.toUtf8();
}

bool WorkflowDeploymentService::executeDeployment(int position, const QString &type,
                                                   const QByteArray &payload,
                                                   const QString &version)
{
    emit deploymentStarted(position, type);
    emit deploymentProgress(position, 50);
    emit deploymentProgress(position, 100);
    emit deploymentCompleted(position, true);
    return true;
}
