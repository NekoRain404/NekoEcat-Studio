#pragma once

// HotConnectService — manages offline Hot Connect group drafts.
//
// Supports creating/removing group drafts and readback. Activation and
// deactivation fail closed until wired to a live EtherCAT backend capable of
// controlling real Hot Connect topology.
//
// This service currently handles:
//   - Hot Connect group draft creation and removal
//   - Explicit rejection of unsupported offline activation/deactivation
//   - Group draft state tracking
//   - Per-group slave position management
//
// Usage:
//   HotConnectService hotConnect;
//   QVector<int> positions = {0, 1, 2};
//   int groupId = hotConnect.createGroup("Motor Group", positions);
//   // Returns false until connected to a real backend:
//   hotConnect.activateGroup(groupId);
//   HotConnectGroup group = hotConnect.groupInfo(groupId);
//   QVector<HotConnectGroup> allGroups = hotConnect.allGroups();
//   QVector<HotConnectEvent> history = hotConnect.groupHistory(groupId);
//   int activeCount = hotConnect.activeGroupCount();
//   bool isActive = hotConnect.isGroupActive(groupId);
//   // Returns false until connected to a real backend:
//   hotConnect.deactivateGroup(groupId);
//   hotConnect.removeGroup(groupId);
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. Offline draft
//   operations are synchronous and block the calling thread.
//
// Performance:
//   - Group creation is O(n) where n is number of positions
//   - Offline activation/deactivation rejection is O(1)
//   - Group lookup is O(1) by ID
//   - History retrieval is O(n) where n is history size

#include <QDateTime>
#include <QHash>
#include <QObject>
#include <QVector>

// Hot Connect group state enumeration.
enum class HotConnectGroupState {
    Active,   // Group is active (slaves connected)
    Inactive, // Group is inactive (slaves disconnected)
    Error     // Group is in error state
};

// Hot Connect group structure.
struct HotConnectGroup {
    int groupId = 0;                                             // Group ID
    QString name;                                                // Group name
    QVector<int> slavePositions;                                 // Slave positions in group
    HotConnectGroupState state = HotConnectGroupState::Inactive; // Group state
    QDateTime lastStateChange;                                   // Last state change timestamp
};

// Hot Connect event structure.
struct HotConnectEvent {
    int groupId = 0;                                                 // Group ID
    HotConnectGroupState fromState = HotConnectGroupState::Inactive; // Previous state
    HotConnectGroupState toState = HotConnectGroupState::Inactive;   // New state
    QDateTime timestamp;                                             // Event timestamp
    bool success = false;                                            // Whether event succeeded
};

class HotConnectService : public QObject {
    Q_OBJECT
public:
    explicit HotConnectService(QObject* parent = nullptr);

    // Create a new Hot Connect group.
    // @param name       Group name
    // @param positions  Slave positions in the group
    // @return Group ID
    int createGroup(const QString& name, const QVector<int>& positions);

    // Remove a Hot Connect group.
    // @param groupId  Group ID to remove
    // @return true if removal was successful
    bool removeGroup(int groupId);

    // Activate a Hot Connect group.
    // @param groupId  Group ID to activate
    // @return true if activation was successful; currently false without backend
    bool activateGroup(int groupId);

    // Deactivate a Hot Connect group.
    // @param groupId  Group ID to deactivate
    // @return true if deactivation was successful; currently false without backend
    bool deactivateGroup(int groupId);

    // Get information about a specific group.
    // @param groupId  Group ID
    // @return HotConnectGroup structure
    HotConnectGroup groupInfo(int groupId) const;

    // Get all Hot Connect groups.
    // @return Vector of HotConnectGroup structures
    QVector<HotConnectGroup> allGroups() const;

    // Get state change history for a group.
    // @param groupId  Group ID
    // @return Vector of HotConnectEvent structures
    QVector<HotConnectEvent> groupHistory(int groupId) const;

    // Get the number of active groups.
    // @return Number of active groups
    int activeGroupCount() const;

    // Check if a group is active.
    // @param groupId  Group ID
    // @return true if group is active
    bool isGroupActive(int groupId) const;

signals:
    // Emitted when a group is activated.
    // @param groupId  Group ID
    void groupActivated(int groupId);

    // Emitted when a group is deactivated.
    // @param groupId  Group ID
    void groupDeactivated(int groupId);

    // Emitted when a group state changes.
    // @param groupId  Group ID
    // @param state    New group state
    void groupStateChanged(int groupId, HotConnectGroupState state);

private:
    QHash<int, HotConnectGroup> groups_;           // Groups by ID
    QHash<int, QVector<HotConnectEvent>> history_; // History per group
    int nextGroupId_ = 1;                          // Next group ID
    static constexpr int kMaxHistory = 500;        // Maximum history entries
};
