#include "AnalyticsDashboardPlugin.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>

AnalyticsDashboardPlugin::AnalyticsDashboardPlugin(QObject *parent)
    : WorkspacePlugin() {}

QString AnalyticsDashboardPlugin::id() const {
  return QStringLiteral("analytics_dashboard");
}

QString AnalyticsDashboardPlugin::displayName() const {
  return QStringLiteral("Analytics Dashboard");
}

QString AnalyticsDashboardPlugin::displayNameZh() const {
  return QStringLiteral("分析仪表盘");
}

QWidget *AnalyticsDashboardPlugin::widget() {
  if (!containerWidget_)
    buildUi();
  return containerWidget_;
}

int AnalyticsDashboardPlugin::defaultOrder() const { return 92; }

bool AnalyticsDashboardPlugin::visible() const { return false; }

void AnalyticsDashboardPlugin::activate() {}

void AnalyticsDashboardPlugin::deactivate() {}

void AnalyticsDashboardPlugin::addMetric(const AnalyticsMetric &metric) {
  metrics_.append(metric);
  rebuildMetricsTable();
}

void AnalyticsDashboardPlugin::updateMetric(int index, double value) {
  if (index >= 0 && index < metrics_.size()) {
    auto &m = metrics_[index];
    m.value = value;
    m.samples++;
    if (value < m.min)
      m.min = value;
    if (value > m.max)
      m.max = value;
    m.avg = (m.avg * (m.samples - 1) + value) / m.samples;
    emit metricUpdated(m.name, value);
    rebuildMetricsTable();
  }
}

int AnalyticsDashboardPlugin::metricCount() const {
  return metrics_.size();
}

QVector<AnalyticsDashboardPlugin::AnalyticsMetric>
AnalyticsDashboardPlugin::metrics() const {
  return metrics_;
}

void AnalyticsDashboardPlugin::addTrend(const AnalyticsTrend &trend) {
  trends_.append(trend);
  emit trendAdded(trend.metric);
  rebuildTrendsTable();
}

int AnalyticsDashboardPlugin::trendCount() const {
  return trends_.size();
}

QVector<AnalyticsDashboardPlugin::AnalyticsTrend>
AnalyticsDashboardPlugin::trends() const {
  return trends_;
}

void AnalyticsDashboardPlugin::addReport(const AnalyticsReport &report) {
  reports_.append(report);
  emit reportGenerated(report.title);
  rebuildReportsTable();
}

int AnalyticsDashboardPlugin::reportCount() const {
  return reports_.size();
}

QVector<AnalyticsDashboardPlugin::AnalyticsReport>
AnalyticsDashboardPlugin::reports() const {
  return reports_;
}

void AnalyticsDashboardPlugin::addFilter(const AnalyticsFilter &filter) {
  filters_.append(filter);
  rebuildFiltersTable();
}

void AnalyticsDashboardPlugin::removeFilter(int index) {
  if (index >= 0 && index < filters_.size()) {
    filters_.removeAt(index);
    rebuildFiltersTable();
  }
}

void AnalyticsDashboardPlugin::toggleFilter(int index) {
  if (index >= 0 && index < filters_.size()) {
    filters_[index].active = !filters_[index].active;
    emit filterToggled(index, filters_[index].active);
    rebuildFiltersTable();
  }
}

int AnalyticsDashboardPlugin::filterCount() const {
  return filters_.size();
}

QVector<AnalyticsDashboardPlugin::AnalyticsFilter>
AnalyticsDashboardPlugin::filters() const {
  return filters_;
}

QTableWidget *AnalyticsDashboardPlugin::metricsTable() const {
  return metricsTable_;
}

QTableWidget *AnalyticsDashboardPlugin::trendsTable() const {
  return trendsTable_;
}

QTableWidget *AnalyticsDashboardPlugin::reportsTable() const {
  return reportsTable_;
}

QTableWidget *AnalyticsDashboardPlugin::filtersTable() const {
  return filtersTable_;
}

QTextEdit *AnalyticsDashboardPlugin::reportView() const {
  return reportView_;
}

QLabel *AnalyticsDashboardPlugin::statusLabel() const {
  return statusLabel_;
}

QString AnalyticsDashboardPlugin::exportReport() const {
  QString report;
  report += QStringLiteral("# Analytics Report\n\n");
  report += QStringLiteral("## Metrics Summary\n\n");
  for (const auto &m : metrics_) {
    report += QStringLiteral("- %1: %2 (min: %3, max: %4, avg: %5, samples: %6)\n")
                  .arg(m.name)
                  .arg(m.value)
                  .arg(m.min)
                  .arg(m.max)
                  .arg(m.avg)
                  .arg(m.samples);
  }
  report += QStringLiteral("\n## Trends\n\n");
  for (const auto &t : trends_) {
    report += QStringLiteral("- %1: direction=%2, slope=%3, points=%4\n")
                  .arg(t.metric, t.direction)
                  .arg(t.slope)
                  .arg(t.values.size());
  }
  report += QStringLiteral("\n## Active Filters: %1\n\n")
                .arg(std::count_if(filters_.begin(), filters_.end(),
                                   [](const AnalyticsFilter &f) {
                                     return f.active;
                                   }));
  for (const auto &f : filters_) {
    if (f.active) {
      report += QStringLiteral("- %1 %2 %3\n")
                    .arg(f.field, f.operator_, f.value);
    }
  }
  return report;
}

void AnalyticsDashboardPlugin::refresh() {
  rebuildMetricsTable();
  rebuildTrendsTable();
  rebuildReportsTable();
  rebuildFiltersTable();
  rebuildReportView();
  if (statusLabel_)
    statusLabel_->setText(
        QStringLiteral("Last refreshed: %1 | Metrics: %2 | Trends: %3")
            .arg(QDateTime::currentDateTime().toString(Qt::ISODate))
            .arg(metrics_.size())
            .arg(trends_.size()));
}

void AnalyticsDashboardPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);

  statusLabel_ = new QLabel;
  mainLayout->addWidget(statusLabel_);

  tabs_ = new QTabWidget;
  mainLayout->addWidget(tabs_);

  metricsTable_ = new QTableWidget;
  metricsTable_->setColumnCount(6);
  metricsTable_->setHorizontalHeaderLabels(
      {tr("Name"), tr("Value"), tr("Min"), tr("Max"), tr("Avg"), tr("Samples")});
  tabs_->addTab(metricsTable_, tr("Metrics"));

  trendsTable_ = new QTableWidget;
  trendsTable_->setColumnCount(4);
  trendsTable_->setHorizontalHeaderLabels(
      {tr("Metric"), tr("Direction"), tr("Slope"), tr("Points")});
  tabs_->addTab(trendsTable_, tr("Trends"));

  reportsTable_ = new QTableWidget;
  reportsTable_->setColumnCount(3);
  reportsTable_->setHorizontalHeaderLabels(
      {tr("Title"), tr("Generated"), tr("Summary")});
  tabs_->addTab(reportsTable_, tr("Reports"));

  filtersTable_ = new QTableWidget;
  filtersTable_->setColumnCount(4);
  filtersTable_->setHorizontalHeaderLabels(
      {tr("Field"), tr("Operator"), tr("Value"), tr("Active")});
  tabs_->addTab(filtersTable_, tr("Filters"));

  reportView_ = new QTextEdit;
  reportView_->setReadOnly(true);
  tabs_->addTab(reportView_, tr("Report"));

  auto *btnLayout = new QHBoxLayout;
  refreshBtn_ = new QPushButton(tr("Refresh"));
  exportBtn_ = new QPushButton(tr("Export"));
  btnLayout->addWidget(refreshBtn_);
  btnLayout->addWidget(exportBtn_);
  btnLayout->addStretch();
  mainLayout->addLayout(btnLayout);

  connect(refreshBtn_, &QPushButton::clicked, this,
          &AnalyticsDashboardPlugin::refresh);
  connect(exportBtn_, &QPushButton::clicked, this, [this]() {
    if (reportView_)
      reportView_->setText(exportReport());
  });

  refresh();
}

void AnalyticsDashboardPlugin::rebuildMetricsTable() {
  if (!metricsTable_)
    return;
  metricsTable_->setRowCount(metrics_.size());
  for (int i = 0; i < metrics_.size(); ++i) {
    metricsTable_->setItem(i, 0, new QTableWidgetItem(metrics_[i].name));
    metricsTable_->setItem(
        i, 1,
        new QTableWidgetItem(QString::number(metrics_[i].value, 'f', 2)));
    metricsTable_->setItem(
        i, 2,
        new QTableWidgetItem(QString::number(metrics_[i].min, 'f', 2)));
    metricsTable_->setItem(
        i, 3,
        new QTableWidgetItem(QString::number(metrics_[i].max, 'f', 2)));
    metricsTable_->setItem(
        i, 4,
        new QTableWidgetItem(QString::number(metrics_[i].avg, 'f', 2)));
    metricsTable_->setItem(
        i, 5, new QTableWidgetItem(QString::number(metrics_[i].samples)));
  }
}

void AnalyticsDashboardPlugin::rebuildTrendsTable() {
  if (!trendsTable_)
    return;
  trendsTable_->setRowCount(trends_.size());
  for (int i = 0; i < trends_.size(); ++i) {
    trendsTable_->setItem(i, 0, new QTableWidgetItem(trends_[i].metric));
    trendsTable_->setItem(i, 1, new QTableWidgetItem(trends_[i].direction));
    trendsTable_->setItem(
        i, 2,
        new QTableWidgetItem(QString::number(trends_[i].slope, 'f', 4)));
    trendsTable_->setItem(
        i, 3,
        new QTableWidgetItem(QString::number(trends_[i].values.size())));
  }
}

void AnalyticsDashboardPlugin::rebuildReportsTable() {
  if (!reportsTable_)
    return;
  reportsTable_->setRowCount(reports_.size());
  for (int i = 0; i < reports_.size(); ++i) {
    reportsTable_->setItem(i, 0, new QTableWidgetItem(reports_[i].title));
    reportsTable_->setItem(
        i, 1,
        new QTableWidgetItem(
            reports_[i].generated.toString(Qt::ISODate)));
    reportsTable_->setItem(i, 2, new QTableWidgetItem(reports_[i].summary));
  }
}

void AnalyticsDashboardPlugin::rebuildFiltersTable() {
  if (!filtersTable_)
    return;
  filtersTable_->setRowCount(filters_.size());
  for (int i = 0; i < filters_.size(); ++i) {
    filtersTable_->setItem(i, 0, new QTableWidgetItem(filters_[i].field));
    filtersTable_->setItem(
        i, 1, new QTableWidgetItem(filters_[i].operator_));
    filtersTable_->setItem(i, 2, new QTableWidgetItem(filters_[i].value));
    filtersTable_->setItem(
        i, 3,
        new QTableWidgetItem(filters_[i].active ? tr("Yes") : tr("No")));
  }
}

void AnalyticsDashboardPlugin::rebuildReportView() {
  if (reportView_)
    reportView_->setText(exportReport());
}
