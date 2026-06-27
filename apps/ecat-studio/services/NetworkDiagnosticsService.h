#pragma once

// NetworkDiagnosticsService — monitors network health for EtherCAT ports.
//
// Tracks link status, error counters (CRC, frame, lost, overrun), bandwidth
// utilization, latency, and jitter. Provides per-port status queries and
// periodic health updates via timer-based polling. Offline start requests do
// not synthesize an active diagnostics session.
//
// Integrates with EcatClient for daemon communication. Emits healthUpdated()
// signals at a configurable interval (default 1s).
//
// This service provides comprehensive network diagnostics for the EtherCAT
// network. It handles:
//   - Per-port link status monitoring
//   - Error counter tracking (CRC, frame, lost, overrun)
//   - Bandwidth utilization monitoring
//   - Latency and jitter measurement
//   - Network health assessment (Good, Degraded, Critical)
//   - Periodic health updates via polling
//
// Usage:
//   ServiceContainer *container = ...;
//   NetworkDiagnosticsService *network = container->networkDiagnostics();
//   network->startMonitoring(1000);  // Poll every 1 second
//   NetworkHealth health = network->currentHealth();
//   PortStatus port = network->portStatus(0);
//   ErrorCounters errors = network->errorCounters();
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   uses a QTimer for periodic polling, which runs on the main thread.
//
// Performance:
//   - Polling interval is configurable (default 1000ms)
//   - Status updates are O(n) where n is number of ports
//   - Error counter tracking is O(1) per update

#include <QObject>
#include <QJsonObject>
#include <QTimer>
#include <QVector>

class EcatClient;

// Status of a single network port.
struct PortStatus {
  int port = -1;           // Port number
  bool linkUp = false;     // Whether link is up
  int speedMbps = 0;       // Link speed in Mbps
  bool fullDuplex = false; // Whether full duplex is active
  int errorCount = 0;      // Error count for this port
};

// Error counters for the network.
struct ErrorCounters {
  quint64 crc = 0;      // CRC error count
  quint64 frame = 0;    // Frame error count
  quint64 lost = 0;     // Lost frame count
  quint64 overrun = 0;  // Overrun error count
};

// Information about a detected error.
struct ErrorInfo {
  qint64 timestampMs = 0;  // Error timestamp (ms since epoch)
  int port = -1;           // Port where error occurred
  QString type;            // Error type
  QString description;     // Human-readable error description
};

// Overall network health status.
struct NetworkHealth {
  enum class Status { 
    Unknown,    // No sampled port/link evidence
    Good,       // All ports healthy
    Degraded,   // Some ports have issues
    Critical    // Significant network problems
  };
  Status overall = Status::Unknown; // Overall health status
  int portCount = 0;              // Total number of ports
  int activePorts = 0;            // Number of active ports
  int errorCount = 0;             // Total error count
  double bandwidth = 0.0;         // Bandwidth utilization (0.0-1.0)
  double latencyMs = 0.0;         // Average latency in milliseconds
  double jitterMs = 0.0;          // Jitter in milliseconds
};

class NetworkDiagnosticsService : public QObject {
  Q_OBJECT
public:
  explicit NetworkDiagnosticsService(EcatClient *client,
                                     QObject *parent = nullptr);

  // Start periodic network monitoring.
  // @param intervalMs  Polling interval in milliseconds (default: 1000ms)
  void startMonitoring(int intervalMs = 1000);

  // Stop periodic network monitoring.
  void stopMonitoring();

  // Check if monitoring is currently active.
  // @return true if monitoring is running
  bool isMonitoring() const;

  // Get the current network health status.
  // @return NetworkHealth structure
  NetworkHealth currentHealth() const;

  // Get the status of a specific port.
  // @param port  Port number
  // @return PortStatus structure
  PortStatus portStatus(int port) const;

  // Get the status of all ports.
  // @return Vector of PortStatus structures
  QVector<PortStatus> allPortStatus() const;

  // Get the current error counters.
  // @return ErrorCounters structure
  ErrorCounters errorCounters() const;

  // Get the current bandwidth utilization.
  // @return Bandwidth utilization (0.0-1.0)
  double bandwidthUtilization() const;

  // Reset all error counters to zero.
  void resetErrorCounters();

signals:
  // Emitted when network health is updated.
  // @param health  Updated NetworkHealth structure
  void healthUpdated(const NetworkHealth &health);

  // Emitted when a port status changes.
  // @param port    Port number
  // @param status  Updated PortStatus structure
  void portStatusChanged(int port, const PortStatus &status);

  // Emitted when an error is detected.
  // @param error  ErrorInfo structure with error details
  void errorDetected(const ErrorInfo &error);

private:
  // Poll daemon for network status.
  void poll();

  EcatClient *client_;              // TCP client to ecatd daemon
  QTimer *pollTimer_ = nullptr;     // Timer for periodic polling
  NetworkHealth health_;            // Current network health
  QVector<PortStatus> ports_;       // Per-port status
  ErrorCounters errors_;            // Error counters
  double bandwidth_ = 0.0;          // Current bandwidth utilization
  double latencyMs_ = 0.0;         // Current latency
  double jitterMs_ = 0.0;          // Current jitter
};
