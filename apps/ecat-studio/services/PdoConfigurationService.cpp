#include "PdoConfigurationService.h"

PdoConfigurationService::PdoConfigurationService(QObject *parent)
    : QObject(parent) {}

bool PdoConfigurationService::configurePdoMapping(int position,
                                                   const PdoMappingConfig &config) {
  QString error;
  if (!validateMapping(config, error)) {
    configurationError(position, error);
    return false;
  }

  auto &slave = ensureSlave(position);
  auto &mappings = slave.mappings;
  for (auto &m : mappings) {
    if (m.index == config.index && m.subIndex == config.subIndex) {
      m = config;
      slave.status.mappingConfigured = true;
      return true;
    }
  }
  mappings.append(config);
  slave.status.mappingConfigured = true;
  return true;
}

bool PdoConfigurationService::configurePdoAssignment(int position,
                                                      const PdoAssignmentConfig &config) {
  QString error;
  if (!validateAssignment(config, error)) {
    configurationError(position, error);
    return false;
  }

  auto &slave = ensureSlave(position);
  auto &assignments = slave.assignments;
  for (auto &a : assignments) {
    if (a.smIndex == config.smIndex) {
      a = config;
      slave.status.assignmentConfigured = true;
      return true;
    }
  }
  assignments.append(config);
  slave.status.assignmentConfigured = true;
  return true;
}

bool PdoConfigurationService::configureSyncManager(int position,
                                                     const PdoSyncManagerConfig &config) {
  if (config.smIndex < 0 || config.smIndex > 7) {
    configurationError(position, QStringLiteral("Invalid sync manager index: %1").arg(config.smIndex));
    return false;
  }

  auto &slave = ensureSlave(position);
  auto &sms = slave.smConfigs;
  for (auto &sm : sms) {
    if (sm.smIndex == config.smIndex) {
      sm = config;
      slave.status.syncManagerConfigured = true;
      return true;
    }
  }
  sms.append(config);
  slave.status.syncManagerConfigured = true;
  return true;
}

bool PdoConfigurationService::configureDcSync(int position,
                                               const DcSyncConfig &config) {
  auto &slave = ensureSlave(position);
  slave.dcSync = config;
  slave.status.dcSyncConfigured = true;
  return true;
}

bool PdoConfigurationService::applyConfiguration(int position) {
  auto it = configs_.find(position);
  if (it == configs_.end()) {
    configurationError(position, QStringLiteral("No configuration found for position %1").arg(position));
    return false;
  }

  auto &slave = *it;
  if (!slave.status.mappingConfigured) {
    configurationError(position, QStringLiteral("PDO mappings not configured for position %1").arg(position));
    return false;
  }

  slave.status.position = position;
  slave.status.lastApplied = QDateTime::currentDateTime();
  slave.status.lastError.clear();

  configurationApplied(position);
  return true;
}

QVector<PdoMappingConfig> PdoConfigurationService::pdoMappings(int position) const {
  auto it = configs_.constFind(position);
  if (it == configs_.constEnd()) return {};
  return it->mappings;
}

QVector<PdoAssignmentConfig> PdoConfigurationService::pdoAssignments(int position) const {
  auto it = configs_.constFind(position);
  if (it == configs_.constEnd()) return {};
  return it->assignments;
}

QVector<PdoSyncManagerConfig> PdoConfigurationService::syncManagers(int position) const {
  auto it = configs_.constFind(position);
  if (it == configs_.constEnd()) return {};
  return it->smConfigs;
}

DcSyncConfig PdoConfigurationService::dcSyncConfig(int position) const {
  auto it = configs_.constFind(position);
  if (it == configs_.constEnd()) return {};
  return it->dcSync;
}

PdoConfigurationStatus PdoConfigurationService::configurationStatus(int position) const {
  auto it = configs_.constFind(position);
  if (it == configs_.constEnd()) {
    PdoConfigurationStatus s;
    s.position = position;
    return s;
  }
  return it->status;
}

PdoConfigurationService::SlaveConfig &PdoConfigurationService::ensureSlave(int position) {
  if (!configs_.contains(position)) {
    SlaveConfig sc;
    sc.status.position = position;
    configs_[position] = sc;
  }
  return configs_[position];
}

bool PdoConfigurationService::validateMapping(const PdoMappingConfig &config,
                                               QString &error) const {
  if (config.index.isEmpty()) {
    error = QStringLiteral("PDO index cannot be empty");
    return false;
  }
  if (config.bitSize <= 0) {
    error = QStringLiteral("Bit size must be positive");
    return false;
  }
  if (config.bitSize > 1500 * 8) {
    error = QStringLiteral("Bit size exceeds maximum (%1)").arg(1500 * 8);
    return false;
  }
  return true;
}

bool PdoConfigurationService::validateAssignment(const PdoAssignmentConfig &config,
                                                   QString &error) const {
  if (config.smIndex < 0 || config.smIndex > 7) {
    error = QStringLiteral("Invalid sync manager index: %1").arg(config.smIndex);
    return false;
  }
  if (config.pdoIndices.isEmpty()) {
    error = QStringLiteral("PDO assignment must contain at least one PDO index");
    return false;
  }
  return true;
}
