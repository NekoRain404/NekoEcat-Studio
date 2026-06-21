#include "BatchOptimizerWidget.h"
#include "services/SdoOptimizationService.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

BatchOptimizerWidget::BatchOptimizerWidget(QWidget *parent)
    : QWidget(parent) {
  buildUi();
}

void BatchOptimizerWidget::buildUi() {
  auto *layout = new QVBoxLayout(this);

  auto *currentGroup = new QGroupBox(tr("Current Batch"));
  auto *currentLayout = new QHBoxLayout(currentGroup);
  currentLayout->addWidget(new QLabel(tr("Batch Size:")));
  batchSizeValue_ = new QLabel(tr("N/A"));
  currentLayout->addWidget(batchSizeValue_);
  currentLayout->addSpacing(20);
  currentLayout->addWidget(new QLabel(tr("Transfer Time:")));
  transferTimeValue_ = new QLabel(tr("N/A"));
  currentLayout->addWidget(transferTimeValue_);
  currentLayout->addSpacing(20);
  currentLayout->addWidget(new QLabel(tr("Overhead:")));
  overheadValue_ = new QLabel(tr("N/A"));
  currentLayout->addWidget(overheadValue_);
  currentLayout->addStretch();
  layout->addWidget(currentGroup);

  auto *comparisonGroup = new QGroupBox(tr("Before / After Comparison"));
  auto *compLayout = new QVBoxLayout(comparisonGroup);

  auto *beforeRow = new QHBoxLayout;
  beforeRow->addWidget(new QLabel(tr("Before:")));
  beforeLabel_ = new QLabel(tr("N/A"));
  beforeRow->addWidget(beforeLabel_);
  beforeRow->addStretch();
  compLayout->addLayout(beforeRow);

  auto *afterRow = new QHBoxLayout;
  afterRow->addWidget(new QLabel(tr("After:")));
  afterLabel_ = new QLabel(tr("N/A"));
  afterRow->addWidget(afterLabel_);
  afterRow->addStretch();
  compLayout->addLayout(afterRow);

  auto *improveRow = new QHBoxLayout;
  improveRow->addWidget(new QLabel(tr("Improvement:")));
  improvementLabel_ = new QLabel(tr("N/A"));
  improvementLabel_->setStyleSheet("color: #22c55e; font-weight: bold;");
  improveRow->addWidget(improvementLabel_);
  improveRow->addStretch();
  compLayout->addLayout(improveRow);

  layout->addWidget(comparisonGroup);

  optimizeBtn_ = new QPushButton(tr("Apply Batch Optimization"));
  optimizeBtn_->setEnabled(false);
  layout->addWidget(optimizeBtn_);

  layout->addStretch();

  connect(optimizeBtn_, &QPushButton::clicked,
          this, &BatchOptimizerWidget::optimizeRequested);
}

void BatchOptimizerWidget::updateCurrentBatch(int batchSize, double transferTime,
                                              double overhead) {
  batchSizeValue_->setText(QString::number(batchSize));
  transferTimeValue_->setText(QString::number(transferTime, 'f', 1) + " ms");
  overheadValue_->setText(QString::number(overhead, 'f', 1) + "%");
}

void BatchOptimizerWidget::showOptimizationResult(const SdoOptimizationResult &result) {
  int beforeBatch = result.before["batchSize"].toInt();
  int afterBatch = result.after["batchSize"].toInt();
  beforeLabel_->setText(QString::number(beforeBatch));
  afterLabel_->setText(QString::number(afterBatch));
  improvementLabel_->setText(QString::number(result.improvement, 'f', 1) + "%");
  optimizeBtn_->setEnabled(true);
}

void BatchOptimizerWidget::setOptimized() {
  optimizeBtn_->setText(tr("Optimization Applied"));
  optimizeBtn_->setEnabled(false);
  optimizeBtn_->setStyleSheet("color: #22c55e;");
}
