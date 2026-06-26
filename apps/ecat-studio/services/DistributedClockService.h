#pragma once

// DistributedClockService — manages EtherCAT Distributed Clock sync.
//
// Provides reference clock detection, Sync0/Sync1 configuration, drift
// monitoring, and jitter measurement. Wraps DcSyncService and EcatClient
// for daemon communication.
//
// This service provides Distributed Clock (DC) synchronization management
// for the EtherCAT network. It handles:
//   - Reference clock detection and management
//   - Sync0/Sync1 configuration for slaves
//   - Drift monitoring and detection
//   - Jitter measurement and statistics
//   - DC sync data processing and updates
//
// Usage:
//   ServiceContainer *container = ...;
//   DistributedClockService *dc = container->distributedClock();
//   int refClock = dc->referenceClock();
//   dc->configureSync(0, 1000, 0);  // Configure slave 0
//   DriftStatus drift = dc->driftStatus();
//   JitterStats jitter = dc->jitterStatistics();
//   dc->requestUpdate();
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   marshals daemon communication to the main thread internally.
//
// Performance:
//   - DC sync configuration is O(1) per slave
//   - Drift monitoring is O(n) where n is number of slaves
//   - Jitter statistics are O(1) for retrieval, O(n) for computation

#include <QObject>
#include <QJsonObject>

class EcatClient;

// Drift status for a slave.
struct DriftStatus {
  int slave = -1;           // Slave position
  double drift = 0.0;       // Current drift in nanoseconds
  double threshold = 1000.0; // Drift threshold in nanoseconds
  QString status;           // Status string (OK, Warning, Error)
};

// Jitter statistics.
struct JitterStats {
  double min = 0.0;         // Minimum jitter in nanoseconds
  double max = 0.0;         // Maximum jitter in nanoseconds
  double avg = 0.0;         // Average jitter in nanoseconds
  double stddev = 0.0;      // Standard deviation in nanoseconds
  int sampleCount = 0;      // Number of samples
  int windowSize = 1000;    // Statistics window size
};

class DistributedClockService : public QObject {
  Q_OBJECT
public:
  explicit DistributedClockService(EcatClient *client,
                                   QObject *parent = nullptr);

  // Get the reference clock slave position.
  // @return Slave position (-1 if no reference clock)
  int referenceClock() const { return refClock_; }

  // Configure Sync0/Sync1 for a slave.
  // @param slave  Slave position
  // @param sync0  Sync0 period in microseconds
  // @param sync1  Sync1 period in microseconds (0 to disable)
  // @return true only after backend-confirmed DC configuration is available.
  bool configureSync(int slave, int sync0, int sync1);

  // Get the current drift status.
  // @return DriftStatus structure
  DriftStatus driftStatus() const { return driftStatus_; }

  // Get the current jitter statistics.
  // @return JitterStats structure
  JitterStats jitterStatistics() const { return jitterStats_; }

  // Request a DC sync update from the daemon.
  void requestUpdate();

signals:
  // Emitted when sync configuration changes.
  // @param slave  Slave position
  // @param sync0  Sync0 period
  // @param sync1  Sync1 period
  void syncChanged(int slave, int sync0, int sync1);

  // Emitted when drift is detected.
  // @param slave  Slave position
  // @param drift  Drift value in nanoseconds
  void driftDetected(int slave, double drift);

  // Emitted when jitter statistics are updated.
  // @param stats  Updated JitterStats structure
  void jitterUpdated(const JitterStats &stats);

  // Emitted when DC sync data is updated.
  // @param data  JSON object with DC sync data
  void dcSyncUpdate(const QJsonObject &data);

private:
  // Process DC sync data from daemon.
  void processDcSyncData(const QJsonObject &data);

  EcatClient *client_;          // TCP client to ecatd daemon
  int refClock_ = -1;           // Reference clock slave position
  DriftStatus driftStatus_;     // Current drift status
  JitterStats jitterStats_;     // Current jitter statistics
};
