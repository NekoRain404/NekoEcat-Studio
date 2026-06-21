#pragma once

/// @brief Workspace plugin for diagnostics events and topology baseline monitoring.
///
/// @details The Diagnostics workspace provides runtime monitoring of EtherCAT
/// bus health, host system diagnostics, and topology change detection. It
/// displays a severity-coded event log with filtering and export capabilities.
///
/// Features:
///   - **Diagnostics event table**: Time-stamped events with severity levels
///     (error, warning, info) and source identification.
///   - **Severity filtering**: Filter events by level (error/warning/info/all).
///   - **Text filtering**: Filter events by text content.
///   - **Topology baseline**: Capture and compare topology snapshots for
///     change detection (new/removed/changed slaves).
///   - **Host health checks**: Run host-level diagnostics (kernel modules,
///     NIC status, IgH master state) and display results.
///   - **Diagnostics export**: Export event log to file for offline analysis.
///   - **Summary statistics**: Count of errors, warnings, and info events.
///
/// @par Constructor
///   DiagnosticsPlugin(ServiceContainer *container, QObject *parent = nullptr)
///
/// @par Plugin Identity
///   - id: "diagnostics"
///   - defaultOrder: 70
///   - visible: always true
///
/// @see WorkspacePlugin, MainWindow, DiagnosticsEventDetail

#include "plugins/WorkspacePlugin.h"

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableWidget;
class ServiceContainer;

class DiagnosticsPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit DiagnosticsPlugin(ServiceContainer *container,
                             QObject *parent = nullptr);

  // WorkspacePlugin identity
  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  // Lifecycle
  void activate() override;
  void deactivate() override;
  void onSettingsChanged(const AppSettings &settings) override;
  void onConnectionChanged(bool connected) override;

  // Table access
  QTableWidget *diagnosticsTable() const;
  QLineEdit *diagnosticsFilter() const;
  QComboBox *diagnosticsLevelFilter() const;
  QLabel *diagnosticsSummaryLabel() const;

  // Topology baseline
  QLabel *topologyBaselineLabel() const;
  QPushButton *captureBaselineButton() const;
  QPushButton *clearBaselineButton() const;

  // Apply filter to diagnostics table
  void filterDiagnosticsTable();

  // Update diagnostics summary label
  void updateDiagnosticsSummary();

  // Export diagnostics report to file
  void exportDiagnosticsReport(QWidget *parentWidget);

private:
  void buildUi();

  ServiceContainer *container_;
  QWidget *containerWidget_ = nullptr;
  QTableWidget *table_ = nullptr;
  QLineEdit *filter_ = nullptr;
  QComboBox *levelFilter_ = nullptr;
  QLabel *summaryLabel_ = nullptr;
  QLabel *baselineLabel_ = nullptr;
  QPushButton *captureBtn_ = nullptr;
  QPushButton *clearBtn_ = nullptr;
};
