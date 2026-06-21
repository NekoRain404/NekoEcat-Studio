#pragma once

/// @brief Workspace plugin for CiA 402 state machine recommendations.
///
/// @details The State Machine workspace displays the EtherCAT state machine
/// status for all slaves and provides recommendations for state transitions.
/// It integrates with the CiA 402 drive profile to suggest appropriate
/// controlword values for drive state transitions.
///
/// Features:
///   - **State machine table**: Shows current state, recommended next state,
///     and transition impact analysis for each slave.
///   - **State change requests**: Request state transitions with confirmation
///     dialogs showing impact details.
///   - **Batch state operations**: Broadcast state changes to all slaves.
///   - **CiA 402 integration**: Recommends controlword values based on
///     current drive state (e.g. "Shutdown", "Switch On", "Enable Operation").
///   - **Color-coded severity**: Rows colored by state health
///     (ok, action needed, warning, info).
///
/// @par Constructor
///   StateMachinePlugin(ServiceContainer *container, QObject *parent = nullptr)
///
/// @par Plugin Identity
///   - id: "statemachine"
///   - defaultOrder: 60
///   - visible: always true
///
/// @par Signals
///   - stateChangeRequested(position, state): Request a slave state change
///   - allStateChangeRequested(state): Broadcast state change to all slaves
///
/// @see WorkspacePlugin, MainWindow, StateMachineRowDetail

#include "plugins/WorkspacePlugin.h"

#include <QColor>
#include <QStringList>

class QTableWidget;
class QLabel;
class ServiceContainer;

class StateMachinePlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit StateMachinePlugin(ServiceContainer *container,
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

  // UI update surface — MainWindow calls these with pre-computed data.
  void setRows(const QStringList &headers, const QList<QStringList> &rows);
  void setSummary(const QString &text, const QString &severity = QString());
  void setSummaryToolTip(const QString &tip);
  void setDetail(const QString &text, const QString &severity = QString());
  void setDetailToolTip(const QString &tip);

  // Selection
  int currentRow() const;
  void setCurrentCell(int row, int column);
  int rowCount() const;
  bool isRowHidden(int row) const;
  void resizeColumnsToContents();

  // Table accessor for MainWindow integration.
  QTableWidget *table() const;
  QLabel *summaryLabel() const;
  QLabel *detailLabel() const;

  // Color scheme for state machine rows.
  static constexpr const char *kOkColor = "#22c55e";
  static constexpr const char *kActionColor = "#f59e0b";
  static constexpr const char *kWarningColor = "#ef4444";
  static constexpr const char *kInfoColor = "#60a5fa";

signals:
  void stateChangeRequested(int position, const QString &state);
  void allStateChangeRequested(const QString &state);

private:
  void buildUi();

  ServiceContainer *container_;
  QWidget *containerWidget_ = nullptr;
  QTableWidget *table_ = nullptr;
  QLabel *summaryLabel_ = nullptr;
  QLabel *detailLabel_ = nullptr;
};
