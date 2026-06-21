#pragma once

/// @brief Workspace plugin for Application-Layer (AL) event log viewing.
///
/// @details The AL Events workspace displays timestamped application-layer
/// event entries from the EtherCAT master. These events include state
/// changes, error conditions, and diagnostic messages from individual
/// slaves and the master itself.
///
/// Features:
///   - **Event log table**: Time-stamped entries with severity level,
///     source identification, error code, and descriptive message.
///   - **Severity filtering**: Filter events by severity level
///     (error, warning, info, all).
///   - **Auto-scroll**: Automatically scrolls to the latest event.
///   - **Clear log**: Clear all accumulated events.
///   - **Real-time updates**: Events are pushed via EventBus::alEvent
///     signals from AlEventService (polled every 1s).
///
/// @par Constructor
///   AlEventPlugin(EventBus *bus, AlEventService *service, QObject *parent = nullptr)
///   Uses fine-grained injection pattern.
///
/// @par Plugin Identity
///   - id: "alevent"
///   - defaultOrder: 65
///   - visible: always true
///
/// @see WorkspacePlugin, AlEventService, EventBus

#include "plugins/WorkspacePlugin.h"

#include <QJsonObject>

class QTableWidget;
class QComboBox;
class EventBus;
class AlEventService;

class AlEventPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit AlEventPlugin(EventBus *bus, AlEventService *service,
                         QObject *parent = nullptr);

  // WorkspacePlugin identity
  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

private slots:
  void handleAlEventUpdate(const QJsonObject &data);
  void applySeverityFilter();

private:
  // Build the toolbar (severity combo + clear button) and event table.
  void buildUi();
  // Parse the JSON payload and append rows to the table.
  void populateTable(const QJsonObject &data);
  // Re-apply the current severity filter to every row.
  void updateFilterVisibility();

  EventBus *bus_;
  AlEventService *service_;
  QWidget *container_     = nullptr;
  QTableWidget *table_    = nullptr;
  QComboBox *filterCombo_ = nullptr;
};
