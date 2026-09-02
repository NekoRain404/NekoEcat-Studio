#include "SdoOptimizationPlugin.h"
#include "BatchOptimizerWidget.h"
#include "CacheOptimizerWidget.h"
#include "infra/EcatClient.h"
#include "services/SdoOptimizationService.h"

#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTextStream>
#include <QVBoxLayout>

SdoOptimizationPlugin::SdoOptimizationPlugin(EcatClient* client, EventBus* bus, QObject* parent) {
    if (parent)
        setParent(parent);

    service_ = new SdoOptimizationService(client, bus, this);
    buildUi();

    connect(service_, &SdoOptimizationService::optimizationCompleted, this,
            &SdoOptimizationPlugin::handleOptimizationCompleted);
    connect(service_, &SdoOptimizationService::optimizationApplied, this,
            &SdoOptimizationPlugin::handleOptimizationApplied);
}

QString SdoOptimizationPlugin::id() const {
    return "sdooptimization";
}
QString SdoOptimizationPlugin::displayName() const {
    return "SDO Optimization";
}
QString SdoOptimizationPlugin::displayNameZh() const {
    return QStringLiteral("SDO 优化");
}
int SdoOptimizationPlugin::defaultOrder() const {
    return 48;
}
bool SdoOptimizationPlugin::visible() const {
    return false;
}
QWidget* SdoOptimizationPlugin::widget() {
    return containerWidget_;
}

void SdoOptimizationPlugin::activate() {}
void SdoOptimizationPlugin::deactivate() {}
void SdoOptimizationPlugin::onConnectionChanged(bool connected) {
    Q_UNUSED(connected);
}

void SdoOptimizationPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* layout = new QVBoxLayout(containerWidget_);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* toolbar = new QHBoxLayout;
    statusLabel_ = new QLabel(tr("Ready"));
    toolbar->addWidget(statusLabel_);
    toolbar->addStretch();

    exportBtn_ = new QPushButton(tr("Export Report"));
    toolbar->addWidget(exportBtn_);

    layout->addLayout(toolbar);

    tabs_ = new QTabWidget;
    tabs_->addTab(buildCacheTab(), tr("Cache"));
    tabs_->addTab(buildBatchTab(), tr("Batch"));
    tabs_->addTab(buildPerformanceTab(), tr("Performance"));
    tabs_->addTab(buildErrorHandlerTab(), tr("Error Handler"));
    layout->addWidget(tabs_);

    connect(exportBtn_, &QPushButton::clicked, this, &SdoOptimizationPlugin::handleExport);
}

QWidget* SdoOptimizationPlugin::buildCacheTab() {
    cacheWidget_ = new CacheOptimizerWidget;
    cacheWidget_->updateCurrentCache(128, 0.45, 12.0);

    connect(cacheWidget_, &CacheOptimizerWidget::optimizeRequested, this, &SdoOptimizationPlugin::handleCacheOptimize);

    return cacheWidget_;
}

QWidget* SdoOptimizationPlugin::buildBatchTab() {
    batchWidget_ = new BatchOptimizerWidget;
    batchWidget_->updateCurrentBatch(1, 240.0, 45.0);

    connect(batchWidget_, &BatchOptimizerWidget::optimizeRequested, this, &SdoOptimizationPlugin::handleBatchOptimize);

    return batchWidget_;
}

QWidget* SdoOptimizationPlugin::buildPerformanceTab() {
    auto* widget = new QWidget;
    auto* layout = new QVBoxLayout(widget);

    auto* currentGroup = new QGroupBox(tr("Current Performance"));
    auto* currentLayout = new QFormLayout(currentGroup);
    perfThroughputLabel_ = new QLabel("512 Kbps");
    perfLatencyLabel_ = new QLabel("8.5 ms");
    perfCpuLabel_ = new QLabel("15%");
    currentLayout->addRow(tr("Throughput:"), perfThroughputLabel_);
    currentLayout->addRow(tr("Avg Latency:"), perfLatencyLabel_);
    currentLayout->addRow(tr("CPU Overhead:"), perfCpuLabel_);
    layout->addWidget(currentGroup);

    auto* optimGroup = new QGroupBox(tr("Optimization Result"));
    auto* optimLayout = new QVBoxLayout(optimGroup);
    auto* improveRow = new QHBoxLayout;
    improveRow->addWidget(new QLabel(tr("Improvement:")));
    auto* improveLabel = new QLabel(tr("N/A"));
    improveLabel->setStyleSheet("color: #22c55e; font-weight: bold;");
    improveRow->addWidget(improveLabel);
    improveRow->addStretch();
    optimLayout->addLayout(improveRow);
    layout->addWidget(optimGroup);

    perfOptimizeBtn_ = new QPushButton(tr("Apply Performance Optimization"));
    layout->addWidget(perfOptimizeBtn_);

    layout->addStretch();

    connect(perfOptimizeBtn_, &QPushButton::clicked, this, &SdoOptimizationPlugin::handlePerformanceOptimize);

    return widget;
}

QWidget* SdoOptimizationPlugin::buildErrorHandlerTab() {
    auto* widget = new QWidget;
    auto* layout = new QVBoxLayout(widget);

    auto* currentGroup = new QGroupBox(tr("Current Error Handling"));
    auto* currentLayout = new QFormLayout(currentGroup);
    errorRecoveryLabel_ = new QLabel("800 ms");
    errorRetryLabel_ = new QLabel("2");
    errorRateLabel_ = new QLabel("8%");
    currentLayout->addRow(tr("Recovery Time:"), errorRecoveryLabel_);
    currentLayout->addRow(tr("Retry Count:"), errorRetryLabel_);
    currentLayout->addRow(tr("Error Rate:"), errorRateLabel_);
    layout->addWidget(currentGroup);

    auto* optimGroup = new QGroupBox(tr("Optimization Result"));
    auto* optimLayout = new QVBoxLayout(optimGroup);
    auto* improveRow = new QHBoxLayout;
    improveRow->addWidget(new QLabel(tr("Improvement:")));
    auto* improveLabel = new QLabel(tr("N/A"));
    improveLabel->setStyleSheet("color: #22c55e; font-weight: bold;");
    improveRow->addWidget(improveLabel);
    improveRow->addStretch();
    optimLayout->addLayout(improveRow);
    layout->addWidget(optimGroup);

    errorOptimizeBtn_ = new QPushButton(tr("Apply Error Handling Optimization"));
    layout->addWidget(errorOptimizeBtn_);

    layout->addStretch();

    connect(errorOptimizeBtn_, &QPushButton::clicked, this, &SdoOptimizationPlugin::handleErrorHandlingOptimize);

    return widget;
}

void SdoOptimizationPlugin::handleCacheOptimize() {
    auto result = service_->optimizeCache();
    service_->applyOptimization(result);
}

void SdoOptimizationPlugin::handleBatchOptimize() {
    auto result = service_->optimizeBatch();
    service_->applyOptimization(result);
}

void SdoOptimizationPlugin::handlePerformanceOptimize() {
    auto result = service_->optimizePerformance();
    service_->applyOptimization(result);
}

void SdoOptimizationPlugin::handleErrorHandlingOptimize() {
    auto result = service_->optimizeErrorHandling();
    service_->applyOptimization(result);
}

void SdoOptimizationPlugin::handleOptimizationCompleted(const SdoOptimizationResult& result) {
    statusLabel_->setText(tr("Optimization ready: %1").arg(result.category));
}

void SdoOptimizationPlugin::handleOptimizationApplied(const SdoOptimizationResult& result) {
    statusLabel_->setText(
        tr("Applied: %1 (%2% improvement)").arg(result.category).arg(QString::number(result.improvement, 'f', 1)));

    if (result.category == tr("Cache"))
        cacheWidget_->setOptimized();
    else if (result.category == tr("Batch"))
        batchWidget_->setOptimized();
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

void SdoOptimizationPlugin::handleExport() {
    const QString path = QFileDialog::getSaveFileName(containerWidget_, tr("Export SDO Optimization Report"), QString(),
                                                      "Markdown (*.md);;Text (*.txt)");
    if (path.isEmpty())
        return;

    exportReportToFile(path);
}

bool SdoOptimizationPlugin::exportReportToFile(const QString& path) {
    if (path.isEmpty())
        return false;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << "# SDO Optimization Report\n\n";

    auto history = service_->optimizationHistory();
    for (const auto& r : history) {
        out << "## " << r.category << "\n";
        out << "- Description: " << r.description << "\n";
        out << "- Improvement: " << QString::number(r.improvement, 'f', 1) << "%\n";
        out << "- Applied: " << (r.applied ? "Yes" : "No") << "\n";
        out << "- Timestamp: " << r.timestamp.toString("yyyy-MM-dd hh:mm:ss") << "\n";
        if (!r.recommendations.isEmpty()) {
            out << "- Recommendations:\n";
            for (const auto& rec : r.recommendations)
                out << "  - " << rec << "\n";
        }
        out << "\n";
    }

    if (history.isEmpty()) {
        out << "No optimizations applied yet.\n";
    }
    return out.status() == QTextStream::Ok && file.flush();
}
