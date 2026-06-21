#include "RedundancyService.h"

// RedundancyService.cpp — Manages dual-path EtherCAT redundancy (primary + standby)
//
// Implementation notes:
//   - Maintains primary (Active) and secondary (Standby) path states
//   - Automatic failover promotes standby to active on primary failure
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
  if (state_ == RedundancyState::DualPath)
    return true;

  auto fromState = state_;
  state_ = RedundancyState::DualPath;
  primaryPath_.state = PathState::Active;
  secondaryPath_.state = PathState::Standby;

  RedundancyEvent event;
  event.fromState = fromState;
  event.toState = RedundancyState::DualPath;
  event.timestamp = QDateTime::currentDateTime();
  event.success = true;
  history_.append(event);
  if (history_.size() > kMaxHistory)
    history_.removeFirst();

  emit redundancyStateChanged(RedundancyState::DualPath);
  return true;
}

bool RedundancyService::disableRedundancy() {
  if (state_ == RedundancyState::SinglePath)
    return true;

  auto fromState = state_;
  state_ = RedundancyState::SinglePath;
  secondaryPath_.state = PathState::Standby;

  RedundancyEvent event;
  event.fromState = fromState;
  event.toState = RedundancyState::SinglePath;
  event.timestamp = QDateTime::currentDateTime();
  event.success = true;
  history_.append(event);
  if (history_.size() > kMaxHistory)
    history_.removeFirst();

  emit redundancyStateChanged(RedundancyState::SinglePath);
  return true;
}

bool RedundancyService::failover() {
  if (state_ != RedundancyState::DualPath)
    return false;

  auto fromState = state_;
  state_ = RedundancyState::Failover;
  primaryPath_.state = PathState::Failed;
  secondaryPath_.state = PathState::Active;

  RedundancyEvent event;
  event.fromState = fromState;
  event.toState = RedundancyState::Failover;
  event.timestamp = QDateTime::currentDateTime();
  event.success = true;
  history_.append(event);
  if (history_.size() > kMaxHistory)
    history_.removeFirst();

  emit redundancyStateChanged(RedundancyState::Failover);
  emit failoverOccurred(0, 1);
  emit pathStateChanged(0, PathState::Failed);
  emit pathStateChanged(1, PathState::Active);
  return true;
}

bool RedundancyService::failback() {
  if (state_ != RedundancyState::Failover)
    return false;

  auto fromState = state_;
  state_ = RedundancyState::DualPath;
  primaryPath_.state = PathState::Active;
  secondaryPath_.state = PathState::Standby;

  RedundancyEvent event;
  event.fromState = fromState;
  event.toState = RedundancyState::DualPath;
  event.timestamp = QDateTime::currentDateTime();
  event.success = true;
  history_.append(event);
  if (history_.size() > kMaxHistory)
    history_.removeFirst();

  emit redundancyStateChanged(RedundancyState::DualPath);
  return true;
}

RedundancyState RedundancyService::currentState() const { return state_; }
RedundancyPath RedundancyService::primaryPath() const { return primaryPath_; }
RedundancyPath RedundancyService::secondaryPath() const { return secondaryPath_; }
QVector<RedundancyEvent> RedundancyService::redundancyHistory() const { return history_; }
bool RedundancyService::isRedundant() const { return state_ == RedundancyState::DualPath; }
