#include "HotConnectService.h"

// HotConnectService.cpp — Hot Connect group lifecycle management
//
// Implementation notes:
//   - Groups track state (Active/Inactive) with timestamped event history
//   - Activation/deactivation emit state-change signals and record events
//   - History per group capped at kMaxHistory entries

HotConnectService::HotConnectService(QObject *parent) : QObject(parent) {}

int HotConnectService::createGroup(const QString &name,
                                    const QVector<int> &positions) {
  HotConnectGroup group;
  group.groupId = nextGroupId_++;
  group.name = name;
  group.slavePositions = positions;
  group.state = HotConnectGroupState::Inactive;
  group.lastStateChange = QDateTime::currentDateTime();
  groups_[group.groupId] = group;
  return group.groupId;
}

bool HotConnectService::removeGroup(int groupId) {
  if (!groups_.contains(groupId))
    return false;
  if (groups_[groupId].state == HotConnectGroupState::Active)
    deactivateGroup(groupId);
  groups_.remove(groupId);
  history_.remove(groupId);
  return true;
}

// Transitions group to Active, records event, emits groupActivated/groupStateChanged
bool HotConnectService::activateGroup(int groupId) {
  if (!groups_.contains(groupId))
    return false;
  auto &group = groups_[groupId];
  if (group.state == HotConnectGroupState::Active)
    return true;

  auto fromState = group.state;
  group.state = HotConnectGroupState::Active;
  group.lastStateChange = QDateTime::currentDateTime();

  HotConnectEvent event;
  event.groupId = groupId;
  event.fromState = fromState;
  event.toState = HotConnectGroupState::Active;
  event.timestamp = group.lastStateChange;
  event.success = true;
  history_[groupId].append(event);
  if (history_[groupId].size() > kMaxHistory)
    history_[groupId].removeFirst();

  emit groupActivated(groupId);
  emit groupStateChanged(groupId, HotConnectGroupState::Active);
  return true;
}

// Transitions group to Inactive, records event, emits groupDeactivated/groupStateChanged
bool HotConnectService::deactivateGroup(int groupId) {
  if (!groups_.contains(groupId))
    return false;
  auto &group = groups_[groupId];
  if (group.state == HotConnectGroupState::Inactive)
    return true;

  auto fromState = group.state;
  group.state = HotConnectGroupState::Inactive;
  group.lastStateChange = QDateTime::currentDateTime();

  HotConnectEvent event;
  event.groupId = groupId;
  event.fromState = fromState;
  event.toState = HotConnectGroupState::Inactive;
  event.timestamp = group.lastStateChange;
  event.success = true;
  history_[groupId].append(event);
  if (history_[groupId].size() > kMaxHistory)
    history_[groupId].removeFirst();

  emit groupDeactivated(groupId);
  emit groupStateChanged(groupId, HotConnectGroupState::Inactive);
  return true;
}

HotConnectGroup HotConnectService::groupInfo(int groupId) const {
  return groups_.value(groupId);
}

QVector<HotConnectGroup> HotConnectService::allGroups() const {
  QVector<HotConnectGroup> result;
  for (const auto &group : groups_)
    result.append(group);
  return result;
}

QVector<HotConnectEvent> HotConnectService::groupHistory(int groupId) const {
  return history_.value(groupId);
}

int HotConnectService::activeGroupCount() const {
  int count = 0;
  for (const auto &group : groups_) {
    if (group.state == HotConnectGroupState::Active)
      count++;
  }
  return count;
}

bool HotConnectService::isGroupActive(int groupId) const {
  if (!groups_.contains(groupId))
    return false;
  return groups_[groupId].state == HotConnectGroupState::Active;
}
