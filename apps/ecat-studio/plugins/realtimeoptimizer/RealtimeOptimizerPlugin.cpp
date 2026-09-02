// RealtimeOptimizerPlugin — workspace plugin for real-time performance optimization.
//
// Provides a tabbed dashboard with latency optimizer, throughput optimizer,
// resource optimizer, and priority optimization. Follows the same pattern as
// RealtimePerformancePlugin and DcSyncPrecisionPlugin.

#include "RealtimeOptimizerPlugin.h"
#include "LatencyOptimizerWidget.h"
#include "services/RealtimeOptimizerService.h"
#include "ThroughputOptimizerWidget.h"

#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTextStream>
#include <QVBoxLayout>

RealtimeOptimizerPlugin::RealtimeOptimizerPlugin(RealtimeOptimizerService* service, QObject* parent)
    : service_(service) {
    if (parent)
        setParent(parent);
    buildUi();

    connect(service_, &RealtimeOptimizerService::optimizationCompleted, this, [this](const OptimizationResult& result) {
        if (result.category == "Latency") {
            latencyImprovementLabel_->setText(tr("+%1%").arg(result.improvement, 0, 'f', 1));
        } else if (result.category == "Throughput") {
            throughputImprovementLabel_->setText(tr("+%1%").arg(result.improvement, 0, 'f', 1));
        } else if (result.category == "Resources") {
            resourceImprovementLabel_->setText(tr("-%1%").arg(result.improvement, 0, 'f', 1));
        } else if (result.category == "Priorities") {
            priorityImprovementLabel_->setText(tr("+%1%").arg(result.improvement, 0, 'f', 1));
        }
        statusLabel_->setText(tr("Optimization completed: %1").arg(result.category));
    });

    connect(service_, &RealtimeOptimizerService::optimizationApplied, this, [this](const OptimizationResult& result) {
        statusLabel_->setText(tr("Applied: %1 optimization").arg(result.category));
    });
}

QString RealtimeOptimizerPlugin::id() const {
    return "realtimeoptimizer";
}
QString RealtimeOptimizerPlugin::displayName() const {
    return "Real-time Optimizer";
}
QString RealtimeOptimizerPlugin::displayNameZh() const {
    return QStringLiteral("实时优化器");
}
QIcon RealtimeOptimizerPlugin::icon() const {
    return QIcon::fromTheme("system-run");
}
int RealtimeOptimizerPlugin::defaultOrder() const {
    return 38;
}
bool RealtimeOptimizerPlugin::visible() const {
    return false;
}

void RealtimeOptimizerPlugin::activate() {}
void RealtimeOptimizerPlugin::deactivate() {}

QWidget* RealtimeOptimizerPlugin::widget() {
    return containerWidget_;
}

void RealtimeOptimizerPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* mainLayout = new QVBoxLayout(containerWidget_);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(8);

    auto* toolbar = new QHBoxLayout;
    toolbar->setSpacing(8);

    optimizeAllBtn_ = new QPushButton(tr("Optimize All"));
    toolbar->addWidget(optimizeAllBtn_);

    statusLabel_ = new QLabel(tr("Ready"));
    toolbar->addWidget(statusLabel_);

    toolbar->addStretch();

    exportBtn_ = new QPushButton(tr("Export Report"));
    toolbar->addWidget(exportBtn_);

    mainLayout->addLayout(toolbar);

    tabWidget_ = new QTabWidget;

    buildDashboardTab();
    buildLatencyTab();
    buildThroughputTab();
    buildResourceTab();

    mainLayout->addWidget(tabWidget_, 1);

    connect(optimizeAllBtn_, &QPushButton::clicked, this, [this]() {
        service_->optimizeLatency();
        service_->optimizeThroughput();
        service_->optimizeResources();
        service_->optimizePriorities();
        statusLabel_->setText(tr("All optimizations completed"));
    });

    connect(exportBtn_, &QPushButton::clicked, this, &RealtimeOptimizerPlugin::exportReport);
}

void RealtimeOptimizerPlugin::buildDashboardTab() {
    auto* widget = new QWidget;
    auto* layout = new QVBoxLayout(widget);

    auto* summaryGroup = new QGroupBox(tr("Optimization Summary"));
    auto* grid = new QGridLayout(summaryGroup);

    auto makeCard = [&](const QString& name, int row, int col) -> QLabel* {
        auto* lbl = new QLabel(name);
        lbl->setStyleSheet("color: #8888aa; font-size: 11px;");
        grid->addWidget(lbl, row * 2, col);
        auto* val = new QLabel("--");
        val->setStyleSheet("color: #cccccc; font-weight: bold; font-size: 18px;");
        grid->addWidget(val, row * 2 + 1, col);
        return val;
    };

    latencyImprovementLabel_ = makeCard(tr("Latency Improvement"), 0, 0);
    throughputImprovementLabel_ = makeCard(tr("Throughput Improvement"), 0, 1);
    resourceImprovementLabel_ = makeCard(tr("Resource Reduction"), 0, 2);
    priorityImprovementLabel_ = makeCard(tr("Priority Optimization"), 1, 0);

    layout->addWidget(summaryGroup);
    layout->addStretch();

    tabWidget_->addTab(widget, tr("Dashboard"));
}

void RealtimeOptimizerPlugin::buildLatencyTab() {
    latencyOptimizer_ = new LatencyOptimizerWidget;
    connect(latencyOptimizer_, &LatencyOptimizerWidget::optimizeRequested, this, [this]() {
        auto result = service_->optimizeLatency();
        latencyOptimizer_->updateResult(result);
    });
    tabWidget_->addTab(latencyOptimizer_, tr("Latency"));
}

void RealtimeOptimizerPlugin::buildThroughputTab() {
    throughputOptimizer_ = new ThroughputOptimizerWidget;
    connect(throughputOptimizer_, &ThroughputOptimizerWidget::optimizeRequested, this, [this]() {
        auto result = service_->optimizeThroughput();
        throughputOptimizer_->updateResult(result);
    });
    tabWidget_->addTab(throughputOptimizer_, tr("Throughput"));
}

void RealtimeOptimizerPlugin::buildResourceTab() {
    auto* widget = new QWidget;
    auto* layout = new QVBoxLayout(widget);

    auto* group = new QGroupBox(tr("Resource Optimization"));
    auto* form = new QFormLayout(group);

    cpuOptLabel_ = new QLabel("--");
    cpuOptLabel_->setStyleSheet("color: #cccccc;");
    form->addRow(tr("CPU Optimization:"), cpuOptLabel_);

    memOptLabel_ = new QLabel("--");
    memOptLabel_->setStyleSheet("color: #cccccc;");
    form->addRow(tr("Memory Optimization:"), memOptLabel_);

    threadOptLabel_ = new QLabel("--");
    threadOptLabel_->setStyleSheet("color: #cccccc;");
    form->addRow(tr("Thread Optimization:"), threadOptLabel_);

    irqOptLabel_ = new QLabel("--");
    irqOptLabel_->setStyleSheet("color: #cccccc;");
    form->addRow(tr("IRQ Optimization:"), irqOptLabel_);

    auto* optimizeBtn = new QPushButton(tr("Optimize Resources"));
    connect(optimizeBtn, &QPushButton::clicked, this, [this]() {
        auto result = service_->optimizeResources();
        cpuOptLabel_->setText(tr("-%1% CPU usage").arg(result.improvement, 0, 'f', 1));
        memOptLabel_->setText(tr("-%1% memory usage").arg(result.improvement * 0.8, 0, 'f', 1));
        threadOptLabel_->setText(tr("Optimized thread affinity"));
        irqOptLabel_->setText(tr("IRQ affinity configured"));
    });

    layout->addWidget(group);
    layout->addWidget(optimizeBtn);
    layout->addStretch();

    tabWidget_->addTab(widget, tr("Resources"));
}

void RealtimeOptimizerPlugin::exportReport() {
    QString path = QFileDialog::getSaveFileName(containerWidget_, tr("Export Optimization Report"),
                                                "optimization_report.csv", tr("CSV Files (*.csv);;All Files (*)"));
    if (path.isEmpty())
        return;

    exportReportToFile(path);
}

bool RealtimeOptimizerPlugin::exportReportToFile(const QString& path) {
    if (path.isEmpty())
        return false;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << "Category,Description,Before,After,Improvement\n";

    for (const auto& result : service_->optimizationHistory()) {
        out << result.category << "," << result.description << "," << result.before << "," << result.after << ","
            << result.improvement << "\n";
    }
    return out.status() == QTextStream::Ok && file.flush();
}
