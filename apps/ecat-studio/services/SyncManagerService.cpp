#include "SyncManagerService.h"

// SyncManagerService.cpp — Configures EtherCAT sync managers (SM0–SM3) per slave
//
// Implementation notes:
//   - Nested QHash keyed by position then SM index (0–3)
//   - Supports PDO assignment, direction, and watchdog timeout per SM
//   - Validates SM index range and emits errors for out-of-bounds access

SyncManagerService::SyncManagerService(QObject *parent) : QObject(parent) {}

bool SyncManagerService::configureSyncManager(int position, int smIndex,
                                              const SyncManagerConfig &config) {
  if (position < 0 || smIndex < 0 || smIndex > 3) {
    emit error(QStringLiteral("Invalid position or SM index"));
    return false;
  }
  configs_[position][smIndex] = config;
  emit syncManagerConfigured(position, smIndex);
  return true;
}

bool SyncManagerService::assignPdo(int position, int smIndex, int pdoIndex) {
  auto posIt = configs_.find(position);
  if (posIt == configs_.end()) {
    emit error(QStringLiteral("No configuration for position %1").arg(position));
    return false;
  }
  auto smIt = posIt->find(smIndex);
  if (smIt == posIt->end()) {
    emit error(QStringLiteral("SM %1 not configured").arg(smIndex));
    return false;
  }
  smIt->pdoIndex = pdoIndex;
  emit syncManagerConfigured(position, smIndex);
  return true;
}

bool SyncManagerService::setDirection(int position, int smIndex,
                                      SmDirection direction) {
  auto posIt = configs_.find(position);
  if (posIt == configs_.end()) return false;
  auto smIt = posIt->find(smIndex);
  if (smIt == posIt->end()) return false;
  smIt->direction = direction;
  emit syncManagerConfigured(position, smIndex);
  return true;
}

bool SyncManagerService::setWatchdog(int position, int smIndex, int timeout) {
  auto posIt = configs_.find(position);
  if (posIt == configs_.end()) return false;
  auto smIt = posIt->find(smIndex);
  if (smIt == posIt->end()) return false;
  smIt->watchdogTimeout = timeout;
  emit syncManagerConfigured(position, smIndex);
  return true;
}

SyncManagerConfig SyncManagerService::syncManagerConfig(int position,
                                                        int smIndex) const {
  return configs_.value(position).value(smIndex);
}

QVector<int> SyncManagerService::syncManagers(int position) const {
  return configs_.value(position).keys().toVector();
}
