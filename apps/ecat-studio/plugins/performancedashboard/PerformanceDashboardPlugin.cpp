// PerformanceDashboardPlugin — implementation. See header for interface documentation.
#include "PerformanceDashboardPlugin.h"
#include "services/PerformanceMonitorService.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>
#include <QHeaderView>
#include <QGroupBox>
#include <QFrame>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

PerformanceDashboardPlugin::PerformanceDashboardPlugin(
    PerformanceMonitorService *service, QObject *parent)
    : service_(service) {
  if (parent) setParent(parent);
  buildUi();

  refreshTimer_ = new QTimer(this);
  connect(refreshTimer_, &QTimer::timeout, this, &PerformanceDashboardPlugin::refresh);
  refreshTimer_->start(1000);

  connect(service_, &PerformanceMonitorService::performanceAlert,
          this, [this](const QString &category, const QString &message,
                       double value, double threshold) {
    if (alertsTable_) {
      int row = alertsTable_->rowCount();
      alertsTable_->insertRow(row);
      alertsTable_->setItem(row, 0, new QTableWidgetItem(
          QDateTime::currentDateTime().toString("hh:mm:ss")));
      alertsTable_->setItem(row, 1, new QTableWidgetItem(category));
      alertsTable_->setItem(row, 2, new QTableWidgetItem(message));
      alertsTable_->setItem(row, 3, new QTableWidgetItem(
          QString::number(value, 'f', 2)));
      alertsTable_->setItem(row, 4, new QTableWidgetItem(
          QString::number(threshold, 'f', 2)));
      alertsTable_->scrollToBottom();
    }
  });
}

QString PerformanceDashboardPlugin::id() const { return "performancedashboard"; }
QString PerformanceDashboardPlugin::displayName() const { return "Performance"; }
QString PerformanceDashboardPlugin::displayNameZh() const {
  return QStringLiteral("性能监控");
}
int PerformanceDashboardPlugin::defaultOrder() const { return 135; }
bool PerformanceDashboardPlugin::visible() const { return false; }
QWidget *PerformanceDashboardPlugin::widget() { return container_; }

void PerformanceDashboardPlugin::activate() {
  refresh();
  refreshTimer_->start(1000);
}

void PerformanceDashboardPlugin::deactivate() {
  refreshTimer_->stop();
}

// ── UI Construction ──────────────────────────────────────────────────

void PerformanceDashboardPlugin::buildUi() {
  container_ = new QWidget;
  auto *layout = new QVBoxLayout(container_);
  layout->setContentsMargins(4, 4, 4, 4);

  tabs_ = new QTabWidget;
  layout->addWidget(tabs_);

  buildOverviewTab();
  buildStartupTab();
  buildRuntimeTab();
  buildMemoryTab();
  buildHistoryTab();
  buildReportsTab();
}

void PerformanceDashboardPlugin::buildOverviewTab() {
  auto *tab = new QWidget;
  auto *layout = new QVBoxLayout(tab);

  // Status row
  auto *statusGroup = new QGroupBox(tr("Status"));
  auto *statusLayout = new QHBoxLayout(statusGroup);

  startupStatus_ = new QLabel(tr("Startup: In Progress"));
  startupStatus_->setStyleSheet("font-weight: bold; font-size: 14px;");
  statusLayout->addWidget(startupStatus_);

  totalStartupTime_ = new QLabel(tr("Total: --"));
  totalStartupTime_->setStyleSheet("font-size: 14px;");
  statusLayout->addWidget(totalStartupTime_);

  statusLayout->addStretch();
  layout->addWidget(statusGroup);

  // Metrics grid
  auto *metricsGroup = new QGroupBox(tr("Key Metrics"));
  auto *metricsLayout = new QGridLayout(metricsGroup);

  auto addMetric = [&](int row, int col, const QString &name) -> QLabel * {
    auto *frame = new QFrame;
    frame->setFrameStyle(QFrame::StyledPanel);
    auto *fl = new QVBoxLayout(frame);
    fl->setContentsMargins(8, 8, 8, 8);

    auto *nameLbl = new QLabel(name);
    nameLbl->setAlignment(Qt::AlignCenter);
    fl->addWidget(nameLbl);

    auto *valLbl = new QLabel("--");
    valLbl->setAlignment(Qt::AlignCenter);
    QFont f = valLbl->font();
    f.setPointSize(f.pointSize() + 4);
    f.setBold(true);
    valLbl->setFont(f);
    fl->addWidget(valLbl);

    metricsLayout->addWidget(frame, row, col);
    return valLbl;
  };

  sdoReadLatencyLabel_ = addMetric(0, 0, tr("SDO Read (ms)"));
  sdoWriteLatencyLabel_ = addMetric(0, 1, tr("SDO Write (ms)"));
  stateTransitionLabel_ = addMetric(0, 2, tr("State Trans (ms)"));
  freeRunCycleLabel_ = addMetric(1, 0, tr("Free Run (us)"));
  uiUpdateLabel_ = addMetric(1, 1, tr("UI Update (ms)"));
  memoryUsageLabel_ = addMetric(1, 2, tr("Memory (MB)"));

  layout->addWidget(metricsGroup);

  // Alerts table
  auto *alertsGroup = new QGroupBox(tr("Recent Alerts"));
  auto *alertsLayout = new QVBoxLayout(alertsGroup);

  alertsTable_ = new QTableWidget;
  alertsTable_->setColumnCount(5);
  alertsTable_->setHorizontalHeaderLabels(
      {tr("Time"), tr("Category"), tr("Message"), tr("Value"), tr("Threshold")});
  alertsTable_->horizontalHeader()->setStretchLastSection(true);
  alertsTable_->setEditTriggers(QTableWidget::NoEditTriggers);
  alertsTable_->setSelectionBehavior(QTableWidget::SelectRows);
  alertsLayout->addWidget(alertsTable_);

  layout->addWidget(alertsGroup);

  tabs_->addTab(tab, tr("Overview"));
}

void PerformanceDashboardPlugin::buildStartupTab() {
  auto *tab = new QWidget;
  auto *layout = new QVBoxLayout(tab);

  startupTotalLabel_ = new QLabel(tr("Total Startup Time: --"));
  startupTotalLabel_->setStyleSheet("font-weight: bold; font-size: 16px;");
  layout->addWidget(startupTotalLabel_);

  startupPhasesTable_ = new QTableWidget;
  startupPhasesTable_->setColumnCount(3);
  startupPhasesTable_->setHorizontalHeaderLabels(
      {tr("Phase"), tr("Duration (ms)"), tr("Percentage")});
  startupPhasesTable_->horizontalHeader()->setStretchLastSection(true);
  startupPhasesTable_->setEditTriggers(QTableWidget::NoEditTriggers);
  startupPhasesTable_->setSelectionBehavior(QTableWidget::SelectRows);
  layout->addWidget(startupPhasesTable_);

  tabs_->addTab(tab, tr("Startup"));
}

void PerformanceDashboardPlugin::buildRuntimeTab() {
  auto *tab = new QWidget;
  auto *layout = new QVBoxLayout(tab);

  runtimeTable_ = new QTableWidget;
  runtimeTable_->setColumnCount(5);
  runtimeTable_->setHorizontalHeaderLabels(
      {tr("Metric"), tr("Current"), tr("Average"), tr("Min"), tr("Max")});
  runtimeTable_->horizontalHeader()->setStretchLastSection(true);
  runtimeTable_->setEditTriggers(QTableWidget::NoEditTriggers);
  runtimeTable_->setSelectionBehavior(QTableWidget::SelectRows);
  layout->addWidget(runtimeTable_);

  tabs_->addTab(tab, tr("Runtime"));
}

void PerformanceDashboardPlugin::buildMemoryTab() {
  auto *tab = new QWidget;
  auto *layout = new QVBoxLayout(tab);

  totalMemoryLabel_ = new QLabel(tr("Total Memory: --"));
  totalMemoryLabel_->setStyleSheet("font-weight: bold; font-size: 14px;");
  layout->addWidget(totalMemoryLabel_);

  auto *tablesLayout = new QHBoxLayout;

  auto *serviceGroup = new QGroupBox(tr("Services"));
  auto *serviceLayout = new QVBoxLayout(serviceGroup);
  serviceMemoryTable_ = new QTableWidget;
  serviceMemoryTable_->setColumnCount(2);
  serviceMemoryTable_->setHorizontalHeaderLabels({tr("Service"), tr("Bytes")});
  serviceMemoryTable_->horizontalHeader()->setStretchLastSection(true);
  serviceMemoryTable_->setEditTriggers(QTableWidget::NoEditTriggers);
  serviceLayout->addWidget(serviceMemoryTable_);
  tablesLayout->addWidget(serviceGroup);

  auto *pluginGroup = new QGroupBox(tr("Plugins"));
  auto *pluginLayout = new QVBoxLayout(pluginGroup);
  pluginMemoryTable_ = new QTableWidget;
  pluginMemoryTable_->setColumnCount(2);
  pluginMemoryTable_->setHorizontalHeaderLabels({tr("Plugin"), tr("Bytes")});
  pluginMemoryTable_->horizontalHeader()->setStretchLastSection(true);
  pluginMemoryTable_->setEditTriggers(QTableWidget::NoEditTriggers);
  pluginLayout->addWidget(pluginMemoryTable_);
  tablesLayout->addWidget(pluginGroup);

  auto *cacheGroup = new QGroupBox(tr("Caches"));
  auto *cacheLayout = new QVBoxLayout(cacheGroup);
  cacheMemoryTable_ = new QTableWidget;
  cacheMemoryTable_->setColumnCount(2);
  cacheMemoryTable_->setHorizontalHeaderLabels({tr("Cache"), tr("Bytes")});
  cacheMemoryTable_->horizontalHeader()->setStretchLastSection(true);
  cacheMemoryTable_->setEditTriggers(QTableWidget::NoEditTriggers);
  cacheLayout->addWidget(cacheMemoryTable_);
  tablesLayout->addWidget(cacheGroup);

  layout->addLayout(tablesLayout);

  tabs_->addTab(tab, tr("Memory"));
}

void PerformanceDashboardPlugin::buildHistoryTab() {
  auto *tab = new QWidget;
  auto *layout = new QVBoxLayout(tab);

  historyTable_ = new QTableWidget;
  historyTable_->setColumnCount(8);
  historyTable_->setHorizontalHeaderLabels(
      {tr("Timestamp"), tr("SDO Read"), tr("SDO Write"),
       tr("State Trans"), tr("Free Run"), tr("UI Update"),
       tr("Memory (MB)"), tr("Cycle Time")});
  historyTable_->horizontalHeader()->setStretchLastSection(true);
  historyTable_->setEditTriggers(QTableWidget::NoEditTriggers);
  historyTable_->setSelectionBehavior(QTableWidget::SelectRows);
  layout->addWidget(historyTable_);

  auto *btnLayout = new QHBoxLayout;
  auto *clearBtn = new QPushButton(tr("Clear History"));
  connect(clearBtn, &QPushButton::clicked, this, [this]() {
    if (historyTable_) historyTable_->setRowCount(0);
  });
  btnLayout->addWidget(clearBtn);
  btnLayout->addStretch();
  layout->addLayout(btnLayout);

  tabs_->addTab(tab, tr("History"));
}

void PerformanceDashboardPlugin::buildReportsTab() {
  auto *tab = new QWidget;
  auto *layout = new QVBoxLayout(tab);

  auto *btnLayout = new QHBoxLayout;
  auto *generateBtn = new QPushButton(tr("Generate Report"));
  connect(generateBtn, &QPushButton::clicked, this,
          &PerformanceDashboardPlugin::generateReport);
  btnLayout->addWidget(generateBtn);

  auto *exportBtn = new QPushButton(tr("Export to File"));
  connect(exportBtn, &QPushButton::clicked, this, [this]() {
    QString fileName = QFileDialog::getSaveFileName(
        container_, tr("Export Performance Report"), "",
        tr("JSON Files (*.json);;All Files (*)"));
    if (!fileName.isEmpty()) {
      QJsonObject report = service_->performanceReport();
      QFile file(fileName);
      if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(report).toJson());
      }
    }
  });
  btnLayout->addWidget(exportBtn);
  btnLayout->addStretch();
  layout->addLayout(btnLayout);

  reportLabel_ = new QLabel(tr("Click 'Generate Report' to view performance report."));
  reportLabel_->setWordWrap(true);
  reportLabel_->setAlignment(Qt::AlignTop);
  layout->addWidget(reportLabel_);

  tabs_->addTab(tab, tr("Reports"));
}

// ── Refresh Logic ────────────────────────────────────────────────────

void PerformanceDashboardPlugin::refresh() {
  updateOverview();
  updateStartup();
  updateRuntime();
  updateMemory();
  updateHistory();
}

void PerformanceDashboardPlugin::updateOverview() {
  QJsonObject metrics = service_->currentMetrics();

  // Startup status
  if (service_->startupComplete()) {
    startupStatus_->setText(tr("Startup: Complete"));
    startupStatus_->setStyleSheet("font-weight: bold; font-size: 14px; color: green;");
    QJsonObject startup = service_->startupReport();
    totalStartupTime_->setText(
        tr("Total: %1 ms").arg(startup["totalMs"].toDouble(), 0, 'f', 1));
  } else {
    startupStatus_->setText(tr("Startup: In Progress"));
    startupStatus_->setStyleSheet("font-weight: bold; font-size: 14px; color: orange;");
    totalStartupTime_->setText(tr("Total: --"));
  }

  // Key metrics
  sdoReadLatencyLabel_->setText(
      QString::number(metrics["sdoReadLatencyMs"].toDouble(), 'f', 2));
  sdoWriteLatencyLabel_->setText(
      QString::number(metrics["sdoWriteLatencyMs"].toDouble(), 'f', 2));
  stateTransitionLabel_->setText(
      QString::number(metrics["stateTransitionMs"].toDouble(), 'f', 2));
  freeRunCycleLabel_->setText(
      QString::number(metrics["freeRunCycleUs"].toDouble(), 'f', 1));
  uiUpdateLabel_->setText(
      QString::number(metrics["uiUpdateMs"].toDouble(), 'f', 2));
  memoryUsageLabel_->setText(
      QString::number(metrics["memoryMB"].toDouble(), 'f', 1));

  // Color code based on thresholds
  auto thresholds = service_->alertThresholds();
  auto colorLabel = [](QLabel *label, double value, double threshold) {
    if (value > threshold)
      label->setStyleSheet("color: red; font-weight: bold;");
    else if (value > threshold * 0.8)
      label->setStyleSheet("color: orange; font-weight: bold;");
    else
      label->setStyleSheet("color: green; font-weight: bold;");
  };

  colorLabel(sdoReadLatencyLabel_, metrics["sdoReadLatencyMs"].toDouble(),
             thresholds.sdoLatencyMs);
  colorLabel(sdoWriteLatencyLabel_, metrics["sdoWriteLatencyMs"].toDouble(),
             thresholds.sdoLatencyMs);
  colorLabel(stateTransitionLabel_, metrics["stateTransitionMs"].toDouble(),
             thresholds.stateTransitionMs);
  colorLabel(freeRunCycleLabel_, metrics["freeRunCycleUs"].toDouble(),
             thresholds.freeRunCycleUs);
  colorLabel(uiUpdateLabel_, metrics["uiUpdateMs"].toDouble(),
             thresholds.uiUpdateMs);
  colorLabel(memoryUsageLabel_, metrics["memoryMB"].toDouble(),
             thresholds.memoryMB);
}

void PerformanceDashboardPlugin::updateStartup() {
  QJsonObject startup = service_->startupReport();

  if (startup["complete"].toBool()) {
    startupTotalLabel_->setText(
        tr("Total Startup Time: %1 ms").arg(startup["totalMs"].toDouble(), 0, 'f', 1));
  } else {
    startupTotalLabel_->setText(tr("Total Startup Time: In Progress..."));
  }

  QJsonObject phases = startup["phases"].toObject();
  QJsonObject percentages = startup["percentages"].toObject();

  startupPhasesTable_->setRowCount(phases.size());
  int row = 0;
  for (auto it = phases.constBegin(); it != phases.constEnd(); ++it) {
    startupPhasesTable_->setItem(row, 0, new QTableWidgetItem(it.key()));
    startupPhasesTable_->setItem(row, 1, new QTableWidgetItem(
        QString::number(it.value().toDouble(), 'f', 2)));
    double pct = percentages[it.key()].toDouble();
    startupPhasesTable_->setItem(row, 2, new QTableWidgetItem(
        QString::number(pct, 'f', 1) + "%"));
    ++row;
  }
}

void PerformanceDashboardPlugin::updateRuntime() {
  QJsonObject report = service_->performanceReport();
  QJsonObject stats = report["statistics"].toObject();

  runtimeTable_->setRowCount(5);
  int row = 0;

  auto setRow = [&](const QString &name, const QJsonObject &s) {
    runtimeTable_->setItem(row, 0, new QTableWidgetItem(name));
    runtimeTable_->setItem(row, 1, new QTableWidgetItem(
        QString::number(s["avg"].toDouble(), 'f', 2)));
    runtimeTable_->setItem(row, 2, new QTableWidgetItem(
        QString::number(s["avg"].toDouble(), 'f', 2)));
    runtimeTable_->setItem(row, 3, new QTableWidgetItem(
        QString::number(s["min"].toDouble(), 'f', 2)));
    runtimeTable_->setItem(row, 4, new QTableWidgetItem(
        QString::number(s["max"].toDouble(), 'f', 2)));
    ++row;
  };

  setRow(tr("SDO Read Latency (ms)"), stats["sdoReadLatency"].toObject());
  setRow(tr("SDO Write Latency (ms)"), stats["sdoWriteLatency"].toObject());
  setRow(tr("State Transition (ms)"), stats["stateTransition"].toObject());
  setRow(tr("Free Run Cycle (us)"), stats["freeRunCycleTime"].toObject());
  setRow(tr("UI Update (ms)"), stats["uiUpdateTime"].toObject());
}

void PerformanceDashboardPlugin::updateMemory() {
  QJsonObject memory = service_->memoryReport();

  totalMemoryLabel_->setText(
      tr("Total Memory: %1 MB").arg(memory["totalMB"].toDouble(), 0, 'f', 2));

  // Service memory
  QJsonObject services = memory["services"].toObject();
  serviceMemoryTable_->setRowCount(services.size());
  int row = 0;
  for (auto it = services.constBegin(); it != services.constEnd(); ++it) {
    serviceMemoryTable_->setItem(row, 0, new QTableWidgetItem(it.key()));
    serviceMemoryTable_->setItem(row, 1, new QTableWidgetItem(
        QString::number(it.value().toDouble(), 'f', 0)));
    ++row;
  }

  // Plugin memory
  QJsonObject plugins = memory["plugins"].toObject();
  pluginMemoryTable_->setRowCount(plugins.size());
  row = 0;
  for (auto it = plugins.constBegin(); it != plugins.constEnd(); ++it) {
    pluginMemoryTable_->setItem(row, 0, new QTableWidgetItem(it.key()));
    pluginMemoryTable_->setItem(row, 1, new QTableWidgetItem(
        QString::number(it.value().toDouble(), 'f', 0)));
    ++row;
  }

  // Cache memory
  QJsonObject caches = memory["caches"].toObject();
  cacheMemoryTable_->setRowCount(caches.size());
  row = 0;
  for (auto it = caches.constBegin(); it != caches.constEnd(); ++it) {
    cacheMemoryTable_->setItem(row, 0, new QTableWidgetItem(it.key()));
    cacheMemoryTable_->setItem(row, 1, new QTableWidgetItem(
        QString::number(it.value().toDouble(), 'f', 0)));
    ++row;
  }
}

void PerformanceDashboardPlugin::updateHistory() {
  QVector<QJsonObject> history = service_->history();
  if (history.isEmpty()) return;

  // Only show last 100 entries
  int start = qMax(0, history.size() - 100);
  historyTable_->setRowCount(history.size() - start);

  for (int i = start; i < history.size(); ++i) {
    const QJsonObject &entry = history[i];
    int row = i - start;

    qint64 ts = entry["timestamp"].toVariant().toLongLong();
    historyTable_->setItem(row, 0, new QTableWidgetItem(
        QDateTime::fromMSecsSinceEpoch(ts).toString("hh:mm:ss")));
    historyTable_->setItem(row, 1, new QTableWidgetItem(
        QString::number(entry["sdoReadLatencyMs"].toDouble(), 'f', 2)));
    historyTable_->setItem(row, 2, new QTableWidgetItem(
        QString::number(entry["sdoWriteLatencyMs"].toDouble(), 'f', 2)));
    historyTable_->setItem(row, 3, new QTableWidgetItem(
        QString::number(entry["stateTransitionMs"].toDouble(), 'f', 2)));
    historyTable_->setItem(row, 4, new QTableWidgetItem(
        QString::number(entry["freeRunCycleUs"].toDouble(), 'f', 1)));
    historyTable_->setItem(row, 5, new QTableWidgetItem(
        QString::number(entry["uiUpdateMs"].toDouble(), 'f', 2)));
    historyTable_->setItem(row, 6, new QTableWidgetItem(
        QString::number(entry["memoryMB"].toDouble(), 'f', 1)));
    historyTable_->setItem(row, 7, new QTableWidgetItem(
        QString::number(entry["cycleTimeUs"].toDouble(), 'f', 1)));
  }
}

void PerformanceDashboardPlugin::generateReport() {
  QJsonObject report = service_->performanceReport();
  QJsonDocument doc(report);
  reportLabel_->setText(doc.toJson(QJsonDocument::Indented));
}
