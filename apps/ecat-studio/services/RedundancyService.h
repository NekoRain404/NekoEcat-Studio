#pragma once

// RedundancyService — manages EtherCAT network redundancy.
// Monitors primary/secondary paths and handles failover.
//
// This service provides network redundancy management for the EtherCAT
// network. It handles:
//   - Primary/secondary path management
//   - Redundancy state tracking (SinglePath, DualPath, Failover, Error)
//   - Path state monitoring (Active, Standby, Failed)
//   - Automatic and manual failover
//   - Failback to primary path
//   - Redundancy event history
//
// Usage:
//   RedundancyService redundancy;
//   redundancy.setPrimaryPath(10);  // 10 slaves on primary
//   redundancy.setSecondaryPath(10);  // 10 slaves on secondary
//   redundancy.enableRedundancy();
//   RedundancyState state = redundancy.currentState();
//   RedundancyPath primary = redundancy.primaryPath();
//   RedundancyPath secondary = redundancy.secondaryPath();
//   redundancy.failover();  // Switch to secondary
//   redundancy.failback();  // Switch back to primary
//   QVector<RedundancyEvent> history = redundancy.redundancyHistory();
//   bool isRedundant = redundancy.isRedundant();
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. Redundancy
//   operations are synchronous and block the calling thread.
//
// Performance:
//   - Path management is O(1)
//   - Failover/failback is O(1)
//   - State monitoring is O(1)
//   - History retrieval is O(n) where n is history size

#include <QObject>
#include <QVector>
#include <QDateTime>

// Redundancy state enumeration.
enum class RedundancyState { 
  SinglePath,  // Only primary path active
  DualPath,    // Both paths active
  Failover,    // Failover to secondary path
  Error        // Redundancy error
};

// Path state enumeration.
enum class PathState { 
  Active,   // Path is active
  Standby,  // Path is in standby
  Failed,   // Path has failed
  Unknown   // Path state unknown
};

// Redundancy path structure.
struct RedundancyPath {
  int pathId = 0;              // Path ID
  PathState state = PathState::Unknown;  // Path state
  int slaveCount = 0;          // Number of slaves on path
  QDateTime lastCheck;         // Last health check timestamp
  bool isHealthy = false;      // Whether path is healthy
};

// Redundancy event structure.
struct RedundancyEvent {
  int pathId = 0;                                          // Path ID
  RedundancyState fromState = RedundancyState::SinglePath;  // Previous state
  RedundancyState toState = RedundancyState::SinglePath;    // New state
  QDateTime timestamp;                                      // Event timestamp
  bool success = false;                                     // Whether event succeeded
  QString reason;                                           // Event reason
};

class RedundancyService : public QObject {
  Q_OBJECT
public:
  explicit RedundancyService(QObject *parent = nullptr);

  // Set the primary path configuration.
  // @param slaveCount  Number of slaves on primary path
  void setPrimaryPath(int slaveCount);

  // Set the secondary path configuration.
  // @param slaveCount  Number of slaves on secondary path
  void setSecondaryPath(int slaveCount);

  // Enable redundancy.
  // @return true if redundancy was enabled successfully
  bool enableRedundancy();

  // Disable redundancy.
  // @return true if redundancy was disabled successfully
  bool disableRedundancy();

  // Perform failover to secondary path.
  // @return true if failover was successful
  bool failover();

  // Perform failback to primary path.
  // @return true if failback was successful
  bool failback();

  // Get the current redundancy state.
  // @return RedundancyState enumeration
  RedundancyState currentState() const;

  // Get the primary path information.
  // @return RedundancyPath structure
  RedundancyPath primaryPath() const;

  // Get the secondary path information.
  // @return RedundancyPath structure
  RedundancyPath secondaryPath() const;

  // Get redundancy event history.
  // @return Vector of RedundancyEvent structures
  QVector<RedundancyEvent> redundancyHistory() const;

  // Check if redundancy is enabled.
  // @return true if redundancy is enabled
  bool isRedundant() const;

signals:
  // Emitted when redundancy state changes.
  // @param state  New redundancy state
  void redundancyStateChanged(RedundancyState state);

  // Emitted when failover occurs.
  // @param fromPath  Source path ID
  // @param toPath    Target path ID
  void failoverOccurred(int fromPath, int toPath);

  // Emitted when a path state changes.
  // @param pathId  Path ID
  // @param state   New path state
  void pathStateChanged(int pathId, PathState state);

private:
  RedundancyState state_ = RedundancyState::SinglePath;  // Current state
  RedundancyPath primaryPath_;    // Primary path
  RedundancyPath secondaryPath_;  // Secondary path
  QVector<RedundancyEvent> history_;  // Event history
  static constexpr int kMaxHistory = 500;  // Maximum history entries
};
