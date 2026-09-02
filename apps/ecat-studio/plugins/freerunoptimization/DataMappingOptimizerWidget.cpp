#include "DataMappingOptimizerWidget.h"
#include "services/FreeRunOptimizationService.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

DataMappingOptimizerWidget::DataMappingOptimizerWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
}

void DataMappingOptimizerWidget::buildUi() {
    auto* layout = new QVBoxLayout(this);

    auto* currentGroup = new QGroupBox(tr("Current Data Mapping"));
    auto* currentLayout = new QHBoxLayout(currentGroup);
    currentLayout->addWidget(new QLabel(tr("Total Bytes:")));
    totalBytesValue_ = new QLabel(tr("N/A"));
    currentLayout->addWidget(totalBytesValue_);
    currentLayout->addSpacing(20);
    currentLayout->addWidget(new QLabel(tr("Entries:")));
    entriesValue_ = new QLabel(tr("N/A"));
    currentLayout->addWidget(entriesValue_);
    currentLayout->addSpacing(20);
    currentLayout->addWidget(new QLabel(tr("Unused:")));
    unusedBytesValue_ = new QLabel(tr("N/A"));
    currentLayout->addWidget(unusedBytesValue_);
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

    optimizeBtn_ = new QPushButton(tr("Apply Data Mapping Optimization"));
    optimizeBtn_->setEnabled(false);
    layout->addWidget(optimizeBtn_);

    layout->addStretch();

    connect(optimizeBtn_, &QPushButton::clicked, this, &DataMappingOptimizerWidget::optimizeRequested);
}

void DataMappingOptimizerWidget::updateCurrentMapping(int totalBytes, int entries, int unusedBytes) {
    totalBytesValue_->setText(QString::number(totalBytes) + " bytes");
    entriesValue_->setText(QString::number(entries));
    unusedBytesValue_->setText(QString::number(unusedBytes) + " bytes");
}

void DataMappingOptimizerWidget::showOptimizationResult(const FreeRunOptimizationResult& result) {
    int beforeBytes = result.before["totalPdoBytes"].toInt();
    int afterBytes = result.after["totalPdoBytes"].toInt();
    beforeLabel_->setText(QString::number(beforeBytes) + " bytes");
    afterLabel_->setText(QString::number(afterBytes) + " bytes");
    improvementLabel_->setText(QString::number(result.improvement, 'f', 1) + "%");
    optimizeBtn_->setEnabled(true);
}

void DataMappingOptimizerWidget::setOptimized() {
    optimizeBtn_->setText(tr("Optimization Applied"));
    optimizeBtn_->setEnabled(false);
    optimizeBtn_->setStyleSheet("color: #22c55e;");
}
