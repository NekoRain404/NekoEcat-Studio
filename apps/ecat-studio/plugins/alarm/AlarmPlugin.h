#pragma once

/// @brief Workspace plugin for system alarm management and monitoring.
///
/// @details The Alarm workspace provides a comprehensive alarm management
/// interface with real-time monitoring, severity-based filtering, and
/// lifecycle state management. It integrates with AlarmService for alarm
/// data and LoggingService for audit trail.
///
/// Features:
///   - **Real-time alarm table**: Severity-based color coding for quick identification
///   - **4 severity levels**: Info, Warning, Error, Critical
///   - **5 alarm categories**: Communication, Device, Network, System, Safety
///   - **3 lifecycle states**: Active, Acknowledged, Cleared
///   - **Multi-filter support**: Filter by level, category, and state
///   - **Acknowledge and clear actions**: Manage alarm lifecycle
///   - **Alarm history export**: Export to file for documentation
///   - **Audit trail integration**: LoggingService integration for compliance
///
/// @par Constructor
///   AlarmPlugin(AlarmService *alarmService, LoggingService *logService,
///               QObject *parent = nullptr)
///   Uses fine-grained injection pattern.
///
/// @par Plugin Identity
///   - id: "alarm"
///   - defaultOrder: 110
///   - visible: always true
///
/// @par UI Description
///   The alarm workspace displays a filterable table of system alarms.
///   Each row shows: timestamp, severity, category, source, message,
///   state, and acknowledge/clear timestamps. Filter combo boxes at the
///   top allow narrowing by level, category, and state. Action buttons
///   provide acknowledge, clear, and export operations.
///
/// @par Signals
///   - AlarmService::alarmRaised — new alarm added to table
///   - AlarmService::alarmAcknowledged — alarm state updated
///   - AlarmService::alarmCleared — alarm removed from active list
///
/// @par Usage Example
///   @code
///   // In MainWindow constructor:
///   auto *alarmService = new AlarmService(this);
///   auto *loggingService = new LoggingService(this);
///   pluginRegistry_->registerPlugin(new AlarmPlugin(alarmService, loggingService, this));
///
///   // Acknowledge selected alarms:
///   alarmPlugin->acknowledgeSelected();
///
///   // Export alarm history:
///   alarmPlugin->exportHistory();
///   @endcode
///
/// @see WorkspacePlugin, AlarmService, LoggingService

#include "plugins/WorkspacePlugin.h"
#include "services/AlarmService.h"

class QTableWidget;
class QComboBox;
class QPushButton;
class AlarmService;
class LoggingService;

/// @brief Workspace plugin for system alarm management and monitoring.
///
/// @details This plugin provides a complete alarm management interface with:
///   - Real-time alarm table with severity-based color coding
///   - Multi-filter support for level, category, and state
///   - Acknowledge and clear actions for alarm lifecycle management
///   - Alarm history export for documentation and compliance
///   - Audit trail integration with LoggingService
///
/// The plugin communicates with AlarmService for alarm data and uses
/// EventBus for real-time alarm notifications.
class AlarmPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  /// @brief Constructs the Alarm plugin with fine-grained service injection.
  /// @param alarmService  AlarmService instance for alarm data management
  /// @param logService    LoggingService instance for audit trail integration
  /// @param parent        Qt parent object (typically MainWindow)
  explicit AlarmPlugin(AlarmService *alarmService, LoggingService *logService,
                       QObject *parent = nullptr);

  // ── WorkspacePlugin Identity ──────────────────────────────────
  QString id() const override;           ///< Returns "alarm"
  QString displayName() const override;  ///< Returns "Alarm"
  QString displayNameZh() const override; ///< Returns "告警"
  QWidget *widget() override;            ///< Returns the root container widget
  int defaultOrder() const override;     ///< Returns 110
  bool visible() const override;         ///< Returns true (always visible)

  bool exportHistoryToFile(const QString &path);

private slots:
  /// @brief Handles new alarm raised events from AlarmService.
  /// @param alarm  The newly raised alarm object
  void onAlarmRaised(const Alarm &alarm);

  /// @brief Handles alarm acknowledged events from AlarmService.
  /// @param alarmId  ID of the acknowledged alarm
  void onAlarmAcknowledged(int alarmId);

  /// @brief Handles alarm cleared events from AlarmService.
  /// @param alarmId  ID of the cleared alarm
  void onAlarmCleared(int alarmId);

  /// @brief Applies current filter settings to the alarm table.
  /// @details Filters alarms based on selected level, category, and state.
  void applyFilter();

  /// @brief Acknowledges the selected alarm(s) in the table.
  void acknowledgeSelected();

  /// @brief Clears the selected alarm(s) from the table.
  void clearSelected();

  /// @brief Exports alarm history to a file.
  /// @details Opens a file dialog and exports all alarms to the selected format.
  void exportHistory();

private:
  /// @brief Builds the UI layout with filter controls and alarm table.
  void buildUi();

  /// @brief Populates a table row with alarm data.
  /// @param row    Row index to populate
  /// @param alarm  Alarm data to display
  void populateRow(int row, const Alarm &alarm);

  /// @brief Updates the visual state of a table row.
  /// @param row    Row index to update
  /// @param state  New alarm state to display
  void updateRowState(int row, AlarmState state);

  /// @brief Finds the table row index for a given alarm ID.
  /// @param alarmId  ID of the alarm to find
  /// @return Row index, or -1 if not found
  int findRowById(int alarmId) const;

  AlarmService *alarmService_;           ///< Alarm service for alarm data management
  LoggingService *logService_;           ///< Logging service for audit trail integration
  QWidget *container_ = nullptr;         ///< Root container widget
  QTableWidget *table_ = nullptr;        ///< Alarm table widget
  QComboBox *levelFilter_ = nullptr;     ///< Severity level filter combo box
  QComboBox *categoryFilter_ = nullptr;  ///< Category filter combo box
  QComboBox *stateFilter_ = nullptr;     ///< State filter combo box
  QPushButton *ackBtn_ = nullptr;        ///< Acknowledge selected alarms button
  QPushButton *clearBtn_ = nullptr;      ///< Clear selected alarms button
  QPushButton *exportBtn_ = nullptr;     ///< Export alarm history button
};
