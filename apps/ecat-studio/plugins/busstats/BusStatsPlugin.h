#pragma once

/// @brief Workspace plugin for bus statistics dashboard.
///
/// @details The Bus Statistics workspace provides real-time monitoring of
/// EtherCAT bus performance metrics. It displays gauges and counters for
/// frame counts, error counts, bandwidth utilization, CRC errors, and
/// lost frames. The plugin polls BusStatsService every second for updates.
///
/// Features:
///   - **Real-time statistics**: Frame counts, error counts, bandwidth
///   - **Performance metrics**: CRC errors, lost frames, frame rate
///   - **Start/Stop control**: Toggle statistics collection
///   - **Status display**: Current collection state and refresh rate
///   - **Table view**: Detailed statistics in tabular format
///
/// @par Constructor
///   BusStatsPlugin(BusStatsService *service, QObject *parent = nullptr)
///   Uses fine-grained injection pattern.
///
/// @par Plugin Identity
///   - id: "busstats"
///   - defaultOrder: 95
///   - visible: always true
///
/// @par Usage Example
///   @code
///   // In MainWindow constructor:
///   auto *busStatsService = new BusStatsService(&client_, this);
///   pluginRegistry_->registerPlugin(new BusStatsPlugin(busStatsService, this));
///
///   // Start statistics collection:
///   busStatsPlugin->startStopBtn_->click();
///   @endcode
///
/// @see WorkspacePlugin, BusStatsService, PluginRegistry

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QPushButton;
class QTableWidget;
class QTimer;
class BusStatsService;

/// @brief Workspace plugin for bus statistics dashboard.
///
/// @details This plugin provides a complete bus statistics monitoring interface with:
///   - Real-time frame count and error count display
///   - Bandwidth utilization monitoring
///   - CRC error and lost frame tracking
///   - Start/Stop control for statistics collection
///   - Status display with refresh rate information
///
/// The plugin communicates with BusStatsService for data collection and uses
/// a QTimer for periodic UI updates.
class BusStatsPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    /// @brief Constructs the Bus Statistics plugin with fine-grained service injection.
    /// @param service  BusStatsService instance for statistics data collection
    /// @param parent   Qt parent object (typically MainWindow)
    explicit BusStatsPlugin(BusStatsService* service, QObject* parent = nullptr);

    // ── WorkspacePlugin Identity ──────────────────────────────────
    QString id() const override;            ///< Returns "busstats"
    QString displayName() const override;   ///< Returns "Bus Statistics"
    QString displayNameZh() const override; ///< Returns "总线统计"
    QIcon icon() const override;            ///< Returns "utilities-system-monitor" theme icon
    QWidget* widget() override;             ///< Returns the root container widget
    int defaultOrder() const override;      ///< Returns 95
    bool visible() const override;          ///< Returns true (always visible)

    // ── Lifecycle Hooks ───────────────────────────────────────────
    void activate() override;   ///< Called when user switches to this tab
    void deactivate() override; ///< Called when user switches away

    // ── Accessors ─────────────────────────────────────────────────
    BusStatsService* service() const { return service_; }    ///< Returns the BusStats service instance
    QTableWidget* statsTable() const { return statsTable_; } ///< Returns the statistics table widget

private:
    /// @brief Builds the UI layout with statistics table and control buttons.
    void buildUi();

    /// @brief Updates the display with current statistics from BusStatsService.
    /// @details Called periodically by uiTimer_ to refresh frame counts,
    /// error counts, bandwidth, and other metrics.
    void updateDisplay();

    BusStatsService* service_;            ///< BusStats service for data collection
    QWidget* containerWidget_ = nullptr;  ///< Root container widget
    QTableWidget* statsTable_ = nullptr;  ///< Statistics table widget
    QPushButton* startStopBtn_ = nullptr; ///< Start/Stop button for statistics collection
    QLabel* statusLabel_ = nullptr;       ///< Status label showing collection state
    QLabel* frameRateLabel_ = nullptr;    ///< Frame rate display label
    QLabel* bandwidthLabel_ = nullptr;    ///< Bandwidth utilization display label
    QTimer* uiTimer_ = nullptr;           ///< Timer for periodic UI updates (1s interval)
};
