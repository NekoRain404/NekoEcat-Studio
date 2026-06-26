#pragma once

/// @brief Workspace plugin for real-time performance monitoring.
///
/// @details Provides a performance dashboard with latency monitoring,
/// throughput monitoring, resource monitoring, and quality assessment.
/// Integrates LatencyMonitorWidget and ThroughputMonitorWidget.
///
/// @par Plugin Identity
///   - id: "realtimeperf"
///   - defaultOrder: 32
///   - visible: always true

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QPushButton;
class QTabWidget;
class QDoubleSpinBox;
class QSpinBox;
class LatencyMonitorWidget;
class ThroughputMonitorWidget;
class RealtimePerformanceService;

class RealtimePerformancePlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit RealtimePerformancePlugin(RealtimePerformanceService *service,
                                     QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  RealtimePerformanceService *service() const { return service_; }
  LatencyMonitorWidget *latencyMonitor() const { return latencyMonitor_; }
  ThroughputMonitorWidget *throughputMonitor() const { return throughputMonitor_; }
  bool exportReportToFile(const QString &path);

private:
  void buildUi();
  void buildDashboardTab();
  void buildLatencyTab();
  void buildThroughputTab();
  void buildResourceTab();
  void exportReport();

  RealtimePerformanceService *service_;
  QWidget *containerWidget_ = nullptr;
  QTabWidget *tabWidget_ = nullptr;

  // Dashboard
  QLabel *qualityLabel_ = nullptr;
  QLabel *qualityScoreLabel_ = nullptr;
  QLabel *latencyAvgLabel_ = nullptr;
  QLabel *throughputLabel_ = nullptr;
  QLabel *cpuLabel_ = nullptr;

  // Latency
  LatencyMonitorWidget *latencyMonitor_ = nullptr;
  QDoubleSpinBox *thresholdSpin_ = nullptr;
  QSpinBox *historyWindowSpin_ = nullptr;

  // Throughput
  ThroughputMonitorWidget *throughputMonitor_ = nullptr;

  // Resource
  QLabel *memLabel_ = nullptr;
  QLabel *threadLabel_ = nullptr;
  QLabel *socketLabel_ = nullptr;
  QLabel *filesLabel_ = nullptr;

  // Controls
  QPushButton *startStopBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QLabel *statusLabel_ = nullptr;
};
