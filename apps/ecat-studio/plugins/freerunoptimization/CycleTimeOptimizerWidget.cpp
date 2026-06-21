#include "CycleTimeOptimizerWidget.h"
#include "services/FreeRunOptimizationService.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

CycleTimeOptimizerWidget::CycleTimeOptimizerWidget(QWidget *parent)
    : QWidget(parent) {
  buildUi();
}

void CycleTimeOptimizerWidget::buildUi() {
  auto *layout = new QVBoxLayout(this);

  auto *currentGroup = new QGroupBox(tr("Current Cycle Time"));
  auto *currentLayout = new QHBoxLayout(currentGroup);
  currentLayout->addWidget(new QLabel(tr("Cycle Time:")));
  cycleTimeValue_ = new QLabel(tr("N/A"));
  currentLayout->addWidget(cycleTimeValue_);
  currentLayout->addSpacing(20);
  currentLayout->addWidget(new QLabel(tr("Jitter:")));
  jitterValue_ = new QLabel(tr("N/A"));
  currentLayout->addWidget(jitterValue_);
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

  optimizeBtn_ = new QPushButton(tr("Apply Cycle Time Optimization"));
  optimizeBtn_->setEnabled(false);
  layout->addWidget(optimizeBtn_);

  layout->addStretch();

  connect(optimizeBtn_, &QPushButton::clicked,
          this, &CycleTimeOptimizerWidget::optimizeRequested);
}

void CycleTimeOptimizerWidget::updateCurrentCycleTime(double cycleTimeUs,
                                                      double jitterUs) {
  cycleTimeValue_->setText(QString::number(cycleTimeUs, 'f', 0) + " us");
  jitterValue_->setText(QString::number(jitterUs, 'f', 1) + " us");
}

void CycleTimeOptimizerWidget::showOptimizationResult(const FreeRunOptimizationResult &result) {
  double beforeCt = result.before["cycleTimeUs"].toDouble();
  double afterCt = result.after["cycleTimeUs"].toDouble();
  beforeLabel_->setText(QString::number(beforeCt, 'f', 0) + " us");
  afterLabel_->setText(QString::number(afterCt, 'f', 0) + " us");
  improvementLabel_->setText(QString::number(result.improvement, 'f', 1) + "%");
  optimizeBtn_->setEnabled(true);
}

void CycleTimeOptimizerWidget::setOptimized() {
  optimizeBtn_->setText(tr("Optimization Applied"));
  optimizeBtn_->setEnabled(false);
  optimizeBtn_->setStyleSheet("color: #22c55e;");
}
