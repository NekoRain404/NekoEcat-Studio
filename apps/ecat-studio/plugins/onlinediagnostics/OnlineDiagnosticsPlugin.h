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

/// @brief Workspace plugin for online EtherCAT bus diagnostics.
class OnlineDiagnosticsPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  /// Constructs the Online Diagnostics plugin with fine-grained service injection.
  /// @param service  OnlineDiagnosticsService for diagnostics data
  /// @param parent   Qt parent object (typically MainWindow)
  explicit OnlineDiagnosticsPlugin(OnlineDiagnosticsService *service,
                                   QObject *parent = nullptr);

  QString id() const override;           ///< Returns "onlinediagnostics"
  QString displayName() const override;  ///< Returns "Online Diagnostics"
  QString displayNameZh() const override; ///< Returns "在线诊断"
  QIcon icon() const override;           ///< Returns the diagnostics theme icon
  QWidget *widget() override;            ///< Returns the root container widget
  int defaultOrder() const override;     ///< Returns 28
  bool visible() const override;         ///< Returns true (always visible)

  void activate() override;              ///< Called when user switches to this tab
  void deactivate() override;            ///< Called when user switches away

  OnlineDiagnosticsService *service() const { return service_; } ///< Returns the diagnostics service
  BusMonitorWidget *busMonitor() const { return busMonitor_; }   ///< Returns the bus monitor widget
  ErrorAnalyzerWidget *errorAnalyzer() const { return errorAnalyzer_; } ///< Returns the error analyzer widget
  /// Exports the diagnostics report to a file.
  /// @param path  Destination file path
  /// @return true on successful export
  bool exportReportToFile(const QString &path);

private:
  void buildUi();       ///< Builds the tab layout with bus monitor and error analyzer
  void exportReport();  ///< Exports the report via the export button

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
