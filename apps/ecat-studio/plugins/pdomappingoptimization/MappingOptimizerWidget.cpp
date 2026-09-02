#include "MappingOptimizerWidget.h"
#include "services/PdoMappingOptimizationService.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

MappingOptimizerWidget::MappingOptimizerWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
}

void MappingOptimizerWidget::buildUi() {
    auto* layout = new QVBoxLayout(this);

    auto* currentGroup = new QGroupBox(tr("Current Mapping"));
    auto* currentLayout = new QHBoxLayout(currentGroup);
    currentLayout->addWidget(new QLabel(tr("PDOs:")));
    totalPdosValue_ = new QLabel(tr("N/A"));
    currentLayout->addWidget(totalPdosValue_);
    currentLayout->addSpacing(20);
    currentLayout->addWidget(new QLabel(tr("Entries:")));
    totalEntriesValue_ = new QLabel(tr("N/A"));
    currentLayout->addWidget(totalEntriesValue_);
    currentLayout->addSpacing(20);
    currentLayout->addWidget(new QLabel(tr("Duplicates:")));
    duplicateEntriesValue_ = new QLabel(tr("N/A"));
    currentLayout->addWidget(duplicateEntriesValue_);
    currentLayout->addSpacing(20);
    currentLayout->addWidget(new QLabel(tr("Unused:")));
    unusedEntriesValue_ = new QLabel(tr("N/A"));
    currentLayout->addWidget(unusedEntriesValue_);
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

    optimizeBtn_ = new QPushButton(tr("Apply Mapping Optimization"));
    optimizeBtn_->setEnabled(false);
    layout->addWidget(optimizeBtn_);

    layout->addStretch();

    connect(optimizeBtn_, &QPushButton::clicked, this, &MappingOptimizerWidget::optimizeRequested);
}

void MappingOptimizerWidget::updateCurrentMapping(int totalPdos, int totalEntries, int duplicateEntries,
                                                  int unusedEntries) {
    totalPdosValue_->setText(QString::number(totalPdos));
    totalEntriesValue_->setText(QString::number(totalEntries));
    duplicateEntriesValue_->setText(QString::number(duplicateEntries));
    unusedEntriesValue_->setText(QString::number(unusedEntries));
}

void MappingOptimizerWidget::showOptimizationResult(const PdoMappingOptimizationResult& result) {
    int beforeEntries = result.before["totalEntries"].toInt();
    int afterEntries = result.after["totalEntries"].toInt();
    beforeLabel_->setText(QString::number(beforeEntries) + tr(" entries"));
    afterLabel_->setText(QString::number(afterEntries) + tr(" entries"));
    improvementLabel_->setText(QString::number(result.improvement, 'f', 1) + "%");
    optimizeBtn_->setEnabled(true);
}

void MappingOptimizerWidget::setOptimized() {
    optimizeBtn_->setText(tr("Optimization Applied"));
    optimizeBtn_->setEnabled(false);
    optimizeBtn_->setStyleSheet("color: #22c55e;");
}
