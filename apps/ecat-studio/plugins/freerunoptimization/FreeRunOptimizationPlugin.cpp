#include "FreeRunOptimizationPlugin.h"
#include "CycleTimeOptimizerWidget.h"
#include "DataMappingOptimizerWidget.h"
#include "services/FreeRunOptimizationService.h"
#include "infra/EcatClient.h"

#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTextStream>
#include <QVBoxLayout>

FreeRunOptimizationPlugin::FreeRunOptimizationPlugin(EcatClient *client,
                                                     EventBus *bus,
                                                     QObject *parent) {
  if (parent) setParent(parent);

  service_ = new FreeRunOptimizationService(client, bus, this);
  buildUi();

  connect(service_, &FreeRunOptimizationService::optimizationCompleted,
          this, &FreeRunOptimizationPlugin::handleOptimizationCompleted);
  connect(service_, &FreeRunOptimizationService::optimizationApplied,
          this, &FreeRunOptimizationPlugin::handleOptimizationApplied);
}

QString FreeRunOptimizationPlugin::id() const {
  return "freerunoptimization";
}
QString FreeRunOptimizationPlugin::displayName() const {
  return "Free Run Optimization";
}
QString FreeRunOptimizationPlugin::displayNameZh() const {
  return QStringLiteral("自由运行优化");
}
int FreeRunOptimizationPlugin::defaultOrder() const { return 44; }
bool FreeRunOptimizationPlugin::visible() const { return false; }
QWidget *FreeRunOptimizationPlugin::widget() { return containerWidget_; }

void FreeRunOptimizationPlugin::activate() {}
void FreeRunOptimizationPlugin::deactivate() {}
void FreeRunOptimizationPlugin::onConnectionChanged(bool connected) {
  Q_UNUSED(connected);
}

void FreeRunOptimizationPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *layout = new QVBoxLayout(containerWidget_);
  layout->setContentsMargins(0, 0, 0, 0);

  auto *toolbar = new QHBoxLayout;
  statusLabel_ = new QLabel(tr("Ready"));
  toolbar->addWidget(statusLabel_);
  toolbar->addStretch();

  exportBtn_ = new QPushButton(tr("Export Report"));
  toolbar->addWidget(exportBtn_);

  layout->addLayout(toolbar);

  tabs_ = new QTabWidget;
  tabs_->addTab(buildCycleTimeTab(), tr("Cycle Time"));
  tabs_->addTab(buildDataMappingTab(), tr("Data Mapping"));
  tabs_->addTab(buildPerformanceTab(), tr("Performance"));
  tabs_->addTab(buildErrorHandlerTab(), tr("Error Handler"));
  layout->addWidget(tabs_);

  connect(exportBtn_, &QPushButton::clicked,
          this, &FreeRunOptimizationPlugin::handleExport);
}

QWidget *FreeRunOptimizationPlugin::buildCycleTimeTab() {
  cycleTimeWidget_ = new CycleTimeOptimizerWidget;
  cycleTimeWidget_->updateCurrentCycleTime(1000.0, 50.0);

  connect(cycleTimeWidget_, &CycleTimeOptimizerWidget::optimizeRequested,
          this, &FreeRunOptimizationPlugin::handleCycleTimeOptimize);

  return cycleTimeWidget_;
}

QWidget *FreeRunOptimizationPlugin::buildDataMappingTab() {
  dataMappingWidget_ = new DataMappingOptimizerWidget;
  dataMappingWidget_->updateCurrentMapping(256, 16, 64);

  connect(dataMappingWidget_, &DataMappingOptimizerWidget::optimizeRequested,
          this, &FreeRunOptimizationPlugin::handleDataMappingOptimize);

  return dataMappingWidget_;
}

QWidget *FreeRunOptimizationPlugin::buildPerformanceTab() {
  auto *widget = new QWidget;
  auto *layout = new QVBoxLayout(widget);

  auto *currentGroup = new QGroupBox(tr("Current Performance"));
  auto *currentLayout = new QFormLayout(currentGroup);
  perfCpuLabel_ = new QLabel("35%");
  perfBusLabel_ = new QLabel("60%");
  perfFrameLabel_ = new QLabel("1000 fps");
  currentLayout->addRow(tr("CPU Usage:"), perfCpuLabel_);
  currentLayout->addRow(tr("Bus Load:"), perfBusLabel_);
  currentLayout->addRow(tr("Frame Rate:"), perfFrameLabel_);
  layout->addWidget(currentGroup);

  auto *optimGroup = new QGroupBox(tr("Optimization Result"));
  auto *optimLayout = new QVBoxLayout(optimGroup);
  auto *improveRow = new QHBoxLayout;
  improveRow->addWidget(new QLabel(tr("Improvement:")));
  auto *improveLabel = new QLabel(tr("N/A"));
  improveLabel->setStyleSheet("color: #22c55e; font-weight: bold;");
  improveRow->addWidget(improveLabel);
  improveRow->addStretch();
  optimLayout->addLayout(improveRow);
  layout->addWidget(optimGroup);

  perfOptimizeBtn_ = new QPushButton(tr("Apply Performance Optimization"));
  layout->addWidget(perfOptimizeBtn_);

  layout->addStretch();

  connect(perfOptimizeBtn_, &QPushButton::clicked,
          this, &FreeRunOptimizationPlugin::handlePerformanceOptimize);

  return widget;
}

QWidget *FreeRunOptimizationPlugin::buildErrorHandlerTab() {
  auto *widget = new QWidget;
  auto *layout = new QVBoxLayout(widget);

  auto *currentGroup = new QGroupBox(tr("Current Error Handling"));
  auto *currentLayout = new QFormLayout(currentGroup);
  errorRecoveryLabel_ = new QLabel("500 ms");
  errorRetryLabel_ = new QLabel("3");
  errorRateLabel_ = new QLabel("5%");
  currentLayout->addRow(tr("Recovery Time:"), errorRecoveryLabel_);
  currentLayout->addRow(tr("Retry Count:"), errorRetryLabel_);
  currentLayout->addRow(tr("Error Rate:"), errorRateLabel_);
  layout->addWidget(currentGroup);

  auto *optimGroup = new QGroupBox(tr("Optimization Result"));
  auto *optimLayout = new QVBoxLayout(optimGroup);
  auto *improveRow = new QHBoxLayout;
  improveRow->addWidget(new QLabel(tr("Improvement:")));
  auto *improveLabel = new QLabel(tr("N/A"));
  improveLabel->setStyleSheet("color: #22c55e; font-weight: bold;");
  improveRow->addWidget(improveLabel);
  improveRow->addStretch();
  optimLayout->addLayout(improveRow);
  layout->addWidget(optimGroup);

  errorOptimizeBtn_ = new QPushButton(tr("Apply Error Handling Optimization"));
  layout->addWidget(errorOptimizeBtn_);

  layout->addStretch();

  connect(errorOptimizeBtn_, &QPushButton::clicked,
          this, &FreeRunOptimizationPlugin::handleErrorHandlingOptimize);

  return widget;
}

void FreeRunOptimizationPlugin::handleCycleTimeOptimize() {
  auto result = service_->optimizeCycleTime();
  service_->applyOptimization(result);
}

void FreeRunOptimizationPlugin::handleDataMappingOptimize() {
  auto result = service_->optimizeDataMapping();
  service_->applyOptimization(result);
}

void FreeRunOptimizationPlugin::handlePerformanceOptimize() {
  auto result = service_->optimizePerformance();
  service_->applyOptimization(result);
}

void FreeRunOptimizationPlugin::handleErrorHandlingOptimize() {
  auto result = service_->optimizeErrorHandling();
  service_->applyOptimization(result);
}

void FreeRunOptimizationPlugin::handleOptimizationCompleted(const FreeRunOptimizationResult &result) {
  statusLabel_->setText(tr("Optimization ready: %1").arg(result.category));
}

void FreeRunOptimizationPlugin::handleOptimizationApplied(const FreeRunOptimizationResult &result) {
  statusLabel_->setText(tr("Applied: %1 (%2% improvement)")
                            .arg(result.category)
                            .arg(QString::number(result.improvement, 'f', 1)));

  if (result.category == tr("Cycle Time"))
    cycleTimeWidget_->setOptimized();
  else if (result.category == tr("Data Mapping"))
    dataMappingWidget_->setOptimized();
  else if (result.category == tr("Performance")) {
    perfOptimizeBtn_->setText(tr("Optimization Applied"));
    perfOptimizeBtn_->setEnabled(false);
    perfOptimizeBtn_->setStyleSheet("color: #22c55e;");
  } else if (result.category == tr("Error Handling")) {
    errorOptimizeBtn_->setText(tr("Optimization Applied"));
    errorOptimizeBtn_->setEnabled(false);
    errorOptimizeBtn_->setStyleSheet("color: #22c55e;");
  }
}

void FreeRunOptimizationPlugin::handleExport() {
  const QString path = QFileDialog::getSaveFileName(
      containerWidget_, tr("Export Free Run Optimization Report"), QString(),
      "Markdown (*.md);;Text (*.txt)");
  if (path.isEmpty()) return;

  exportReportToFile(path);
}

bool FreeRunOptimizationPlugin::exportReportToFile(const QString &path) {
  if (path.isEmpty()) return false;

  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

  QTextStream out(&file);
  out << "# Free Run Optimization Report\n\n";

  auto history = service_->optimizationHistory();
  for (const auto &r : history) {
    out << "## " << r.category << "\n";
    out << "- Description: " << r.description << "\n";
    out << "- Improvement: " << QString::number(r.improvement, 'f', 1) << "%\n";
    out << "- Applied: " << (r.applied ? "Yes" : "No") << "\n";
    out << "- Timestamp: " << r.timestamp.toString("yyyy-MM-dd hh:mm:ss") << "\n";
    if (!r.recommendations.isEmpty()) {
      out << "- Recommendations:\n";
      for (const auto &rec : r.recommendations)
        out << "  - " << rec << "\n";
    }
    out << "\n";
  }

  if (history.isEmpty()) {
    out << "No optimizations applied yet.\n";
  }
  return out.status() == QTextStream::Ok && file.flush();
}
