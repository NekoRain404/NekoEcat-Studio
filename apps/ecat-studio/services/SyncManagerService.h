#pragma once

// SyncManagerService — EtherCAT Sync Manager configuration facade.
//
// Exposes the UI/service boundary for Sync Manager configuration. Write
// operations fail closed until this service is wired to a live EtherCAT master
// backend capable of applying SM/PDO/watchdog changes to real slaves.
//
// This service currently handles:
//   - Parameter validation for Sync Manager write requests
//   - Explicit errors for unsupported offline write operations
//   - Empty/default readback state inspection
//
// Usage:
//   SyncManagerService syncMgr;
//   SyncManagerConfig config;
//   config.smIndex = 0;
//   config.direction = SmDirection::Input;
//   config.pdoIndex = 0x6000;
//   config.watchdogTimeout = 1000;
//   config.enable = true;
//   // These calls return false until connected to a real backend:
//   syncMgr.configureSyncManager(0, 0, config);
//   syncMgr.assignPdo(0, 0, 0x6000);
//   syncMgr.setDirection(0, 0, SmDirection::Output);
//   syncMgr.setWatchdog(0, 0, 2000);
//   SyncManagerConfig cfg = syncMgr.syncManagerConfig(0, 0);
//   QVector<int> sms = syncMgr.syncManagers(0);
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. Sync Manager
//   operations are synchronous and block the calling thread.
//
// Performance:
//   - Offline rejection is O(1)

#include <QHash>
#include <QObject>
#include <QString>
#include <QVector>

// Sync Manager direction enumeration.
enum class SmDirection {
    Input,  // Input direction (slave to master)
    Output, // Output direction (master to slave)
    Both    // Both directions
};

// Sync Manager configuration structure.
struct SyncManagerConfig {
    int smIndex = 0;                            // Sync Manager index
    SmDirection direction = SmDirection::Input; // Direction
    int pdoIndex = 0;                           // Assigned PDO index
    int watchdogTimeout = 0;                    // Watchdog timeout in ms
    bool enable = true;                         // Whether SM is enabled
    bool virtualSm = false;                     // Whether SM is virtual
};

class SyncManagerService : public QObject {
    Q_OBJECT
public:
    explicit SyncManagerService(QObject* parent = nullptr);

    // Configure a Sync Manager for a slave.
    // @param position  Slave position
    // @param smIndex   Sync Manager index
    // @param config    SyncManagerConfig structure
    // @return true if configuration was successful
    bool configureSyncManager(int position, int smIndex, const SyncManagerConfig& config);

    // Assign a PDO to a Sync Manager.
    // @param position  Slave position
    // @param smIndex   Sync Manager index
    // @param pdoIndex  PDO index to assign
    // @return true if assignment was successful
    bool assignPdo(int position, int smIndex, int pdoIndex);

    // Set the direction of a Sync Manager.
    // @param position  Slave position
    // @param smIndex   Sync Manager index
    // @param direction SmDirection enumeration
    // @return true if direction was set successfully
    bool setDirection(int position, int smIndex, SmDirection direction);

    // Set the watchdog timeout for a Sync Manager.
    // @param position  Slave position
    // @param smIndex   Sync Manager index
    // @param timeout   Watchdog timeout in milliseconds
    // @return true if timeout was set successfully
    bool setWatchdog(int position, int smIndex, int timeout);

    // Get the configuration of a Sync Manager.
    // @param position  Slave position
    // @param smIndex   Sync Manager index
    // @return SyncManagerConfig structure
    SyncManagerConfig syncManagerConfig(int position, int smIndex) const;

    // Get all Sync Manager indices for a slave.
    // @param position  Slave position
    // @return Vector of Sync Manager indices
    QVector<int> syncManagers(int position) const;

signals:
    // Emitted when a Sync Manager is configured.
    // @param position  Slave position
    // @param smIndex   Sync Manager index
    void syncManagerConfigured(int position, int smIndex);

    // Emitted when an error occurs.
    // @param message  Human-readable error message
    void error(const QString& message);

private:
    // Per-slave Sync Manager configurations.
    QHash<int, QHash<int, SyncManagerConfig>> configs_;
};
