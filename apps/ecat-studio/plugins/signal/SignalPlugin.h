#pragma once

/// @brief Workspace plugin for real-time multi-channel signal analysis.
///
/// @details The Signal Analyzer workspace provides real-time waveform
/// visualization of EtherCAT process data signals. It uses a QPainter-based
/// anti-aliased scrolling chart with a 10,000-point ring buffer per channel.
///
/// Features:
///   - **Multi-channel real-time chart**: SignalChartWidget with scrolling
///     time-domain display and color-coded channels.
///   - **Channel management**: Add/remove signal subscriptions by specifying
///     slave position, index, and subindex.
///   - **Configurable window size**: Adjust the time window for display
///     (e.g. 1s, 5s, 10s, 30s).
///   - **Per-channel statistics**: Min, max, mean, standard deviation
///     overlay for the selected channel.
///   - **Signal integration**: Uses SignalService for data acquisition
///     and EventBus::signalData for real-time updates.
///
/// @par Constructor
///   SignalPlugin(SignalService *service, QObject *parent = nullptr)
///   Uses fine-grained injection pattern.
///
/// @par Plugin Identity
///   - id: "signal"
///   - defaultOrder: 67 (after AlEvent, before Consistency)
///   - visible: always true
///
/// @see WorkspacePlugin, SignalService, SignalChartWidget, EventBus

#include "plugins/WorkspacePlugin.h"

class QListWidget;
class QComboBox;
class QLabel;
class SignalChartWidget;
class SignalService;

/// @brief Workspace plugin for real-time multi-channel signal analysis.
class SignalPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    /// Constructs the Signal Analyzer plugin with fine-grained service injection.
    /// @param service  SignalService for signal data acquisition
    /// @param parent   Qt parent object (typically MainWindow)
    explicit SignalPlugin(SignalService* service, QObject* parent = nullptr);

    // WorkspacePlugin identity
    QString id() const override;            ///< Returns "signal"
    QString displayName() const override;   ///< Returns "Signal Analyzer"
    QString displayNameZh() const override; ///< Returns "信号分析"
    QWidget* widget() override;             ///< Returns the root container widget
    int defaultOrder() const override;      ///< Returns 67
    bool visible() const override;          ///< Returns true (always visible)

private slots:
    void showAddChannelDialog();            ///< Opens the add-channel dialog for signal subscription
    void removeSelectedChannel();           ///< Removes the selected channel from the chart
    void refreshChart();                    ///< Refreshes the chart with latest signal data
    void updateStatsOverlay(int channelId); ///< Updates statistics overlay for the given channel

private:
    void buildUi(); ///< Builds the chart, channel list, window size combo, and stats layout

    SignalService* service_;
    QWidget* container_ = nullptr;
    SignalChartWidget* chart_ = nullptr;
    QListWidget* channelList_ = nullptr;
    QComboBox* windowSizeCombo_ = nullptr;
    QLabel* statsLabel_ = nullptr;
};
