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

class SignalPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit SignalPlugin(SignalService *service, QObject *parent = nullptr);

  // WorkspacePlugin identity
  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

private slots:
  void showAddChannelDialog();
  void removeSelectedChannel();
  void refreshChart();
  void updateStatsOverlay(int channelId);

private:
  void buildUi();

  SignalService *service_;
  QWidget *container_     = nullptr;
  SignalChartWidget *chart_ = nullptr;
  QListWidget *channelList_ = nullptr;
  QComboBox *windowSizeCombo_ = nullptr;
  QLabel *statsLabel_ = nullptr;
};
