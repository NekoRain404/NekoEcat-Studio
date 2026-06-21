#pragma once

// DiagnosticReportService — generates comprehensive diagnostic reports
// including bus topology, slave status, error history, performance metrics,
// DC sync status, and watchdog status.  Reports are available in Markdown
// and CSV formats.
//
// This service provides diagnostic report generation for the EtherCAT
// network. It handles:
//   - Bus topology summary and slave status
//   - Performance metrics (cycle time, jitter, frame loss)
//   - DC synchronization status per slave
//   - Watchdog status and trigger history
//   - Report export in Markdown and CSV formats
//
// Usage:
//   ServiceContainer *container = ...;
//   DiagnosticReportService *report = container->diagnosticReport();
//   QString markdown = report->generateReport();
//   report->exportReport("/path/to/report.md");
//   report->exportReportCsv("/path/to/report.csv");
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   reads data from other services but does not perform I/O operations.
//
// Performance:
//   - Report generation is O(n) where n is number of slaves
//   - Export operations are synchronous (blocking)
//   - Report data is generated on-demand (not cached)

#include <QObject>
#include <QString>

class EventBus;
class EcatClient;
class TopologyService;
class DcSyncService;
class PerformanceMonitorService;
class WatchdogService;

class DiagnosticReportService : public QObject {
  Q_OBJECT
public:
  DiagnosticReportService(EventBus *bus, EcatClient *client,
                          TopologyService *topology, DcSyncService *dcSync,
                          PerformanceMonitorService *perfMonitor,
                          WatchdogService *watchdog,
                          QObject *parent = nullptr);

  // Generate a comprehensive diagnostic report in Markdown format.
  // @return Markdown-formatted diagnostic report
  QString generateReport() const;

  // Export the diagnostic report to a file in Markdown format.
  // @param filePath  Path to the output file
  void exportReport(const QString &filePath) const;

  // Export the diagnostic report to a file in CSV format.
  // @param filePath  Path to the output file
  void exportReportCsv(const QString &filePath) const;

signals:
  // Emitted when a report is generated.
  // @param report  Markdown-formatted report content
  void reportGenerated(const QString &report);

private:
  // Generate the topology section of the report.
  QString topologySection() const;

  // Generate the slave status section of the report.
  QString slaveStatusSection() const;

  // Generate the performance metrics section of the report.
  QString performanceSection() const;

  // Generate the DC sync section of the report.
  QString dcSyncSection() const;

  // Generate the watchdog section of the report.
  QString watchdogSection() const;

  EventBus *bus_;                           // Event bus for data access
  EcatClient *client_;                      // TCP client to ecatd daemon
  TopologyService *topology_;               // Topology data source
  DcSyncService *dcSync_;                   // DC sync data source
  PerformanceMonitorService *perfMonitor_;  // Performance data source
  WatchdogService *watchdog_;               // Watchdog data source
};
