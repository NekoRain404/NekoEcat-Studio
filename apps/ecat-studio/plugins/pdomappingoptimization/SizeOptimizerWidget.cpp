#include "SizeOptimizerWidget.h"
#include "services/PdoMappingOptimizationService.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

SizeOptimizerWidget::SizeOptimizerWidget(QWidget *parent)
    : QWidget(parent) {
  buildUi();
}

void SizeOptimizerWidget::buildUi() {
  auto *layout = new QVBoxLayout(this);

  auto *currentGroup = new QGroupBox(tr("Current Size"));
  auto *currentLayout = new QHBoxLayout(currentGroup);
  currentLayout->addWidget(new QLabel(tr("Total:")));
  totalBytesValue_ = new QLabel(tr("N/A"));
  currentLayout->addWidget(totalBytesValue_);
  currentLayout->addSpacing(20);
  currentLayout->addWidget(new QLabel(tr("Input:")));
  inputValue_ = new QLabel(tr("N/A"));
  currentLayout->addWidget(inputValue_);
  currentLayout->addSpacing(20);
  currentLayout->addWidget(new QLabel(tr("Output:")));
  outputValue_ = new QLabel(tr("N/A"));
  currentLayout->addWidget(outputValue_);
  currentLayout->addSpacing(20);
  currentLayout->addWidget(new QLabel(tr("Wasted:")));
  wastedValue_ = new QLabel(tr("N/A"));
  currentLayout->addWidget(wastedValue_);
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

  optimizeBtn_ = new QPushButton(tr("Apply Size Optimization"));
  optimizeBtn_->setEnabled(false);
  layout->addWidget(optimizeBtn_);

  layout->addStretch();

  connect(optimizeBtn_, &QPushButton::clicked,
          this, &SizeOptimizerWidget::optimizeRequested);
}

void SizeOptimizerWidget::updateCurrentSize(int totalBytes, int inputBytes,
                                             int outputBytes, int wastedBytes) {
  totalBytesValue_->setText(QString::number(totalBytes) + tr(" bytes"));
  inputValue_->setText(QString::number(inputBytes) + tr(" bytes"));
  outputValue_->setText(QString::number(outputBytes) + tr(" bytes"));
  wastedValue_->setText(QString::number(wastedBytes) + tr(" bytes"));
}

void SizeOptimizerWidget::showOptimizationResult(const PdoMappingOptimizationResult &result) {
  int beforeBytes = result.before["totalBytes"].toInt();
  int afterBytes = result.after["totalBytes"].toInt();
  beforeLabel_->setText(QString::number(beforeBytes) + tr(" bytes"));
  afterLabel_->setText(QString::number(afterBytes) + tr(" bytes"));
  improvementLabel_->setText(QString::number(result.improvement, 'f', 1) + "%");
  optimizeBtn_->setEnabled(true);
}

void SizeOptimizerWidget::setOptimized() {
  optimizeBtn_->setText(tr("Optimization Applied"));
  optimizeBtn_->setEnabled(false);
  optimizeBtn_->setStyleSheet("color: #22c55e;");
}
