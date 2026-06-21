#pragma once

// MasterManagerService — manages EtherCAT master lifecycle.
//
// Provides master state monitoring, configuration, diagnostics, and restart
// capabilities. Wraps EcatClient for daemon communication and maintains
// cached master information.
//
// This service provides comprehensive EtherCAT master management. It handles:
//   - Master state monitoring (Unknown, Idle, Active, Error, Configuring)
//   - Master configuration management
//   - Master diagnostics and health checks
//   - Master restart capabilities
//   - Master information caching
//
// Usage:
//   ServiceContainer *container = ...;
//   MasterManagerService *master = container->masterManager();
//   MasterMgrState state = master->masterState();
//   MasterMgrInfo info = master->masterInfo();
//   MasterMgrConfig config;
//   config.adapterName = "eth0";
//   config.cycleTime = 1000;
//   master->configureMaster(config);
//   master->restartMaster();
//   MasterMgrDiagnosticResult diag = master->diagnoseMaster();
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   marshals daemon communication to the main thread internally.
//
// Performance:
//   - State monitoring is O(1)
//   - Configuration is O(1) for local state, O(n) for daemon communication
//   - Diagnostics are O(n) where n is number of checks
//   - Restart is O(1) for request, O(n) for completion

#include <QObject>
#include <QString>

class EcatClient;

// Master state enumeration.
enum class MasterMgrState {
  Unknown,      // State is unknown
  Idle,         // Master is idle (no active communication)
  Active,       // Master is actively communicating
  Error,        // Master is in error state
  Configuring,  // Master is being configured
};

// Master information structure.
struct MasterMgrInfo {
  QString version;         // Master software version
  QString buildDate;       // Master build date
  QString adapterName;     // Network adapter name
  QString adapterMac;      // Network adapter MAC address
  int slaveCount = 0;      // Number of connected slaves
  MasterMgrState masterState = MasterMgrState::Unknown;  // Master state
  int cycleTime = 0;       // Cycle time in microseconds
  int sync0Time = 0;       // SYNC0 time in microseconds
  int errorCount = 0;      // Error count
};

// Master configuration structure.
struct MasterMgrConfig {
  QString adapterName;     // Network adapter name
  int cycleTime = 1000;    // Cycle time in microseconds
  int sync0Time = 0;       // SYNC0 time in microseconds
  int watchdogTimeout = 1000;  // Watchdog timeout in milliseconds
  int debugLevel = 0;      // Debug level (0-3)
};

// Master diagnostic result structure.
struct MasterMgrDiagnosticResult {
  bool success = false;    // Whether diagnostics passed
  QString summary;         // Diagnostic summary
  QStringList details;     // Detailed diagnostic information
  int errorCode = 0;       // Error code (if failed)
};

class MasterManagerService : public QObject {
  Q_OBJECT
public:
  explicit MasterManagerService(EcatClient *client,
                                QObject *parent = nullptr);

  // Get the current master state.
  // @return MasterMgrState enumeration
  MasterMgrState masterState() const { return state_; }

  // Configure the master with new settings.
  // @param config  MasterMgrConfig structure
  // @return true if configuration was successful
  bool configureMaster(const MasterMgrConfig &config);

  // Run diagnostics on the master.
  // @return MasterMgrDiagnosticResult with diagnostic information
  MasterMgrDiagnosticResult diagnoseMaster();

  // Restart the master.
  // @return true if restart was initiated successfully
  bool restartMaster();

  // Get the current master information.
  // @return MasterMgrInfo structure
  MasterMgrInfo masterInfo() const { return info_; }

  // Refresh master information from the daemon.
  void refresh();

signals:
  // Emitted when the master state changes.
  // @param state  New master state
  void masterStateChanged(const MasterMgrState &state);

  // Emitted when a master error occurs.
  // @param error  Human-readable error message
  void masterError(const QString &error);

  // Emitted when master information is updated.
  // @param info  Updated MasterMgrInfo structure
  void masterInfoUpdated(const MasterMgrInfo &info);

private:
  // Update master information from daemon response text.
  void updateFromMasterText(const QString &text);

  // Set the master state and emit signal if changed.
  void setState(MasterMgrState state);

  EcatClient *client_;                      // TCP client to ecatd daemon
  MasterMgrState state_ = MasterMgrState::Unknown;  // Current master state
  MasterMgrInfo info_;                      // Cached master information
};
