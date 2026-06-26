#include "EtherCATIntegrationService.h"

// EtherCATIntegrationService.cpp — External system integration request facade
//
// Implementation notes:
//   - Validates connection parameters before establishing links
//   - Uses EventBus and EcatClient for orchestration
//   - Rejects offline integration attempts instead of synthesizing connections

EtherCATIntegrationService::EtherCATIntegrationService(EventBus *bus,
                                                        EcatClient *client,
                                                        QObject *parent)
    : QObject(parent), bus_(bus), client_(client)
{
}

bool EtherCATIntegrationService::connectToPLC(const PlcConfig &config)
{
    if (config.ipAddress.isEmpty() || config.port <= 0)
        return false;
    if (!backendReady())
        return false;

    emit connectedToSystem(QStringLiteral("PLC"));
    return true;
}

bool EtherCATIntegrationService::connectToSCADA(const ScadaConfig &config)
{
    if (config.serverUrl.isEmpty() || config.username.isEmpty())
        return false;
    if (!backendReady())
        return false;

    emit connectedToSystem(QStringLiteral("SCADA"));
    return true;
}

bool EtherCATIntegrationService::connectToMES(const MesConfig &config)
{
    if (config.endpoint.isEmpty() || config.apiKey.isEmpty())
        return false;
    if (!backendReady())
        return false;

    emit connectedToSystem(QStringLiteral("MES"));
    return true;
}

bool EtherCATIntegrationService::connectToERP(const ErpConfig &config)
{
    if (config.host.isEmpty() || config.database.isEmpty())
        return false;
    if (!backendReady())
        return false;

    emit connectedToSystem(QStringLiteral("ERP"));
    return true;
}

bool EtherCATIntegrationService::syncData(const QString &system)
{
    if (system.isEmpty())
        return false;
    if (!backendReady())
        return false;

    emit dataSynced(system, 0);
    return true;
}

bool EtherCATIntegrationService::backendReady() const
{
    // No real external integration backend is wired yet; keep success paths unreachable.
    return false;
}
