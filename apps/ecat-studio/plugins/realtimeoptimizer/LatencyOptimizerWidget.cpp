// LatencyOptimizerWidget — widget for latency optimization.

#include "LatencyOptimizerWidget.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

LatencyOptimizerWidget::LatencyOptimizerWidget(QWidget *parent)
    : QWidget(parent) {
  buildUi();
}

void LatencyOptimizerWidget::buildUi() {
  auto *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(10, 10, 10, 10);
  mainLayout->setSpacing(8);

  auto *currentGroup = new QGroupBox(tr("Current Latency"));
  auto *currentLayout = new QHBoxLayout(currentGroup);
  currentLatencyLabel_ = new QLabel(tr("150.0 us"));
  currentLatencyLabel_->setStyleSheet(
      "color: #cccccc; font-weight: bold; font-size: 24px;");
  currentLayout->addWidget(currentLatencyLabel_);
  mainLayout->addWidget(currentGroup);

  auto *resultGroup = new QGroupBox(tr("Optimization Results"));
  auto *resultLayout = new QVBoxLayout(resultGroup);

  auto *comparisonLayout = new QHBoxLayout;

  auto *beforeLayout = new QVBoxLayout;
  beforeLayout->addWidget(new QLabel(tr("Before:")));
  auto *beforeValue = new QLabel(tr("150.0 us"));
  beforeValue->setStyleSheet("color: #ef4444; font-size: 18px;");
  beforeLayout->addWidget(beforeValue);
  comparisonLayout->addLayout(beforeLayout);

  comparisonLayout->addStretch();

  auto *arrowLabel = new QLabel(tr("→"));
  arrowLabel->setStyleSheet("color: #888888; font-size: 24px;");
  comparisonLayout->addWidget(arrowLabel);

  comparisonLayout->addStretch();

  auto *afterLayout = new QVBoxLayout;
  afterLayout->addWidget(new QLabel(tr("After:")));
  optimizedLatencyLabel_ = new QLabel(tr("85.0 us"));
  optimizedLatencyLabel_->setStyleSheet(
      "color: #22c55e; font-size: 18px;");
  afterLayout->addWidget(optimizedLatencyLabel_);
  comparisonLayout->addLayout(afterLayout);

  resultLayout->addLayout(comparisonLayout);

  improvementLabel_ = new QLabel(tr("Improvement: +43.3%"));
  improvementLabel_->setStyleSheet(
      "color: #60a5fa; font-weight: bold; font-size: 16px;");
  resultLayout->addWidget(improvementLabel_);

  mainLayout->addWidget(resultGroup);

  auto *recGroup = new QGroupBox(tr("Recommendations"));
  auto *recLayout = new QVBoxLayout(recGroup);
  recommendationsEdit_ = new QTextEdit;
  recommendationsEdit_->setReadOnly(true);
  recommendationsEdit_->setMaximumHeight(150);
  recommendationsEdit_->setStyleSheet(
      "background-color: #1e1e2e; color: #cccccc;");
  recLayout->addWidget(recommendationsEdit_);
  mainLayout->addWidget(recGroup);

  auto *buttonLayout = new QHBoxLayout;
  optimizeBtn_ = new QPushButton(tr("Analyze & Optimize"));
  connect(optimizeBtn_, &QPushButton::clicked, this,
          &LatencyOptimizerWidget::optimizeRequested);
  buttonLayout->addWidget(optimizeBtn_);

  applyBtn_ = new QPushButton(tr("Apply Optimization"));
  applyBtn_->setEnabled(false);
  buttonLayout->addWidget(applyBtn_);

  mainLayout->addLayout(buttonLayout);
  mainLayout->addStretch();
}

void LatencyOptimizerWidget::updateResult(const OptimizationResult &result) {
  lastResult_ = result;

  currentLatencyLabel_->setText(tr("%1 us").arg(result.before, 0, 'f', 1));
  optimizedLatencyLabel_->setText(tr("%1 us").arg(result.after, 0, 'f', 1));
  improvementLabel_->setText(
      tr("Improvement: +%1%").arg(result.improvement, 0, 'f', 1));

  QString recs;
  for (const auto &rec : result.recommendations) {
    recs += "• " + rec + "\n";
  }
  recommendationsEdit_->setText(recs);

  applyBtn_->setEnabled(true);
}
