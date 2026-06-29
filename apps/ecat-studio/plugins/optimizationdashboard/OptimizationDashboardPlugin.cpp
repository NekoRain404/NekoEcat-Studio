#include "OptimizationDashboardPlugin.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>

OptimizationDashboardPlugin::OptimizationDashboardPlugin(QObject *parent)
    : WorkspacePlugin() {}

QString OptimizationDashboardPlugin::id() const {
  return QStringLiteral("optimization_dashboard");
}

QString OptimizationDashboardPlugin::displayName() const {
  return QStringLiteral("Optimization Dashboard");
}

QString OptimizationDashboardPlugin::displayNameZh() const {
  return QStringLiteral("优化仪表盘");
}

QWidget *OptimizationDashboardPlugin::widget() {
  if (!containerWidget_)
    buildUi();
  return containerWidget_;
}

int OptimizationDashboardPlugin::defaultOrder() const { return 90; }

bool OptimizationDashboardPlugin::visible() const { return false; }

void OptimizationDashboardPlugin::activate() {}

void OptimizationDashboardPlugin::deactivate() {}

void OptimizationDashboardPlugin::addMetric(const OptimizationMetric &metric) {
  metrics_.append(metric);
  rebuildMetricsTable();
}

void OptimizationDashboardPlugin::updateMetric(int index, double value) {
  if (index >= 0 && index < metrics_.size()) {
    metrics_[index].value = value;
    metrics_[index].improvement =
        (metrics_[index].target > 0)
            ? ((value - metrics_[index].target) / metrics_[index].target * 100)
            : 0.0;
    emit metricUpdated(metrics_[index].name, value);
    rebuildMetricsTable();
  }
}

int OptimizationDashboardPlugin::metricCount() const {
  return metrics_.size();
}

QVector<OptimizationDashboardPlugin::OptimizationMetric>
OptimizationDashboardPlugin::metrics() const {
  return metrics_;
}

void OptimizationDashboardPlugin::addHistoryEntry(
    const OptimizationHistoryEntry &entry) {
  history_.append(entry);
  rebuildHistoryTable();
}

int OptimizationDashboardPlugin::historyCount() const {
  return history_.size();
}

QVector<OptimizationDashboardPlugin::OptimizationHistoryEntry>
OptimizationDashboardPlugin::history() const {
  return history_;
}

void OptimizationDashboardPlugin::addRecommendation(
    const OptimizationRecommendation &rec) {
  recommendations_.append(rec);
  emit recommendationAdded(rec.title);
  rebuildRecommendationsTable();
}

void OptimizationDashboardPlugin::removeRecommendation(int index) {
  if (index >= 0 && index < recommendations_.size()) {
    recommendations_.removeAt(index);
    rebuildRecommendationsTable();
  }
}

int OptimizationDashboardPlugin::recommendationCount() const {
  return recommendations_.size();
}

QVector<OptimizationDashboardPlugin::OptimizationRecommendation>
OptimizationDashboardPlugin::recommendations() const {
  return recommendations_;
}

void OptimizationDashboardPlugin::addAction(const OptimizationAction &action) {
  actions_.append(action);
  rebuildActionsTable();
}

void OptimizationDashboardPlugin::executeAction(int index) {
  if (index >= 0 && index < actions_.size()) {
    actions_[index].executed = true;
    actions_[index].result = QStringLiteral("Executed successfully");
    emit actionExecuted(actions_[index].name, actions_[index].result);
    rebuildActionsTable();
  }
}

int OptimizationDashboardPlugin::actionCount() const {
  return actions_.size();
}

QVector<OptimizationDashboardPlugin::OptimizationAction>
OptimizationDashboardPlugin::actions() const {
  return actions_;
}

QTableWidget *OptimizationDashboardPlugin::metricsTable() const {
  return metricsTable_;
}

QTableWidget *OptimizationDashboardPlugin::historyTable() const {
  return historyTable_;
}

QTableWidget *OptimizationDashboardPlugin::recommendationsTable() const {
  return recommendationsTable_;
}

QTableWidget *OptimizationDashboardPlugin::actionsTable() const {
  return actionsTable_;
}

QTextEdit *OptimizationDashboardPlugin::reportView() const {
  return reportView_;
}

QLabel *OptimizationDashboardPlugin::statusLabel() const {
  return statusLabel_;
}

QString OptimizationDashboardPlugin::exportReport() const {
  QString report;
  report += QStringLiteral("# Optimization Report\n\n");
  report += QStringLiteral("## Metrics\n\n");
  for (const auto &m : metrics_) {
    report += QStringLiteral("- %1: %2 (target: %3, improvement: %4%)\n")
                  .arg(m.name)
                  .arg(m.value)
                  .arg(m.target)
                  .arg(m.improvement);
  }
  report += QStringLiteral("\n## Recommendations\n\n");
  for (const auto &r : recommendations_) {
    report += QStringLiteral("- [%1] %2: %3\n")
                  .arg(r.priority, r.title, r.description);
  }
  return report;
}

void OptimizationDashboardPlugin::refresh() {
  rebuildMetricsTable();
  rebuildHistoryTable();
  rebuildRecommendationsTable();
  rebuildActionsTable();
  rebuildReportView();
  if (statusLabel_)
    statusLabel_->setText(
        QStringLiteral("Last refreshed: %1")
            .arg(QDateTime::currentDateTime().toString(Qt::ISODate)));
}

void OptimizationDashboardPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);

  statusLabel_ = new QLabel;
  mainLayout->addWidget(statusLabel_);

  tabs_ = new QTabWidget;
  mainLayout->addWidget(tabs_);

  metricsTable_ = new QTableWidget;
  metricsTable_->setColumnCount(4);
  metricsTable_->setHorizontalHeaderLabels(
      {tr("Name"), tr("Value"), tr("Target"), tr("Improvement")});
  tabs_->addTab(metricsTable_, tr("Metrics"));

  historyTable_ = new QTableWidget;
  historyTable_->setColumnCount(4);
  historyTable_->setHorizontalHeaderLabels(
      {tr("Timestamp"), tr("Action"), tr("Result"), tr("Improvement")});
  tabs_->addTab(historyTable_, tr("History"));

  recommendationsTable_ = new QTableWidget;
  recommendationsTable_->setColumnCount(4);
  recommendationsTable_->setHorizontalHeaderLabels(
      {tr("Title"), tr("Description"), tr("Priority"), tr("Category")});
  tabs_->addTab(recommendationsTable_, tr("Recommendations"));

  actionsTable_ = new QTableWidget;
  actionsTable_->setColumnCount(4);
  actionsTable_->setHorizontalHeaderLabels(
      {tr("Name"), tr("Description"), tr("Executed"), tr("Result")});
  tabs_->addTab(actionsTable_, tr("Actions"));

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
          &OptimizationDashboardPlugin::refresh);
  connect(exportBtn_, &QPushButton::clicked, this, [this]() {
    if (reportView_)
      reportView_->setText(exportReport());
  });

  refresh();
}

void OptimizationDashboardPlugin::rebuildMetricsTable() {
  if (!metricsTable_)
    return;
  metricsTable_->setRowCount(metrics_.size());
  for (int i = 0; i < metrics_.size(); ++i) {
    metricsTable_->setItem(
        i, 0, new QTableWidgetItem(metrics_[i].name));
    metricsTable_->setItem(
        i, 1,
        new QTableWidgetItem(QString::number(metrics_[i].value, 'f', 2)));
    metricsTable_->setItem(
        i, 2,
        new QTableWidgetItem(QString::number(metrics_[i].target, 'f', 2)));
    metricsTable_->setItem(
        i, 3,
        new QTableWidgetItem(
            QString::number(metrics_[i].improvement, 'f', 2) + QStringLiteral("%")));
  }
}

void OptimizationDashboardPlugin::rebuildHistoryTable() {
  if (!historyTable_)
    return;
  historyTable_->setRowCount(history_.size());
  for (int i = 0; i < history_.size(); ++i) {
    historyTable_->setItem(
        i, 0,
        new QTableWidgetItem(history_[i].timestamp.toString(Qt::ISODate)));
    historyTable_->setItem(i, 1, new QTableWidgetItem(history_[i].action));
    historyTable_->setItem(i, 2, new QTableWidgetItem(history_[i].result));
    historyTable_->setItem(
        i, 3,
        new QTableWidgetItem(
            QString::number(history_[i].improvement, 'f', 2) + QStringLiteral("%")));
  }
}

void OptimizationDashboardPlugin::rebuildRecommendationsTable() {
  if (!recommendationsTable_)
    return;
  recommendationsTable_->setRowCount(recommendations_.size());
  for (int i = 0; i < recommendations_.size(); ++i) {
    recommendationsTable_->setItem(
        i, 0, new QTableWidgetItem(recommendations_[i].title));
    recommendationsTable_->setItem(
        i, 1, new QTableWidgetItem(recommendations_[i].description));
    recommendationsTable_->setItem(
        i, 2, new QTableWidgetItem(recommendations_[i].priority));
    recommendationsTable_->setItem(
        i, 3, new QTableWidgetItem(recommendations_[i].category));
  }
}

void OptimizationDashboardPlugin::rebuildActionsTable() {
  if (!actionsTable_)
    return;
  actionsTable_->setRowCount(actions_.size());
  for (int i = 0; i < actions_.size(); ++i) {
    actionsTable_->setItem(i, 0, new QTableWidgetItem(actions_[i].name));
    actionsTable_->setItem(
        i, 1, new QTableWidgetItem(actions_[i].description));
    actionsTable_->setItem(
        i, 2,
        new QTableWidgetItem(actions_[i].executed ? tr("Yes") : tr("No")));
    actionsTable_->setItem(i, 3, new QTableWidgetItem(actions_[i].result));
  }
}

void OptimizationDashboardPlugin::rebuildReportView() {
  if (reportView_)
    reportView_->setText(exportReport());
}
