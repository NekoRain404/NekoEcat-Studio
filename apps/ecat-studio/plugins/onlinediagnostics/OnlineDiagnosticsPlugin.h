#pragma once

/// @brief Workspace plugin for online EtherCAT bus diagnostics.
///
/// @details Provides real-time bus monitoring, error analysis, performance
/// metrics, and health status alerts. Integrates BusMonitorWidget for traffic
/// visualization and ErrorAnalyzerWidget for error classification.
///
/// @par Constructor
///   OnlineDiagnosticsPlugin(OnlineDiagnosticsService *service, QObject *parent = nullptr)
///
/// @par Plugin Identity
///   - id: "onlinediagnostics"
///   - defaultOrder: 28
///   - visible: always true

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QPushButton;
class QTabWidget;
class BusMonitorWidget;
class ErrorAnalyzerWidget;
class OnlineDiagnosticsService;

class OnlineDiagnosticsPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit OnlineDiagnosticsPlugin(OnlineDiagnosticsService *service,
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

  OnlineDiagnosticsService *service() const { return service_; }
  BusMonitorWidget *busMonitor() const { return busMonitor_; }
  ErrorAnalyzerWidget *errorAnalyzer() const { return errorAnalyzer_; }
  bool exportReportToFile(const QString &path);

private:
  void buildUi();
  void exportReport();

  OnlineDiagnosticsService *service_;
  QWidget *containerWidget_ = nullptr;
  QTabWidget *tabWidget_ = nullptr;
  BusMonitorWidget *busMonitor_ = nullptr;
  ErrorAnalyzerWidget *errorAnalyzer_ = nullptr;
  QPushButton *startStopBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QLabel *healthLabel_ = nullptr;
  QLabel *statusLabel_ = nullptr;
  QLabel *perfLabel_ = nullptr;
};
