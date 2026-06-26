#pragma once

// EcatHealthService — monitors EtherCAT-specific health metrics.
//
// Aggregates master state, slave states, DC sync status, AL events, and
// watchdog status into a 0-100 health score with a letter grade (A-F).
//
// Uses EventBus for topology changes and polls the daemon for state updates.
// Emits healthChanged() at a configurable interval (default 1s). Offline start
// requests do not synthesize an active health-monitoring session.
//
// Health score calculation:
//   - Base 100, minus penalties for non-OP slaves, DC drift, watchdog triggers,
//     AL errors, and unresponsive master.
//
// This service provides comprehensive EtherCAT health monitoring. It handles:
//   - Master state monitoring (responsive/unresponsive)
//   - Per-slave state tracking (OP, SAFEOP, PREOP, INIT, Error)
//   - DC synchronization status monitoring
//   - AL event status tracking
//   - Watchdog status monitoring
//   - Overall health score calculation (0-100 with letter grade)
//
// Usage:
//   ServiceContainer *container = ...;
//   EcatHealthService *health = container->ecatHealth();
//   health->startMonitoring(1000);  // Poll every 1 second
//   HealthScore score = health->overallHealth();
//   MasterState master = health->masterState();
//   SlaveState slave = health->slaveState(0);
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   uses a QTimer for periodic polling, which runs on the main thread.
//
// Performance:
//   - Polling interval is configurable (default 1000ms)
//   - Health score calculation is O(n) where n is number of slaves
//   - State updates are O(1) per slave

#include <QObject>
#include <QJsonObject>
#include <QTimer>
#include <QVector>

class EcatClient;
class EventBus;
class TopologyService;
class DcSyncService;
class AlEventService;
class WatchdogService;

// Master state information.
struct MasterState {
  QString state;           // Master state string
  bool responsive = false; // Whether master is responsive
};

// Slave state information.
struct SlaveState {
  int position = -1;       // Slave position on the bus
  QString state;           // Slave state string (OP, SAFEOP, PREOP, INIT)
  bool responding = false; // Whether slave is responding
  bool hasError = false;   // Whether slave has an error
};

// DC synchronization status.
struct DcSyncStatus {
  bool inSync = false;     // Whether DC is in sync
  double driftNs = 0.0;   // DC drift in nanoseconds
  int referencePort = 0;   // Reference port for DC sync
};

// AL event status.
struct AlEventStatus {
  quint32 events = 0;      // Number of AL events
  bool hasError = false;   // Whether there are error events
  QString lastError;       // Last error message
};

// Watchdog status.
struct WatchdogStatus {
  bool triggered = false;  // Whether watchdog has been triggered
  int expiredSlaves = 0;   // Number of slaves with expired watchdog
  QString detail;          // Watchdog status detail
};

// Overall health score with breakdown.
struct HealthScore {
  int score = 100;         // Health score (0-100)
  QString grade;           // Letter grade (A, B, C, D, F)
  QString summary;         // Human-readable health summary
  int totalSlaves = 0;     // Total number of slaves
  int opSlaves = 0;        // Number of OP slaves
  int safeOpSlaves = 0;    // Number of SAFEOP slaves
  int preOpSlaves = 0;     // Number of PREOP slaves
  int initSlaves = 0;      // Number of INIT slaves
  int errorSlaves = 0;     // Number of error slaves
};

class EcatHealthService : public QObject {
  Q_OBJECT
public:
  explicit EcatHealthService(EcatClient *client, EventBus *bus,
                             TopologyService *topology, DcSyncService *dcSync,
                             AlEventService *alEvent,
                             WatchdogService *watchdog,
                             QObject *parent = nullptr);

  // Start periodic health monitoring.
  // @param intervalMs  Polling interval in milliseconds (default: 1000ms)
  void startMonitoring(int intervalMs = 1000);

  // Stop periodic health monitoring.
  void stopMonitoring();

  // Check if monitoring is currently active.
  // @return true if monitoring is running
  bool isMonitoring() const;

  // Get the current master state.
  // @return MasterState structure
  MasterState masterState() const;

  // Get the state of a specific slave.
  // @param position  Slave position on the bus
  // @return SlaveState structure
  SlaveState slaveState(int position) const;

  // Get the current DC synchronization status.
  // @return DcSyncStatus structure
  DcSyncStatus dcSyncStatus() const;

  // Get the current AL event status.
  // @return AlEventStatus structure
  AlEventStatus alEventStatus() const;

  // Get the current watchdog status.
  // @return WatchdogStatus structure
  WatchdogStatus watchdogStatus() const;

  // Get the overall health score.
  // @return HealthScore structure with score, grade, and breakdown
  HealthScore overallHealth() const;

signals:
  // Emitted when the overall health score changes.
  // @param score  Updated HealthScore structure
  void healthChanged(const HealthScore &score);

  // Emitted when a slave state changes.
  // @param position  Slave position
  // @param state     Updated SlaveState structure
  void stateChanged(int position, const SlaveState &state);

private:
  // Poll daemon for health status.
  void poll();

  EcatClient *client_;           // TCP client to ecatd daemon
  EventBus *bus_;                // Event bus for topology changes
  TopologyService *topology_;    // Topology service for slave info
  DcSyncService *dcSync_;        // DC sync service
  AlEventService *alEvent_;      // AL event service
  WatchdogService *watchdog_;    // Watchdog service
  QTimer *pollTimer_ = nullptr;  // Timer for periodic polling

  MasterState master_;           // Current master state
  QVector<SlaveState> slaves_;   // Current slave states
  DcSyncStatus dcSyncStatus_;    // Current DC sync status
  AlEventStatus alEventStatus_;  // Current AL event status
  WatchdogStatus watchdogStatus_; // Current watchdog status
  HealthScore health_;           // Current health score
};
