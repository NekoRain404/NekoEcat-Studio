#pragma once

// ChartPlugin — workspace plugin for data visualization and charting.
//
// Features:
//   - 5 chart types: Line, Bar, Pie, Scatter, Gauge
//   - Data source selection from EtherCAT metrics (bandwidth, latency, etc.)
//   - Export chart as image (PNG/SVG)
//   - Real-time data refresh
//   - Interactive chart with zoom and pan
//
// UI Description:
//   The chart workspace provides a stacked widget with chart type selector
//   (combo box) and data source selector. Each chart type renders differently:
//   Line/Bar for time-series, Pie for distribution, Scatter for correlation,
//   Gauge for single-value metrics. Export button saves the current chart.
//
// Constructor Pattern: Fine-grained injection (ChartService)
// Default Order: 125

#include "plugins/WorkspacePlugin.h"

class QComboBox;
class QPushButton;
class QStackedWidget;
class QLabel;
class EcatChartWidget;
class ChartService;

class ChartPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit ChartPlugin(ChartService* service, QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    ChartService* service() const { return service_; }

private slots:
    void onChartTypeChanged(int index);
    void onDataSourceChanged(int index);
    void exportChart();
    void refreshChart();

private:
    void buildUi();
    void loadSampleData();

    ChartService* service_;
    QWidget* container_ = nullptr;
    EcatChartWidget* chart_ = nullptr;
    QComboBox* chartTypeCombo_ = nullptr;
    QComboBox* dataSourceCombo_ = nullptr;
    QPushButton* exportBtn_ = nullptr;
    int currentChartId_ = -1;
};
