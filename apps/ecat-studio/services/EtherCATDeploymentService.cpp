#include "EtherCATDeploymentService.h"
#include <QDateTime>

// EtherCATDeploymentService.cpp — Deployment records and offline deployment facade
//
// Implementation notes:
//   - Position-based device deployment fails closed without a live backend
//   - Generates unique deployment IDs via nextId_ counter
//   - Supports rollback by deployment ID with history tracking

EtherCATDeploymentService::EtherCATDeploymentService(EventBus *bus,
                                                     EcatClient *client,
                                                     QObject *parent)
    : QObject(parent), bus_(bus), client_(client)
{
}

DeploymentResult EtherCATDeploymentService::makeResult(
    const QString &id, const QString &target, const QString &config,
    const QString &status, const QString &log)
{
    DeploymentResult r;
    r.id = id;
    r.target = target;
    r.config = config;
    r.status = status;
    r.timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    r.log = log;
    return r;
}

DeploymentResult EtherCATDeploymentService::deployConfiguration(
    const QString &target, const QString &config)
{
    QString id = QStringLiteral("deploy_%1").arg(nextId_++);
    auto result = makeResult(id, target, config,
                             QStringLiteral("Success"),
                             QStringLiteral("Configuration '%1' deployed to '%2' successfully")
                                 .arg(config, target));
    deployments_.append(result);
    emit deploymentCompleted(result);
    return result;
}

DeploymentResult EtherCATDeploymentService::rollbackDeployment(
    const QString &deploymentId)
{
    for (auto &d : deployments_) {
        if (d.id == deploymentId) {
            d.status = QStringLiteral("RolledBack");
            d.log = QStringLiteral("Deployment '%1' rolled back").arg(deploymentId);
            d.timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
            emit deploymentCompleted(d);
            return d;
        }
    }
    return makeResult(deploymentId, QString(), QString(),
                      QStringLiteral("Failed"),
                      QStringLiteral("Deployment '%1' not found").arg(deploymentId));
}

QVector<DeploymentResult> EtherCATDeploymentService::listDeployments()
{
    return deployments_;
}

DeploymentResult EtherCATDeploymentService::getDeploymentStatus(
    const QString &deploymentId)
{
    for (const auto &d : deployments_) {
        if (d.id == deploymentId)
            return d;
    }
    return makeResult(deploymentId, QString(), QString(),
                      QStringLiteral("NotFound"),
                      QStringLiteral("Deployment '%1' not found").arg(deploymentId));
}

bool EtherCATDeploymentService::deployConfiguration(int position,
                                                     const ConfigData &data)
{
    if (data.configuration.isEmpty() || data.version.isEmpty())
        return false;
    Q_UNUSED(position);
    return false;
}

bool EtherCATDeploymentService::deployFirmware(int position,
                                               const FirmwareData &data)
{
    if (data.firmware.isEmpty() || data.version.isEmpty())
        return false;
    Q_UNUSED(position);
    return false;
}

bool EtherCATDeploymentService::deploySoftware(int position,
                                               const SoftwareData &data)
{
    if (data.software.isEmpty() || data.version.isEmpty())
        return false;
    Q_UNUSED(position);
    return false;
}

bool EtherCATDeploymentService::deploySystem(int position,
                                             const SystemData &data)
{
    if (data.system.isEmpty() || data.version.isEmpty())
        return false;
    Q_UNUSED(position);
    return false;
}
