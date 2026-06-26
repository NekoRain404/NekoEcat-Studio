#include "RedundancyService.h"

// RedundancyService.cpp — Manages offline dual-path redundancy drafts
//
// Implementation notes:
//   - Maintains primary/secondary path draft metadata
//   - Runtime enable/failover/failback fail closed without a live backend
//   - Transition history capped at kMaxHistory entries

RedundancyService::RedundancyService(QObject *parent) : QObject(parent) {
  primaryPath_.pathId = 0;
  primaryPath_.state = PathState::Unknown;
  secondaryPath_.pathId = 1;
  secondaryPath_.state = PathState::Unknown;
}

void RedundancyService::setPrimaryPath(int slaveCount) {
  primaryPath_.slaveCount = slaveCount;
  primaryPath_.state = PathState::Active;
  primaryPath_.isHealthy = true;
  primaryPath_.lastCheck = QDateTime::currentDateTime();
}

void RedundancyService::setSecondaryPath(int slaveCount) {
  secondaryPath_.slaveCount = slaveCount;
  secondaryPath_.state = PathState::Standby;
  secondaryPath_.isHealthy = true;
  secondaryPath_.lastCheck = QDateTime::currentDateTime();
}

bool RedundancyService::enableRedundancy() {
  if (primaryPath_.slaveCount == 0 || secondaryPath_.slaveCount == 0)
    return false;
  return false;
}

bool RedundancyService::disableRedundancy() {
  return false;
}

bool RedundancyService::failover() {
  return false;
}

bool RedundancyService::failback() {
  return false;
}

RedundancyState RedundancyService::currentState() const { return state_; }
RedundancyPath RedundancyService::primaryPath() const { return primaryPath_; }
RedundancyPath RedundancyService::secondaryPath() const { return secondaryPath_; }
QVector<RedundancyEvent> RedundancyService::redundancyHistory() const { return history_; }
bool RedundancyService::isRedundant() const { return state_ == RedundancyState::DualPath; }
