#include "HotConnectService.h"

// HotConnectService.cpp — Hot Connect group draft management
//
// Implementation notes:
//   - Groups are offline drafts until wired to a live EtherCAT backend
//   - Activation/deactivation fail closed instead of simulating slave presence
//   - History per group capped at kMaxHistory entries

HotConnectService::HotConnectService(QObject* parent) : QObject(parent) {}

int HotConnectService::createGroup(const QString& name, const QVector<int>& positions) {
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

// Activating Hot Connect groups changes real slave topology and requires a
// live EtherCAT backend. Do not synthesize Active state offline.
bool HotConnectService::activateGroup(int groupId) {
    if (!groups_.contains(groupId))
        return false;
    return false;
}

// Deactivation is also a real topology operation; without a backend there is no
// authoritative active state to transition from.
bool HotConnectService::deactivateGroup(int groupId) {
    if (!groups_.contains(groupId))
        return false;
    return false;
}

HotConnectGroup HotConnectService::groupInfo(int groupId) const {
    return groups_.value(groupId);
}

QVector<HotConnectGroup> HotConnectService::allGroups() const {
    QVector<HotConnectGroup> result;
    for (const auto& group : groups_)
        result.append(group);
    return result;
}

QVector<HotConnectEvent> HotConnectService::groupHistory(int groupId) const {
    return history_.value(groupId);
}

int HotConnectService::activeGroupCount() const {
    int count = 0;
    for (const auto& group : groups_) {
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
