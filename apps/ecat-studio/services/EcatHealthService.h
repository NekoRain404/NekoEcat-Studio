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

/// @brief Master state information.
struct MasterState {
  QString state;           ///< Master state string.
  bool responsive = false; ///< Whether master is responsive.
};

/// @brief Slave state information.
struct SlaveState {
  int position = -1;       ///< Slave position on the bus.
  QString state;           ///< Slave state string (OP, SAFEOP, PREOP, INIT).
  bool responding = false; ///< Whether slave is responding.
  bool hasError = false;   ///< Whether slave has an error.
};

/// @brief DC synchronization status.
struct DcSyncStatus {
  bool inSync = false;     ///< Whether DC is in sync.
  double driftNs = 0.0;   ///< DC drift in nanoseconds.
  int referencePort = 0;   ///< Reference port for DC sync.
};

/// @brief AL event status.
struct AlEventStatus {
  quint32 events = 0;      ///< Number of AL events.
  bool hasError = false;   ///< Whether there are error events.
  QString lastError;       ///< Last error message.
};

/// @brief Watchdog status.
struct WatchdogStatus {
  bool triggered = false;  ///< Whether watchdog has been triggered.
  int expiredSlaves = 0;   ///< Number of slaves with expired watchdog.
  QString detail;          ///< Watchdog status detail.
};

/// @brief Overall health score with breakdown by slave state.
struct HealthScore {
  int score = 0;           ///< Health score (0-100).
  QString grade = QStringLiteral("Unknown"); ///< Health letter grade (A-F).
  QString summary = QStringLiteral("No EtherCAT health evidence sampled"); ///< Human-readable summary.
  int totalSlaves = 0;     ///< Total number of slaves.
  int opSlaves = 0;        ///< Number of slaves in OP state.
  int safeOpSlaves = 0;    ///< Number of slaves in SAFEOP state.
  int preOpSlaves = 0;     ///< Number of slaves in PREOP state.
  int initSlaves = 0;      ///< Number of slaves in INIT state.
  int errorSlaves = 0;     ///< Number of slaves in error state.
};

/// @brief Monitors EtherCAT-specific health metrics.
///
/// Aggregates master state, slave states, DC sync status, AL events, and
/// watchdog status into a 0-100 health score with a letter grade (A-F).
/// Uses EventBus for topology changes and polls the daemon for state updates.
class EcatHealthService : public QObject {
  Q_OBJECT
public:
  /// @brief Construct the EtherCAT health service.
  /// @param client    TCP client to the ecatd daemon.
  /// @param bus       Event bus for topology changes.
  /// @param topology  Topology service for slave information.
  /// @param dcSync    DC synchronization service.
  /// @param alEvent   AL event service.
  /// @param watchdog  Watchdog service.
  /// @param parent    Parent QObject.
  explicit EcatHealthService(EcatClient *client, EventBus *bus,
                             TopologyService *topology, DcSyncService *dcSync,
                             AlEventService *alEvent,
                             WatchdogService *watchdog,
                             QObject *parent = nullptr);

  /// @brief Start periodic health monitoring.
  /// @param intervalMs  Polling interval in milliseconds (default: 1000ms).
  void startMonitoring(int intervalMs = 1000);

  /// @brief Stop periodic health monitoring.
  void stopMonitoring();

  /// @brief Check if monitoring is currently active.
  /// @return true if monitoring is running.
  bool isMonitoring() const;

  /// @brief Get the current master state.
  /// @return MasterState structure with state string and responsiveness flag.
  MasterState masterState() const;

  /// @brief Get the state of a specific slave.
  /// @param position  Slave position on the bus.
  /// @return SlaveState structure with state, responsiveness, and error status.
  SlaveState slaveState(int position) const;

  /// @brief Get the current DC synchronization status.
  /// @return DcSyncStatus structure with sync state, drift, and reference port.
  DcSyncStatus dcSyncStatus() const;

  /// @brief Get the current AL event status.
  /// @return AlEventStatus structure with event count and error information.
  AlEventStatus alEventStatus() const;

  /// @brief Get the current watchdog status.
  /// @return WatchdogStatus structure with trigger state and expired slave count.
  WatchdogStatus watchdogStatus() const;

  /// @brief Get the overall health score with breakdown.
  /// @return HealthScore structure with 0-100 score, letter grade, and slave counts.
  HealthScore overallHealth() const;

signals:
  /// @brief Emitted when the overall health score changes.
  /// @param score  Updated HealthScore structure.
  void healthChanged(const HealthScore &score);

  /// @brief Emitted when a slave state changes.
  /// @param position  Slave position on the bus.
  /// @param state     Updated SlaveState structure.
  void stateChanged(int position, const SlaveState &state);

private:
  /// @brief Poll daemon for health status and update internal state.
  void poll();

  EcatClient *client_;           ///< TCP client to ecatd daemon.
  EventBus *bus_;                ///< Event bus for topology changes.
  TopologyService *topology_;    ///< Topology service for slave info.
  DcSyncService *dcSync_;        ///< DC synchronization service.
  AlEventService *alEvent_;      ///< AL event service.
  WatchdogService *watchdog_;    ///< Watchdog service.
  QTimer *pollTimer_ = nullptr;  ///< Timer for periodic health polling.

  MasterState master_;            ///< Current master state.
  QVector<SlaveState> slaves_;    ///< Current slave states.
  DcSyncStatus dcSyncStatus_;     ///< Current DC sync status.
  AlEventStatus alEventStatus_;   ///< Current AL event status.
  WatchdogStatus watchdogStatus_; ///< Current watchdog status.
  HealthScore health_;            ///< Current overall health score.
};
