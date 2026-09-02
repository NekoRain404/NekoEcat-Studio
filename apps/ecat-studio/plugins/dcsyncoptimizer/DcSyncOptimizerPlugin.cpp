#include "DcSyncOptimizerPlugin.h"
#include "DriftOptimizerWidget.h"
#include "infra/EcatClient.h"
#include "services/DcSyncOptimizerService.h"
#include "services/EventBus.h"
#include "SyncOptimizerWidget.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonDocument>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTabWidget>
#include <QVBoxLayout>

DcSyncOptimizerPlugin::DcSyncOptimizerPlugin(EcatClient* client, EventBus* bus, QObject* parent)
    : service_(new DcSyncOptimizerService(client, bus, this)) {
    if (parent)
        setParent(parent);
    buildUi();

    connect(service_, &DcSyncOptimizerService::optimizationCompleted, this,
            &DcSyncOptimizerPlugin::handleOptimizationCompleted);
    connect(service_, &DcSyncOptimizerService::optimizationApplied, this,
            &DcSyncOptimizerPlugin::handleOptimizationApplied);
}

QString DcSyncOptimizerPlugin::id() const {
    return "dcsyncoptimizer";
}
QString DcSyncOptimizerPlugin::displayName() const {
    return "DC Sync Optimizer";
}
QString DcSyncOptimizerPlugin::displayNameZh() const {
    return QStringLiteral("DC 同步优化器");
}
int DcSyncOptimizerPlugin::defaultOrder() const {
    return 40;
}
bool DcSyncOptimizerPlugin::visible() const {
    return true;
}
QWidget* DcSyncOptimizerPlugin::widget() {
    return containerWidget_;
}

void DcSyncOptimizerPlugin::activate() {}
void DcSyncOptimizerPlugin::deactivate() {}
void DcSyncOptimizerPlugin::onConnectionChanged(bool /*connected*/) {}

void DcSyncOptimizerPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* layout = new QVBoxLayout(containerWidget_);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* topBar = new QHBoxLayout;
    auto* titleLabel = new QLabel(tr("DC Sync Optimizer"));
    titleLabel->setStyleSheet("font-weight: bold; font-size: 16px;");
    topBar->addWidget(titleLabel);
    topBar->addStretch();
    exportBtn_ = new QPushButton(tr("Export Report"));
    exportBtn_->setStyleSheet("QPushButton { background: #6366f1; color: white; padding: 6px 12px;"
                              "border-radius: 4px; }"
                              "QPushButton:hover { background: #4f46e5; }");
    connect(exportBtn_, &QPushButton::clicked, this, &DcSyncOptimizerPlugin::handleExport);
    topBar->addWidget(exportBtn_);
    layout->addLayout(topBar);

    tabs_ = new QTabWidget;
    tabs_->addTab(buildSyncTab(), tr("Sync Optimizer"));
    tabs_->addTab(buildDriftTab(), tr("Drift Optimizer"));
    tabs_->addTab(buildJitterTab(), tr("Jitter Optimizer"));
    tabs_->addTab(buildConfigTab(), tr("Configuration Optimizer"));
    layout->addWidget(tabs_);
}

QWidget* DcSyncOptimizerPlugin::buildSyncTab() {
    syncWidget_ = new SyncOptimizerWidget;
    connect(syncWidget_, &SyncOptimizerWidget::applyRequested, this, &DcSyncOptimizerPlugin::handleApplySync);

    auto* optimizeBtn = new QPushButton(tr("Analyze & Optimize Sync"));
    optimizeBtn->setStyleSheet("QPushButton { background: #22c55e; color: white; padding: 8px 16px;"
                               "border-radius: 4px; font-weight: bold; }"
                               "QPushButton:hover { background: #16a34a; }");
    connect(optimizeBtn, &QPushButton::clicked, this, &DcSyncOptimizerPlugin::handleSyncOptimize);

    auto* wrapper = new QWidget;
    auto* layout = new QVBoxLayout(wrapper);
    layout->addWidget(optimizeBtn);
    layout->addWidget(syncWidget_);
    return wrapper;
}

QWidget* DcSyncOptimizerPlugin::buildDriftTab() {
    driftWidget_ = new DriftOptimizerWidget;
    connect(driftWidget_, &DriftOptimizerWidget::applyRequested, this, &DcSyncOptimizerPlugin::handleApplyDrift);

    auto* optimizeBtn = new QPushButton(tr("Analyze & Optimize Drift"));
    optimizeBtn->setStyleSheet("QPushButton { background: #22c55e; color: white; padding: 8px 16px;"
                               "border-radius: 4px; font-weight: bold; }"
                               "QPushButton:hover { background: #16a34a; }");
    connect(optimizeBtn, &QPushButton::clicked, this, &DcSyncOptimizerPlugin::handleDriftOptimize);

    auto* wrapper = new QWidget;
    auto* layout = new QVBoxLayout(wrapper);
    layout->addWidget(optimizeBtn);
    layout->addWidget(driftWidget_);
    return wrapper;
}

QWidget* DcSyncOptimizerPlugin::buildJitterTab() {
    jitterWidget_ = new QWidget;
    auto* layout = new QVBoxLayout(jitterWidget_);
    layout->setContentsMargins(4, 4, 4, 4);

    jitterOptimizeBtn_ = new QPushButton(tr("Analyze & Optimize Jitter"));
    jitterOptimizeBtn_->setStyleSheet("QPushButton { background: #22c55e; color: white; padding: 8px 16px;"
                                      "border-radius: 4px; font-weight: bold; }"
                                      "QPushButton:hover { background: #16a34a; }");
    connect(jitterOptimizeBtn_, &QPushButton::clicked, this, &DcSyncOptimizerPlugin::handleJitterOptimize);
    layout->addWidget(jitterOptimizeBtn_);

    auto* resultsGroup = new QGroupBox(tr("Optimization Results"));
    auto* resultsLayout = new QVBoxLayout(resultsGroup);

    jitterParamsTable_ = new QTableWidget;
    jitterParamsTable_->setColumnCount(2);
    jitterParamsTable_->setHorizontalHeaderLabels({tr("Parameter"), tr("Value")});
    jitterParamsTable_->horizontalHeader()->setStretchLastSection(true);
    jitterParamsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    jitterParamsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    resultsLayout->addWidget(jitterParamsTable_);

    jitterImprovementLabel_ = new QLabel(tr("Improvement: --"));
    jitterImprovementLabel_->setStyleSheet("font-weight: bold; font-size: 14px; color: #22c55e;");
    resultsLayout->addWidget(jitterImprovementLabel_);

    jitterRecommendationsLabel_ = new QLabel;
    jitterRecommendationsLabel_->setWordWrap(true);
    jitterRecommendationsLabel_->setStyleSheet("color: #94a3b8;");
    resultsLayout->addWidget(jitterRecommendationsLabel_);

    layout->addWidget(resultsGroup);

    jitterApplyBtn_ = new QPushButton(tr("Apply Optimization"));
    jitterApplyBtn_->setStyleSheet("QPushButton { background: #3b82f6; color: white; padding: 8px 16px;"
                                   "border-radius: 4px; font-weight: bold; }"
                                   "QPushButton:hover { background: #2563eb; }");
    connect(jitterApplyBtn_, &QPushButton::clicked, this, &DcSyncOptimizerPlugin::handleApplySync);
    layout->addWidget(jitterApplyBtn_);

    return jitterWidget_;
}

QWidget* DcSyncOptimizerPlugin::buildConfigTab() {
    configWidget_ = new QWidget;
    auto* layout = new QVBoxLayout(configWidget_);
    layout->setContentsMargins(4, 4, 4, 4);

    configOptimizeBtn_ = new QPushButton(tr("Analyze & Optimize Configuration"));
    configOptimizeBtn_->setStyleSheet("QPushButton { background: #22c55e; color: white; padding: 8px 16px;"
                                      "border-radius: 4px; font-weight: bold; }"
                                      "QPushButton:hover { background: #16a34a; }");
    connect(configOptimizeBtn_, &QPushButton::clicked, this, &DcSyncOptimizerPlugin::handleConfigOptimize);
    layout->addWidget(configOptimizeBtn_);

    auto* resultsGroup = new QGroupBox(tr("Optimization Results"));
    auto* resultsLayout = new QVBoxLayout(resultsGroup);

    configParamsTable_ = new QTableWidget;
    configParamsTable_->setColumnCount(2);
    configParamsTable_->setHorizontalHeaderLabels({tr("Parameter"), tr("Value")});
    configParamsTable_->horizontalHeader()->setStretchLastSection(true);
    configParamsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    configParamsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    resultsLayout->addWidget(configParamsTable_);

    configImprovementLabel_ = new QLabel(tr("Improvement: --"));
    configImprovementLabel_->setStyleSheet("font-weight: bold; font-size: 14px; color: #22c55e;");
    resultsLayout->addWidget(configImprovementLabel_);

    configRecommendationsLabel_ = new QLabel;
    configRecommendationsLabel_->setWordWrap(true);
    configRecommendationsLabel_->setStyleSheet("color: #94a3b8;");
    resultsLayout->addWidget(configRecommendationsLabel_);

    layout->addWidget(resultsGroup);

    configApplyBtn_ = new QPushButton(tr("Apply Optimization"));
    configApplyBtn_->setStyleSheet("QPushButton { background: #3b82f6; color: white; padding: 8px 16px;"
                                   "border-radius: 4px; font-weight: bold; }"
                                   "QPushButton:hover { background: #2563eb; }");
    connect(configApplyBtn_, &QPushButton::clicked, this, &DcSyncOptimizerPlugin::handleApplySync);
    layout->addWidget(configApplyBtn_);

    return configWidget_;
}

void DcSyncOptimizerPlugin::handleSyncOptimize() {
    lastSyncResult_ = service_->optimizeSync();
    syncWidget_->displayResult(lastSyncResult_);
}

void DcSyncOptimizerPlugin::handleDriftOptimize() {
    lastDriftResult_ = service_->optimizeDrift();
    driftWidget_->displayResult(lastDriftResult_);
}

void DcSyncOptimizerPlugin::handleJitterOptimize() {
    lastJitterResult_ = service_->optimizeJitter();
    jitterParamsTable_->setRowCount(0);
    const auto keys = lastJitterResult_.after.keys();
    for (const auto& key : keys) {
        int row = jitterParamsTable_->rowCount();
        jitterParamsTable_->insertRow(row);
        jitterParamsTable_->setItem(row, 0, new QTableWidgetItem(key));
        jitterParamsTable_->setItem(row, 1, new QTableWidgetItem(lastJitterResult_.after[key].toVariant().toString()));
    }
    jitterImprovementLabel_->setText(tr("Improvement: %1%").arg(lastJitterResult_.improvement, 0, 'f', 1));
    QString recs;
    for (const auto& rec : lastJitterResult_.recommendations)
        recs += "  " + rec + "\n";
    jitterRecommendationsLabel_->setText(tr("Recommendations:\n") + recs);
}

void DcSyncOptimizerPlugin::handleConfigOptimize() {
    lastConfigResult_ = service_->optimizeConfiguration();
    configParamsTable_->setRowCount(0);
    const auto keys = lastConfigResult_.after.keys();
    for (const auto& key : keys) {
        int row = configParamsTable_->rowCount();
        configParamsTable_->insertRow(row);
        configParamsTable_->setItem(row, 0, new QTableWidgetItem(key));
        configParamsTable_->setItem(row, 1, new QTableWidgetItem(lastConfigResult_.after[key].toVariant().toString()));
    }
    configImprovementLabel_->setText(tr("Improvement: %1%").arg(lastConfigResult_.improvement, 0, 'f', 1));
    QString recs;
    for (const auto& rec : lastConfigResult_.recommendations)
        recs += "  " + rec + "\n";
    configRecommendationsLabel_->setText(tr("Recommendations:\n") + recs);
}

void DcSyncOptimizerPlugin::handleApplySync() {
    if (!lastSyncResult_.category.isEmpty())
        service_->applyOptimization(lastSyncResult_);
}

void DcSyncOptimizerPlugin::handleApplyDrift() {
    if (!lastDriftResult_.category.isEmpty())
        service_->applyOptimization(lastDriftResult_);
}

void DcSyncOptimizerPlugin::handleOptimizationCompleted(const DcSyncOptimizationResult& /*result*/) {}

void DcSyncOptimizerPlugin::handleOptimizationApplied(const DcSyncOptimizationResult& /*result*/) {}

void DcSyncOptimizerPlugin::handleExport() {
    QString report;
    report += "# DC Sync Optimization Report\n\n";
    report += QString("Generated: %1\n\n").arg(QDateTime::currentDateTime().toString(Qt::ISODate));

    const auto results = service_->pendingResults();
    for (const auto& r : results) {
        report += QString("## %1 Optimization\n").arg(r.category);
        report += QString("Description: %1\n").arg(r.description);
        report += QString("Improvement: %1%\n").arg(r.improvement, 0, 'f', 1);
        report += QString("Applied: %1\n").arg(r.applied ? "Yes" : "No");
        report += "\n### Recommendations\n";
        for (const auto& rec : r.recommendations)
            report += "- " + rec + "\n";
        report += "\n### Before Parameters\n";
        const auto beforeKeys = r.before.keys();
        for (const auto& key : beforeKeys)
            report += QString("- %1: %2\n").arg(key, r.before[key].toVariant().toString());
        report += "\n### After Parameters\n";
        const auto afterKeys = r.after.keys();
        for (const auto& key : afterKeys)
            report += QString("- %1: %2\n").arg(key, r.after[key].toVariant().toString());
        report += "\n---\n\n";
    }

    emit updateDiagnostics("info", "DcSyncOptimizer", report.length() > 200 ? report.left(200) + "..." : report);
}
