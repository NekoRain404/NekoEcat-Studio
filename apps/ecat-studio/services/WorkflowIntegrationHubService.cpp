#include "WorkflowIntegrationHubService.h"

WorkflowIntegrationHubService::WorkflowIntegrationHubService(QObject *parent)
    : QObject(parent) {}

bool WorkflowIntegrationHubService::integrateSystem(const SystemConfig &config) {
  if (config.name.isEmpty() || !validateEndpoint(config.endpoint))
    return false;
  return false;
}

bool WorkflowIntegrationHubService::integrateData(const DataConfig &config) {
  if (config.source.isEmpty() || config.destination.isEmpty())
    return false;
  return false;
}

bool WorkflowIntegrationHubService::integrateProcess(const ProcessConfig &config) {
  if (config.workflow.isEmpty())
    return false;
  return false;
}

bool WorkflowIntegrationHubService::integrateService(const ServiceConfig &config) {
  if (config.service.isEmpty() || !validateEndpoint(config.endpoint))
    return false;
  return false;
}

bool WorkflowIntegrationHubService::validateEndpoint(const QString &endpoint) const {
  return !endpoint.isEmpty() &&
         (endpoint.startsWith("http://") || endpoint.startsWith("https://"));
}
