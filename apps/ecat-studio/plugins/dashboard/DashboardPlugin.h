#pragma once

// DashboardPlugin — workspace plugin for a configurable real-time dashboard.
//
// Features:
//   - 4 gauge widgets (EcatChartWidget) reserved for backend metrics
//   - 4 counter labels reserved for backend key performance indicators
//   - Manual refresh button for backend-backed updates
//   - Manual refresh button
//   - Grid layout for responsive dashboard arrangement
//
// UI Description:
//   The dashboard presents a grid of gauge charts and counter cards.
//   Each gauge shows a metric slot using EcatChartWidget. Counters display
//   aggregate statistics only when backend evidence is wired. Manual refresh is
//   available but does not synthesize runtime data.
//
// Constructor Pattern: Fine-grained injection (ChartService)
// Default Order: 130 (appears in the right portion of the tab bar)

#include "plugins/WorkspacePlugin.h"

class QGridLayout;
class QSpinBox;
class QLabel;
class QPushButton;
class QTimer;
class ChartService;
class EcatChartWidget;

class DashboardPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit DashboardPlugin(ChartService *service, QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  ChartService *service() const { return service_; }

  int gaugeCount() const;
  int counterCount() const;

public slots:
  void refresh();

private:
  void buildUi();
  void setupDashboard();
  struct GaugeWidget {
    EcatChartWidget *chart;
    QLabel *label;
  };

  ChartService *service_;
  QWidget *container_ = nullptr;
  QWidget *dashboardGrid_ = nullptr;
  QGridLayout *gridLayout_ = nullptr;
  QSpinBox *refreshSpin_ = nullptr;
  QTimer *refreshTimer_ = nullptr;
  QPushButton *refreshBtn_ = nullptr;

  QVector<GaugeWidget> gauges_;
  QVector<QLabel *> counters_;
};
