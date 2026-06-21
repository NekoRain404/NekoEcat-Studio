#pragma once

// DiagnosticService — runs system, performance, network, and device
// diagnostics and produces structured reports with recommendations.
//
// This service provides comprehensive diagnostic capabilities for the
// EtherCAT system. It handles:
//   - System diagnostics (CPU, memory, disk, processes)
//   - Performance diagnostics (cycle time, jitter, throughput)
//   - Network diagnostics (link status, errors, bandwidth)
//   - Device diagnostics (per-slave health and status)
//   - Structured diagnostic reports with recommendations
//
// Usage:
//   ServiceContainer *container = ...;
//   DiagnosticService *diagnostics = container->diagnostic();
//   DiagnosticReport sysReport = diagnostics->runSystemDiagnostics();
//   DiagnosticReport perfReport = diagnostics->runPerformanceDiagnostics();
//   DiagnosticReport netReport = diagnostics->runNetworkDiagnostics();
//   DiagnosticReport devReport = diagnostics->runDeviceDiagnostics(0);
//   if (devReport.status == DiagnosticStatus::Fail) {
//     // Handle failure
//   }
//   QStringList recommendations = devReport.recommendations;
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. Diagnostic
//   operations are synchronous and block the calling thread.
//
// Performance:
//   - System diagnostics are O(1) for collection
//   - Performance diagnostics are O(n) where n is number of metrics
//   - Network diagnostics are O(1) for collection
//   - Device diagnostics are O(1) per slave

#include <QObject>
#include <QVector>
#include <QDateTime>
#include <QVariantMap>

// Diagnostic categories.
enum class DiagnosticCategory { 
  System,       // System-level diagnostics
  Performance,  // Performance diagnostics
  Network,      // Network diagnostics
  Device        // Device diagnostics
};

// Diagnostic status.
enum class DiagnosticStatus { 
  Pass,  // Diagnostic passed
  Warn,  // Diagnostic warning
  Fail   // Diagnostic failed
};

// Individual diagnostic item.
struct DiagnosticItem {
  QString name;                                     // Item name
  DiagnosticStatus status = DiagnosticStatus::Pass; // Item status
  QString message;                                  // Status message
  QVariantMap details;                              // Detailed information
};

// Diagnostic report structure.
struct DiagnosticReport {
  QDateTime timestamp;                                      // Report timestamp
  DiagnosticCategory category = DiagnosticCategory::System; // Report category
  DiagnosticStatus status = DiagnosticStatus::Pass;         // Overall status
  QString message;                                          // Report message
  QVector<DiagnosticItem> items;                            // Diagnostic items
  QStringList recommendations;                              // Recommendations
};

class EcatClient;
class EventBus;

class DiagnosticService : public QObject {
  Q_OBJECT
public:
  explicit DiagnosticService(EventBus *bus, EcatClient *client,
                             QObject *parent = nullptr);

  // Run system diagnostics.
  // @return DiagnosticReport with system diagnostic results
  DiagnosticReport runSystemDiagnostics();

  // Run performance diagnostics.
  // @return DiagnosticReport with performance diagnostic results
  DiagnosticReport runPerformanceDiagnostics();

  // Run network diagnostics.
  // @return DiagnosticReport with network diagnostic results
  DiagnosticReport runNetworkDiagnostics();

  // Run device diagnostics for a specific slave.
  // @param position  Slave position
  // @return DiagnosticReport with device diagnostic results
  DiagnosticReport runDeviceDiagnostics(int position);

signals:
  // Emitted when a diagnostic completes.
  // @param report  DiagnosticReport with results
  void diagnosticCompleted(const DiagnosticReport &report);

private:
  // Create a diagnostic report from items and recommendations.
  DiagnosticReport makeReport(DiagnosticCategory cat,
                              const QVector<DiagnosticItem> &items,
                              const QStringList &recommendations);

  EventBus *bus_;      // Event bus for data access
  EcatClient *client_; // TCP client to ecatd daemon
};
