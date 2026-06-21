#pragma once

/// @brief Workspace plugin for consistency checks and commissioning gate.
///
/// @details The Consistency workspace performs read-only validation of the
/// engineering state by comparing topology, startup SDOs, watch values,
/// I/O variable metadata, and project configuration. It acts as a
/// commissioning gate — blocking dangerous operations when critical
/// inconsistencies are detected.
///
/// Features:
///   - **Severity-coded issue table**: Displays consistency issues with
///     levels (error, warning, info, ready) across multiple scopes
///     (topology, startup, I/O variables, metadata).
///   - **Issue count queries**: Provides error/warning/info/ready counts
///     for the commissioning gate logic.
///   - **Scope filtering**: Filter issues by scope (topology, startup, etc.).
///   - **Text filtering**: Filter issues by text content.
///   - **Evidence navigation**: Click an issue to navigate to the relevant
///     workspace and focus on the evidence.
///
/// @par Constructor
///   ConsistencyPlugin(ServiceContainer *container, QObject *parent = nullptr)
///
/// @par Plugin Identity
///   - id: "consistency"
///   - defaultOrder: 67
///   - visible: always true
///
/// @see WorkspacePlugin, MainWindow, ConsistencyModel, ConsistencyDetail

#include "plugins/WorkspacePlugin.h"

#include <QList>
#include <QStringList>

class QComboBox;
class QLabel;
class QLineEdit;
class QTableWidget;
class ServiceContainer;
struct ConsistencyIssueCounts;

class ConsistencyPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit ConsistencyPlugin(ServiceContainer *container,
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
  QTableWidget *consistencyTable() const;

  // Populate the consistency table from MainWindow-gathered data.
  // Each row is a QStringList of 7 cells: level, scope, target, evidence,
  // expected, actual, action.
  void updateConsistencyView(const QList<QStringList> &rows);

  // Query issue counts by severity.
  ConsistencyIssueCounts consistencyIssueCounts() const;

  // Filter management
  QLineEdit *consistencyFilter() const;
  QComboBox *consistencyScopeFilter() const;
  QLabel *consistencySummaryLabel() const;

private:
  void buildUi();

  ServiceContainer *container_;
  QWidget *containerWidget_ = nullptr;
  QTableWidget *table_ = nullptr;
  QLineEdit *filter_ = nullptr;
  QComboBox *scopeFilter_ = nullptr;
  QLabel *summaryLabel_ = nullptr;
};
