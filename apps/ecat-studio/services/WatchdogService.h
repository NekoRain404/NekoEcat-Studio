#pragma once

// WatchdogService — monitors EtherCAT watchdog status including timeout
// counters, trigger counts, per-slave watchdog state, and last trigger
// timestamps. Connects to EventBus for topology changes and polls the
// daemon for watchdog status only while a live daemon connection exists.
//
// This service provides comprehensive watchdog monitoring for the EtherCAT
// network. It handles:
//   - Per-slave watchdog status tracking
//   - Timeout count monitoring
//   - Trigger count tracking
//   - Last trigger timestamp recording
//   - Topology change integration
//   - Periodic status polling from daemon
//   - Offline start requests remain inactive instead of synthesizing status
//
// Usage:
//   ServiceContainer *container = ...;
//   WatchdogService *watchdog = container->watchdog();
//   watchdog->startMonitoring(1000);  // Poll every 1 second
//   QJsonObject status = watchdog->currentStatus();
//   QVector<WatchdogSlaveStatus> slaves = watchdog->slaveStatuses();
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   uses a QTimer for periodic polling, which runs on the main thread.
//
// Performance:
//   - Polling interval is configurable (default 1000ms)
//   - Status updates are O(n) where n is number of slaves
//   - Topology changes trigger immediate status refresh

#include "EthercatTypes.h"
#include <QDateTime>
#include <QJsonObject>
#include <QObject>
#include <QVector>

class QTimer;
class EventBus;
class EcatClient;

// Watchdog status for a single slave.
struct WatchdogSlaveStatus {
    int position = -1;        // Slave position on the bus
    bool watchdogOk = true;   // Whether watchdog is healthy
    int timeoutCount = 0;     // Number of watchdog timeouts
    int triggerCount = 0;     // Number of watchdog triggers
    qint64 lastTriggerMs = 0; // Last trigger timestamp (ms since epoch)
};

class WatchdogService : public QObject {
    Q_OBJECT
public:
    explicit WatchdogService(EventBus* bus, EcatClient* client, QObject* parent = nullptr);

    // Start periodic watchdog monitoring when the daemon connection is live.
    // Offline calls are ignored and leave the service inactive.
    // @param intervalMs  Polling interval in milliseconds (default: 1000ms)
    void startMonitoring(int intervalMs = 1000);

    // Stop periodic watchdog monitoring.
    void stopMonitoring();

    // Check if monitoring is currently active.
    // @return true if monitoring is running
    bool isMonitoring() const { return running_; }

    // Get the current watchdog status as a JSON object.
    // @return JSON object with overall watchdog status
    QJsonObject currentStatus() const;

    // Get watchdog status for all slaves.
    // @return Vector of WatchdogSlaveStatus structures
    QVector<WatchdogSlaveStatus> slaveStatuses() const;

signals:
    // Emitted when a watchdog timeout is detected for a slave.
    // @param slavePosition  Slave position on the bus
    // @param reason         Human-readable reason for the trigger
    void watchdogTriggered(int slavePosition, const QString& reason);

    // Emitted when overall watchdog status changes.
    // @param status  JSON object with updated watchdog status
    void watchdogStatusChanged(const QJsonObject& status);

private slots:
    // Handle topology changes from EventBus.
    void onTopologyChanged(const QVector<SlaveInfo>& slaves);

    // Poll daemon for watchdog status.
    void poll();

private:
    // Update slave statuses based on current topology.
    void updateSlaveStatuses(const QVector<SlaveInfo>& slaves);

    EventBus* bus_;           // Event bus for topology changes
    EcatClient* client_;      // TCP client to ecatd daemon
    QTimer* timer_ = nullptr; // Timer for periodic polling
    bool running_ = false;    // Whether monitoring is active

    int totalTimeouts_ = 0;                      // Total timeout count across all slaves
    int totalTriggers_ = 0;                      // Total trigger count across all slaves
    qint64 lastTriggerMs_ = 0;                   // Last trigger timestamp
    QVector<WatchdogSlaveStatus> slaveStatuses_; // Per-slave watchdog status
};
