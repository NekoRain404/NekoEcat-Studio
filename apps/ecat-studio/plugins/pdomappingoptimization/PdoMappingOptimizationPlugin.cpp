#include "PdoMappingOptimizationPlugin.h"
#include "MappingOptimizerWidget.h"
#include "SizeOptimizerWidget.h"
#include "services/PdoMappingOptimizationService.h"

#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTextStream>
#include <QVBoxLayout>

PdoMappingOptimizationPlugin::PdoMappingOptimizationPlugin(QObject *parent) {
  if (parent) setParent(parent);

  service_ = new PdoMappingOptimizationService(this);
  buildUi();

  connect(service_, &PdoMappingOptimizationService::optimizationCompleted,
          this, &PdoMappingOptimizationPlugin::handleOptimizationCompleted);
  connect(service_, &PdoMappingOptimizationService::optimizationApplied,
          this, &PdoMappingOptimizationPlugin::handleOptimizationApplied);
}

QString PdoMappingOptimizationPlugin::id() const {
  return "pdomappingoptimization";
}
QString PdoMappingOptimizationPlugin::displayName() const {
  return "PDO Mapping Optimization";
}
QString PdoMappingOptimizationPlugin::displayNameZh() const {
  return QStringLiteral("PDO 映射优化");
}
int PdoMappingOptimizationPlugin::defaultOrder() const { return 46; }
bool PdoMappingOptimizationPlugin::visible() const { return true; }
QWidget *PdoMappingOptimizationPlugin::widget() { return containerWidget_; }

void PdoMappingOptimizationPlugin::activate() {}
void PdoMappingOptimizationPlugin::deactivate() {}
void PdoMappingOptimizationPlugin::onConnectionChanged(bool connected) {
  Q_UNUSED(connected);
}

void PdoMappingOptimizationPlugin::buildUi() {
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
  tabs_->addTab(buildMappingTab(), tr("Mapping"));
  tabs_->addTab(buildSizeTab(), tr("Size"));
  tabs_->addTab(buildAlignmentTab(), tr("Alignment"));
  tabs_->addTab(buildPerformanceTab(), tr("Performance"));
  layout->addWidget(tabs_);

  connect(exportBtn_, &QPushButton::clicked,
          this, &PdoMappingOptimizationPlugin::handleExport);
}

QWidget *PdoMappingOptimizationPlugin::buildMappingTab() {
  mappingWidget_ = new MappingOptimizerWidget;
  mappingWidget_->updateCurrentMapping(8, 32, 4, 6);

  connect(mappingWidget_, &MappingOptimizerWidget::optimizeRequested,
          this, &PdoMappingOptimizationPlugin::handleMappingOptimize);

  return mappingWidget_;
}

QWidget *PdoMappingOptimizationPlugin::buildSizeTab() {
  sizeWidget_ = new SizeOptimizerWidget;
  sizeWidget_->updateCurrentSize(256, 128, 128, 48);

  connect(sizeWidget_, &SizeOptimizerWidget::optimizeRequested,
          this, &PdoMappingOptimizationPlugin::handleSizeOptimize);

  return sizeWidget_;
}

QWidget *PdoMappingOptimizationPlugin::buildAlignmentTab() {
  auto *widget = new QWidget;
  auto *layout = new QVBoxLayout(widget);

  auto *currentGroup = new QGroupBox(tr("Current Alignment"));
  auto *currentLayout = new QFormLayout(currentGroup);
  alignMisalignLabel_ = new QLabel("3");
  alignPaddingLabel_ = new QLabel("24 bytes");
  alignCrossLabel_ = new QLabel("2");
  currentLayout->addRow(tr("Misaligned Entries:"), alignMisalignLabel_);
  currentLayout->addRow(tr("Padding Bytes:"), alignPaddingLabel_);
  currentLayout->addRow(tr("Cross-SM Boundaries:"), alignCrossLabel_);
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

  alignOptimizeBtn_ = new QPushButton(tr("Apply Alignment Optimization"));
  layout->addWidget(alignOptimizeBtn_);

  layout->addStretch();

  connect(alignOptimizeBtn_, &QPushButton::clicked,
          this, &PdoMappingOptimizationPlugin::handleAlignmentOptimize);

  return widget;
}

QWidget *PdoMappingOptimizationPlugin::buildPerformanceTab() {
  auto *widget = new QWidget;
  auto *layout = new QVBoxLayout(widget);

  auto *currentGroup = new QGroupBox(tr("Current Performance"));
  auto *currentLayout = new QFormLayout(currentGroup);
  perfCycleLabel_ = new QLabel("1000 us");
  perfBusLabel_ = new QLabel("65%");
  perfThroughputLabel_ = new QLabel("85 Mbps");
  currentLayout->addRow(tr("Cycle Time:"), perfCycleLabel_);
  currentLayout->addRow(tr("Bus Utilization:"), perfBusLabel_);
  currentLayout->addRow(tr("Throughput:"), perfThroughputLabel_);
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
          this, &PdoMappingOptimizationPlugin::handlePerformanceOptimize);

  return widget;
}

void PdoMappingOptimizationPlugin::handleMappingOptimize() {
  auto result = service_->optimizeMapping();
  service_->applyOptimization(result);
}

void PdoMappingOptimizationPlugin::handleSizeOptimize() {
  auto result = service_->optimizeSize();
  service_->applyOptimization(result);
}

void PdoMappingOptimizationPlugin::handleAlignmentOptimize() {
  auto result = service_->optimizeAlignment();
  service_->applyOptimization(result);
}

void PdoMappingOptimizationPlugin::handlePerformanceOptimize() {
  auto result = service_->optimizePerformance();
  service_->applyOptimization(result);
}

void PdoMappingOptimizationPlugin::handleOptimizationCompleted(const PdoMappingOptimizationResult &result) {
  statusLabel_->setText(tr("Optimization ready: %1").arg(result.category));
}

void PdoMappingOptimizationPlugin::handleOptimizationApplied(const PdoMappingOptimizationResult &result) {
  statusLabel_->setText(tr("Applied: %1 (%2% improvement)")
                            .arg(result.category)
                            .arg(QString::number(result.improvement, 'f', 1)));

  if (result.category == tr("Mapping"))
    mappingWidget_->setOptimized();
  else if (result.category == tr("Size"))
    sizeWidget_->setOptimized();
  else if (result.category == tr("Alignment")) {
    alignOptimizeBtn_->setText(tr("Optimization Applied"));
    alignOptimizeBtn_->setEnabled(false);
    alignOptimizeBtn_->setStyleSheet("color: #22c55e;");
  } else if (result.category == tr("Performance")) {
    perfOptimizeBtn_->setText(tr("Optimization Applied"));
    perfOptimizeBtn_->setEnabled(false);
    perfOptimizeBtn_->setStyleSheet("color: #22c55e;");
  }
}

void PdoMappingOptimizationPlugin::handleExport() {
  const QString path = QFileDialog::getSaveFileName(
      containerWidget_, tr("Export PDO Mapping Optimization Report"), QString(),
      "Markdown (*.md);;Text (*.txt)");
  if (path.isEmpty()) return;

  exportReportToFile(path);
}

bool PdoMappingOptimizationPlugin::exportReportToFile(const QString &path) {
  if (path.isEmpty()) return false;

  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

  QTextStream out(&file);
  out << "# PDO Mapping Optimization Report\n\n";

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
