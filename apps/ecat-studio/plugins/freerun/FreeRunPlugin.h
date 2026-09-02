#pragma once

/// @brief Workspace plugin for Free Run real-time process data monitoring.
///
/// @details The Free Run workspace provides real-time monitoring of EtherCAT
/// process data (PDO inputs/outputs) during Free Run mode. It displays a
/// table of process data entries cross-referenced against the PDO map for
/// naming and context.
///
/// Features:
///   - **Real-time process data entry table**: Shows current input/output
///     process values with PDO map cross-reference for naming.
///   - **Text filtering**: Filter entries by name or index with text search.
///   - **Changed-only filter**: Highlight entries that have drifted from
///     their baseline values.
///   - **Summary and detail panels**: Aggregate statistics and per-entry
///     information display.
///   - **Real-time chart dialogs**: Open multiple chart windows for
///     continuous visualization of selected entries.
///   - **Entry caching**: Fast lookup of entry names and values via
///     QHash caches.
///   - **Free Run state tracking**: Tracks enabled/disabled state and
///     last telemetry status.
///
/// @par Constructor
///   FreeRunPlugin(ServiceContainer *container, QObject *parent = nullptr)
///
/// @par Plugin Identity
///   - id: "freerun"
///   - defaultOrder: 35
///   - visible: always true
///
/// @par Signals
///   - chartOpened(dialog): Emitted when a new chart dialog is opened
///   - chartClosed(dialog): Emitted when a chart dialog is closed
///   - pollingIntervalChanged(ms): Emitted when the polling interval changes
///
/// @see WorkspacePlugin, MainWindow, FreeRunController

#include "plugins/WorkspacePlugin.h"

#include <QHash>
#include <QStringList>

class QCheckBox;
class QLabel;
class QLineEdit;
class QTableWidget;
class RealtimeChartDialog;
class ServiceContainer;

/// @brief Workspace plugin for Free Run real-time process data monitoring.
class FreeRunPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    /// Constructs the Free Run plugin, building the entry table and controls.
    /// @param container  Service container for accessing domain services
    /// @param parent     Qt parent object (typically MainWindow)
    explicit FreeRunPlugin(ServiceContainer* container, QObject* parent = nullptr);

    // WorkspacePlugin identity
    QString id() const override;            ///< Returns "freerun"
    QString displayName() const override;   ///< Returns "Free Run"
    QString displayNameZh() const override; ///< Returns "自由运行"
    QIcon icon() const override;            ///< Returns the Free Run theme icon
    QWidget* widget() override;             ///< Returns the root container widget
    int defaultOrder() const override;      ///< Returns 35
    bool visible() const override;          ///< Returns true (always visible)

    // Lifecycle
    void activate() override;                                     ///< Called when user switches to this tab
    void deactivate() override;                                   ///< Called when user switches away
    void onSettingsChanged(const AppSettings& settings) override; ///< Reacts to settings changes
    void onConnectionChanged(bool connected) override;            ///< Reacts to daemon connection state

    // Table access
    QTableWidget* entryTable() const; ///< Returns the process data entry table
    QLineEdit* filter() const;        ///< Returns the text filter input
    QCheckBox* changedOnly() const;   ///< Returns the changed-only filter checkbox
    QLabel* summaryLabel() const;     ///< Returns the summary statistics label
    QLabel* detailLabel() const;      ///< Returns the per-entry detail label

    // UI update surface — MainWindow calls these with pre-computed data.
    /// Populates the entry table with headers and row data.
    /// @param headers Column header strings
    /// @param rows    List of row data (each row is a list of cell strings)
    void setEntryRows(const QStringList& headers, const QList<QStringList>& rows);
    /// Sets the summary label text (e.g. "48 entries, 12 changed").
    /// @param text Summary text to display
    void setSummary(const QString& text);
    /// Sets the detail label text for the selected entry.
    /// @param text Detail text to display
    void setDetail(const QString& text);

    // Entry lookup
    /// Returns the display name for the entry at the given row.
    /// @param row  Row index in the entry table
    /// @return Entry name string, or empty if not cached
    QString entryName(int row) const;
    /// Returns the current value for the entry at the given row.
    /// @param row  Row index in the entry table
    /// @return Entry value string, or empty if not cached
    QString entryValue(int row) const;
    /// Sets the entry name cache for fast lookup by key.
    /// @param names Map of entry key to display name
    void setEntryNames(const QHash<QString, QString>& names);
    /// Sets the entry value cache for fast lookup by key.
    /// @param values Map of entry key to current value
    void setEntryValues(const QHash<QString, QString>& values);

    // Chart management
    /// Registers a chart dialog as currently open.
    /// @param dialog  The RealtimeChartDialog to track
    void addOpenChart(RealtimeChartDialog* dialog);
    /// Unregisters a chart dialog when it is closed.
    /// @param dialog  The RealtimeChartDialog to stop tracking
    void removeOpenChart(RealtimeChartDialog* dialog);
    /// Returns all currently open chart dialogs.
    /// @return Vector of open RealtimeChartDialog pointers
    QVector<RealtimeChartDialog*> openCharts() const;

    // Free Run state
    /// Returns whether Free Run mode is currently enabled.
    /// @return true if Free Run is active
    bool freeRunEnabled() const;
    /// Sets the Free Run enabled state.
    /// @param enabled  true to enable Free Run mode
    void setFreeRunEnabled(bool enabled);
    /// Returns the last received Free Run status string.
    /// @return Status text (e.g. "Running", "Stopped")
    QString lastStatus() const;
    /// Sets the last Free Run status string.
    /// @param status  Status text to store
    void setLastStatus(const QString& status);

signals:
    void chartOpened(RealtimeChartDialog* dialog); ///< Emitted when a new chart dialog is opened
    void chartClosed(RealtimeChartDialog* dialog); ///< Emitted when a chart dialog is closed
    void pollingIntervalChanged(int ms);           ///< Emitted when the polling interval changes

private:
    void buildUi(); ///< Builds the entry table, filter, and detail panel layout

    ServiceContainer* container_;
    QWidget* containerWidget_ = nullptr;
    QTableWidget* entryTable_ = nullptr;
    QLineEdit* filter_ = nullptr;
    QCheckBox* changedOnly_ = nullptr;
    QLabel* summaryLabel_ = nullptr;
    QLabel* detailLabel_ = nullptr;

    QHash<QString, QString> entryNames_;
    QHash<QString, QString> entryValues_;
    QVector<RealtimeChartDialog*> openCharts_;
    bool freeRunEnabled_ = false;
    QString lastStatus_ = "Stopped";
};
