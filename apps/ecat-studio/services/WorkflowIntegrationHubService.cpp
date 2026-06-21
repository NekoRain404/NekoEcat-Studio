#include "WorkflowIntegrationHubService.h"

WorkflowIntegrationHubService::WorkflowIntegrationHubService(QObject *parent)
    : QObject(parent) {}

bool WorkflowIntegrationHubService::integrateSystem(const SystemConfig &config) {
  if (config.name.isEmpty() || !validateEndpoint(config.endpoint))
    return false;
  emit integrationConnected(config.name);
  return true;
}

bool WorkflowIntegrationHubService::integrateData(const DataConfig &config) {
  if (config.source.isEmpty() || config.destination.isEmpty())
    return false;
  emit dataSynced(config.source, 0);
  return true;
}

bool WorkflowIntegrationHubService::integrateProcess(const ProcessConfig &config) {
  if (config.workflow.isEmpty())
    return false;
  emit integrationConnected(config.workflow);
  return true;
}

bool WorkflowIntegrationHubService::integrateService(const ServiceConfig &config) {
  if (config.service.isEmpty() || !validateEndpoint(config.endpoint))
    return false;
  emit integrationConnected(config.service);
  return true;
}

bool WorkflowIntegrationHubService::validateEndpoint(const QString &endpoint) const {
  return !endpoint.isEmpty() &&
         (endpoint.startsWith("http://") || endpoint.startsWith("https://"));
}
