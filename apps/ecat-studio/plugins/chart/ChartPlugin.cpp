// ChartPlugin — implementation.  See header for interface documentation.
#include "ChartPlugin.h"
#include "EcatChartWidget.h"
#include "services/ChartService.h"

#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QMessageBox>

ChartPlugin::ChartPlugin(ChartService *service, QObject *parent)
    : service_(service) {
  if (parent) setParent(parent);
  buildUi();
  loadSampleData();

  connect(service_, &ChartService::chartUpdated, this, [this](int id) {
    if (id == currentChartId_) refreshChart();
  });
}

QString ChartPlugin::id() const { return "chart"; }
QString ChartPlugin::displayName() const { return "Charts"; }
QString ChartPlugin::displayNameZh() const { return QStringLiteral("图表"); }
int ChartPlugin::defaultOrder() const { return 125; }
bool ChartPlugin::visible() const { return true; }
QWidget *ChartPlugin::widget() { return container_; }

// ── UI construction ───────────────────────────────────────────────────

void ChartPlugin::buildUi() {
  container_ = new QWidget;
  auto *root = new QHBoxLayout(container_);
  root->setContentsMargins(0, 0, 0, 0);
  root->setSpacing(0);

  // ── Left panel: controls ──
  auto *leftPanel = new QWidget;
  leftPanel->setFixedWidth(200);
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(4, 4, 4, 4);

  auto *typeGroup = new QGroupBox(tr("Chart Type"));
  auto *typeLayout = new QVBoxLayout(typeGroup);
  chartTypeCombo_ = new QComboBox;
  chartTypeCombo_->addItems({tr("Line"), tr("Bar"), tr("Pie"), tr("Scatter"), tr("Gauge")});
  typeLayout->addWidget(chartTypeCombo_);
  leftLayout->addWidget(typeGroup);

  auto *srcGroup = new QGroupBox(tr("Data Source"));
  auto *srcLayout = new QVBoxLayout(srcGroup);
  dataSourceCombo_ = new QComboBox;
  dataSourceCombo_->addItems({tr("Bus Statistics"), tr("Signal Data"), tr("Custom")});
  srcLayout->addWidget(dataSourceCombo_);
  leftLayout->addWidget(srcGroup);

  exportBtn_ = new QPushButton(tr("Export"));
  leftLayout->addWidget(exportBtn_);

  leftLayout->addStretch();
  root->addWidget(leftPanel);

  // ── Center: chart display ──
  chart_ = new EcatChartWidget;
  root->addWidget(chart_, 1);

  // ── Connections ──
  connect(chartTypeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &ChartPlugin::onChartTypeChanged);
  connect(dataSourceCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &ChartPlugin::onDataSourceChanged);
  connect(exportBtn_, &QPushButton::clicked, this, &ChartPlugin::exportChart);
}

void ChartPlugin::loadSampleData() {
  onChartTypeChanged(0);
}

// ── Slots ─────────────────────────────────────────────────────────────

void ChartPlugin::onChartTypeChanged(int index) {
  static const EcatChartWidget::ChartType types[] = {
      EcatChartWidget::Line, EcatChartWidget::Bar, EcatChartWidget::Pie,
      EcatChartWidget::Scatter, EcatChartWidget::Gauge};
  if (index < 0 || index >= 5) return;

  chart_->setChartType(types[index]);

  if (currentChartId_ > 0) {
    service_->removeChart(currentChartId_);
    currentChartId_ = -1;
  }

  ChartData data;
  if (index == 0) { // Line
    data.labels = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
    ChartDataset ds1;
    ds1.name = "TX Frames";
    ds1.values = {10, 25, 40, 35, 50, 65, 60, 75, 80, 90};
    data.datasets.append(ds1);
    ChartDataset ds2;
    ds2.name = "RX Frames";
    ds2.values = {8, 22, 38, 33, 48, 62, 58, 72, 78, 88};
    data.datasets.append(ds2);
  } else if (index == 1) { // Bar
    data.labels = {"Slave 0", "Slave 1", "Slave 2", "Slave 3"};
    ChartDataset ds;
    ds.name = "Latency (us)";
    ds.values = {120, 85, 200, 150};
    data.datasets.append(ds);
  } else if (index == 2) { // Pie
    data.labels = {"OP", "PRE-OP", "SAFE-OP", "INIT"};
    ChartDataset ds;
    ds.name = "States";
    ds.values = {45, 20, 15, 20};
    data.datasets.append(ds);
  } else if (index == 3) { // Scatter
    ChartDataset ds;
    ds.name = "Load vs Temp";
    ds.values = {10, 25, 20, 35, 30, 50, 40, 60, 50, 75};
    data.datasets.append(ds);
  } else { // Gauge
    chart_->setGaugeValue(72.5, 0, 100);
    chart_->setTitle(tr("CPU Load (%)"));
    return;
  }

  currentChartId_ = service_->createChart(
      chartTypeCombo_->currentText(), tr("Sample Chart"), data);
  chart_->setTitle(tr("Sample Chart"));
  chart_->setLabels(data.labels);
  chart_->setDatasets(data.datasets);
}

void ChartPlugin::onDataSourceChanged(int index) {
  Q_UNUSED(index);
  onChartTypeChanged(chartTypeCombo_->currentIndex());
}

void ChartPlugin::exportChart() {
  if (currentChartId_ <= 0) {
    QMessageBox::information(container_, tr("Export"), tr("No chart to export."));
    return;
  }
  const QString path = QFileDialog::getSaveFileName(
      container_, tr("Export Chart"), QString(), tr("Images (*.png *.jpg)"));
  if (path.isEmpty()) return;

  if (service_->exportChart(currentChartId_, path)) {
    QMessageBox::information(container_, tr("Export"),
                             tr("Chart exported to %1").arg(path));
  }
}

void ChartPlugin::refreshChart() {
  if (currentChartId_ <= 0) return;
  const ChartData data = service_->chartData(currentChartId_);
  chart_->setLabels(data.labels);
  chart_->setDatasets(data.datasets);
}
