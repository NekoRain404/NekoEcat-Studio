#include "WorkflowIntegrationService.h"

// WorkflowIntegrationService.cpp — Connects to CI, issue trackers, and communication tools
//
// Implementation notes:
//   - Each integration validated for server URL and token before storing
//   - Emits integrationConnected/integrationError signals per service type
//   - Independent connection state tracked per integration (CI, issues, comms, docs)

WorkflowIntegrationService::WorkflowIntegrationService(QObject *parent)
    : QObject(parent) {}

bool WorkflowIntegrationService::integrateWithCI(const CIConfig &config) {
  if (!validateServer(config.server) || !validateToken(config.token)) {
    emit integrationError("ci", "Invalid server or token");
    return false;
  }
  ciConfig_ = config;
  ciConnected_ = true;
  emit integrationConnected("ci");
  return true;
}

bool WorkflowIntegrationService::integrateWithIssueTracker(
    const IssueTrackerConfig &config) {
  if (!validateServer(config.server) || !validateToken(config.token)) {
    emit integrationError("issue_tracker", "Invalid server or token");
    return false;
  }
  issueTrackerConfig_ = config;
  issueTrackerConnected_ = true;
  emit integrationConnected("issue_tracker");
  return true;
}

bool WorkflowIntegrationService::integrateWithCommunication(
    const CommunicationConfig &config) {
  if (!validateServer(config.server) || !validateToken(config.token)) {
    emit integrationError("communication", "Invalid server or token");
    return false;
  }
  commConfig_ = config;
  commConnected_ = true;
  emit integrationConnected("communication");
  return true;
}

bool WorkflowIntegrationService::integrateWithDocumentation(
    const DocumentationConfig &config) {
  if (!validateServer(config.server) || !validateToken(config.token)) {
    emit integrationError("documentation", "Invalid server or token");
    return false;
  }
  docConfig_ = config;
  docConnected_ = true;
  emit integrationConnected("documentation");
  return true;
}

bool WorkflowIntegrationService::validateServer(const QString &server) const {
  return !server.isEmpty() &&
         (server.startsWith("http://") || server.startsWith("https://"));
}

bool WorkflowIntegrationService::validateToken(const QString &token) const {
  return !token.isEmpty();
}
