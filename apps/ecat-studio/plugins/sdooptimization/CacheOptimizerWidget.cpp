#include "CacheOptimizerWidget.h"
#include "services/SdoOptimizationService.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

CacheOptimizerWidget::CacheOptimizerWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
}

void CacheOptimizerWidget::buildUi() {
    auto* layout = new QVBoxLayout(this);

    auto* currentGroup = new QGroupBox(tr("Current Cache"));
    auto* currentLayout = new QHBoxLayout(currentGroup);
    currentLayout->addWidget(new QLabel(tr("Size:")));
    cacheSizeValue_ = new QLabel(tr("N/A"));
    currentLayout->addWidget(cacheSizeValue_);
    currentLayout->addSpacing(20);
    currentLayout->addWidget(new QLabel(tr("Hit Rate:")));
    hitRateValue_ = new QLabel(tr("N/A"));
    currentLayout->addWidget(hitRateValue_);
    currentLayout->addSpacing(20);
    currentLayout->addWidget(new QLabel(tr("Miss Latency:")));
    missLatencyValue_ = new QLabel(tr("N/A"));
    currentLayout->addWidget(missLatencyValue_);
    currentLayout->addStretch();
    layout->addWidget(currentGroup);

    auto* comparisonGroup = new QGroupBox(tr("Before / After Comparison"));
    auto* compLayout = new QVBoxLayout(comparisonGroup);

    auto* beforeRow = new QHBoxLayout;
    beforeRow->addWidget(new QLabel(tr("Before:")));
    beforeLabel_ = new QLabel(tr("N/A"));
    beforeRow->addWidget(beforeLabel_);
    beforeRow->addStretch();
    compLayout->addLayout(beforeRow);

    auto* afterRow = new QHBoxLayout;
    afterRow->addWidget(new QLabel(tr("After:")));
    afterLabel_ = new QLabel(tr("N/A"));
    afterRow->addWidget(afterLabel_);
    afterRow->addStretch();
    compLayout->addLayout(afterRow);

    auto* improveRow = new QHBoxLayout;
    improveRow->addWidget(new QLabel(tr("Improvement:")));
    improvementLabel_ = new QLabel(tr("N/A"));
    improvementLabel_->setStyleSheet("color: #22c55e; font-weight: bold;");
    improveRow->addWidget(improvementLabel_);
    improveRow->addStretch();
    compLayout->addLayout(improveRow);

    layout->addWidget(comparisonGroup);

    optimizeBtn_ = new QPushButton(tr("Apply Cache Optimization"));
    optimizeBtn_->setEnabled(false);
    layout->addWidget(optimizeBtn_);

    layout->addStretch();

    connect(optimizeBtn_, &QPushButton::clicked, this, &CacheOptimizerWidget::optimizeRequested);
}

void CacheOptimizerWidget::updateCurrentCache(int size, double hitRate, double missLatency) {
    cacheSizeValue_->setText(QString::number(size) + tr(" entries"));
    hitRateValue_->setText(QString::number(hitRate * 100.0, 'f', 1) + "%");
    missLatencyValue_->setText(QString::number(missLatency, 'f', 1) + " ms");
}

void CacheOptimizerWidget::showOptimizationResult(const SdoOptimizationResult& result) {
    int beforeSize = result.before["cacheSize"].toInt();
    int afterSize = result.after["cacheSize"].toInt();
    beforeLabel_->setText(QString::number(beforeSize) + tr(" entries"));
    afterLabel_->setText(QString::number(afterSize) + tr(" entries"));
    improvementLabel_->setText(QString::number(result.improvement, 'f', 1) + "%");
    optimizeBtn_->setEnabled(true);
}

void CacheOptimizerWidget::setOptimized() {
    optimizeBtn_->setText(tr("Optimization Applied"));
    optimizeBtn_->setEnabled(false);
    optimizeBtn_->setStyleSheet("color: #22c55e;");
}
