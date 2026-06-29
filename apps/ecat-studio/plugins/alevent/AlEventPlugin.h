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

/// @brief Workspace plugin for Application-Layer (AL) event log viewing.
class AlEventPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  /// Constructs the AL Event plugin with fine-grained service injection.
  /// @param bus      EventBus for receiving AL event signals
  /// @param service  AlEventService for event data
  /// @param parent   Qt parent object (typically MainWindow)
  explicit AlEventPlugin(EventBus *bus, AlEventService *service,
                         QObject *parent = nullptr);

  // WorkspacePlugin identity
  QString id() const override;           ///< Returns "alevent"
  QString displayName() const override;  ///< Returns "AL Events"
  QString displayNameZh() const override; ///< Returns "应用层事件"
  QWidget *widget() override;            ///< Returns the root container widget
  int defaultOrder() const override;     ///< Returns 65
  bool visible() const override;         ///< Returns true (always visible)

private slots:
  void handleAlEventUpdate(const QJsonObject &data); ///< Handles incoming AL event data from EventBus
  void applySeverityFilter();  ///< Applies the current severity filter to table rows

private:
  void buildUi();  ///< Builds the toolbar (severity combo + clear button) and event table
  void populateTable(const QJsonObject &data); ///< Parses the JSON payload and appends rows
  void updateFilterVisibility(); ///< Re-applies the current severity filter to every row

  EventBus *bus_;
  AlEventService *service_;
  QWidget *container_     = nullptr;
  QTableWidget *table_    = nullptr;
  QComboBox *filterCombo_ = nullptr;
};
