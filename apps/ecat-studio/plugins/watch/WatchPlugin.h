#pragma once

/// @brief Workspace plugin for the SDO Watch list.
///
/// @details The Watch workspace provides real-time monitoring of SDO values
/// across multiple slaves. It supports:
///
///   - **Live value polling**: Periodic refresh of watched SDO values from
///     the daemon, with configurable intervals (500ms, 1s, 2s, 5s).
///   - **Baseline drift detection**: Captures a baseline snapshot and
///     highlights values that have changed since capture.
///   - **Startup SDO comparison**: Compares live values against expected
///     startup SDO values to detect configuration drift.
///   - **Filtering**: Text-based filtering of watch rows by any column.
///   - **CiA 402 presets**: Quick-add of drive-specific watch entries
///     (statusword, actual velocity, actual position, etc.).
///
/// The plugin owns the UI widgets (table, filter, auto-refresh controls)
/// but delegates all data operations to MainWindow's watch workspace
/// partials (MainWindowWatchWorkspace.cpp, MainWindowWatchSync.cpp).
///
/// @par Constructor
///   WatchPlugin(ServiceContainer *container, QObject *parent = nullptr)
///
/// @par Plugin Identity
///   - id: "watch"
///   - defaultOrder: 30
///   - visible: always true
///
/// @par Signals
///   - watchModified(): Emitted when the watch list is modified (add/remove/clear)
///   - filterChanged(text): Emitted when the filter text changes
///
/// @see WorkspacePlugin, MainWindow, WatchService

#include "plugins/WorkspacePlugin.h"

#include <QColor>
#include <QStringList>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QTableWidget;
class QTimer;
class ServiceContainer;

class WatchPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    /// Constructs the Watch plugin, building the filter/refresh UI and table.
    /// @param container  Service container for accessing WatchService, EventBus
    /// @param parent     Qt parent object (typically MainWindow)
    explicit WatchPlugin(ServiceContainer* container, QObject* parent = nullptr);

    // ── WorkspacePlugin Identity ──────────────────────────────────
    QString id() const override;            ///< Returns "watch"
    QString displayName() const override;   ///< Returns "Watch"
    QString displayNameZh() const override; ///< Returns "监视"
    QIcon icon() const override;            ///< Returns "utilities-system-monitor" theme icon
    QWidget* widget() override;             ///< Returns the root container widget
    int defaultOrder() const override;      ///< Returns 30
    bool visible() const override;          ///< Returns true (always visible)

    // ── Lifecycle Hooks ───────────────────────────────────────────
    void activate() override;                                     ///< Called when user switches to this tab
    void deactivate() override;                                   ///< Called when user switches away
    void onSettingsChanged(const AppSettings& settings) override; ///< Reacts to settings changes
    void onConnectionChanged(bool connected) override;            ///< Reacts to daemon connection state

    // ── Widget Accessors ──────────────────────────────────────────
    /// Returns the main watch table (12 columns: Time, Slave, Index, Sub,
    /// Value, Decoded, Type, Mode, Baseline, Delta, Startup, Startup Delta).
    QTableWidget* watchTable() const;
    /// Returns the filter input for text-based row filtering.
    QLineEdit* filterInput() const;
    /// Returns the auto-refresh toggle checkbox.
    QCheckBox* autoRefreshCheckBox() const;
    /// Returns the refresh interval combo box (500ms/1s/2s/5s).
    QComboBox* refreshIntervalCombo() const;
    /// Returns the summary label showing watch statistics.
    QLabel* summaryLabel() const;

    // ── Table Population (called by MainWindow) ───────────────────
    /// Ensures the table has 12 columns with correct headers.
    void ensureWatchTable();
    /// Sets custom column headers (for advanced layouts).
    void setWatchHeaders(const QStringList& headers);
    /// Updates an existing row's cell values.
    void updateWatchRow(int row, const QStringList& columns);
    /// Inserts a new row at the given position.
    void insertWatchRow(int row, const QStringList& columns);
    /// Removes a row from the table.
    void removeWatchRow(int row);
    /// Clears all rows from the watch table.
    void clearWatch();

    // ── Selection ─────────────────────────────────────────────────
    int currentRow() const;          ///< Returns the currently selected row, or -1
    void selectRow(int row);         ///< Selects a specific row
    int rowCount() const;            ///< Returns the number of rows in the table
    bool isRowHidden(int row) const; ///< Checks if a row is hidden by filter
    void resizeColumnsToContents();  ///< Auto-resizes columns to fit content

    // ── Summary ───────────────────────────────────────────────────
    /// Sets the summary label text (e.g. "12 entries, 3 changed").
    void setSummary(const QString& text);

    // ── Color Constants ───────────────────────────────────────────
    /// Color scheme for baseline/startup delta cells.
    /// Changed values use warm yellow tones; unchanged values use green tones.
    /// Light/dark variants support both theme modes.
    static constexpr const char* kChangedLightBg = "#fff7cc";   ///< Changed cell background (light theme)
    static constexpr const char* kChangedLightFg = "#854d0e";   ///< Changed cell foreground (light theme)
    static constexpr const char* kChangedDarkBg = "#3a2f16";    ///< Changed cell background (dark theme)
    static constexpr const char* kChangedDarkFg = "#fde68a";    ///< Changed cell foreground (dark theme)
    static constexpr const char* kUnchangedLightBg = "#dcfce7"; ///< Unchanged cell background (light theme)
    static constexpr const char* kUnchangedLightFg = "#166534"; ///< Unchanged cell foreground (light theme)
    static constexpr const char* kUnchangedDarkBg = "#12351f";  ///< Unchanged cell background (dark theme)
    static constexpr const char* kUnchangedDarkFg = "#86efac";  ///< Unchanged cell foreground (dark theme)

signals:
    /// Emitted when the watch list is modified (add, remove, clear, or refresh).
    void watchModified();
    /// Emitted when the filter text changes, carrying the new filter string.
    void filterChanged(const QString& text);

private:
    /// Builds the UI: filter row (input + auto-refresh + interval + summary) and table.
    void buildUi();

    ServiceContainer* container_;          ///< Service container for domain access
    QWidget* containerWidget_ = nullptr;   ///< Root container widget
    QTableWidget* table_ = nullptr;        ///< Main watch table (12 columns)
    QLineEdit* filter_ = nullptr;          ///< Filter input for row filtering
    QCheckBox* autoRefresh_ = nullptr;     ///< Auto-refresh toggle checkbox
    QComboBox* refreshInterval_ = nullptr; ///< Refresh interval selector (500ms-5s)
    QLabel* summaryLabel_ = nullptr;       ///< Summary label for watch statistics
};
