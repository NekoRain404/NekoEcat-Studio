#pragma once

// SignalPlugin — workspace plugin for the Signal Analyzer.
// Displays a real-time scrolling multi-channel chart with channel management
// controls and per-channel statistics overlay.

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
