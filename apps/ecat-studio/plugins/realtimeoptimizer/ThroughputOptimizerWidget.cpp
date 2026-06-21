// ThroughputOptimizerWidget — widget for throughput optimization.

#include "ThroughputOptimizerWidget.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

ThroughputOptimizerWidget::ThroughputOptimizerWidget(QWidget *parent)
    : QWidget(parent) {
  buildUi();
}

void ThroughputOptimizerWidget::buildUi() {
  auto *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(10, 10, 10, 10);
  mainLayout->setSpacing(8);

  auto *currentGroup = new QGroupBox(tr("Current Throughput"));
  auto *currentLayout = new QHBoxLayout(currentGroup);
  currentThroughputLabel_ = new QLabel(tr("1000 f/s"));
  currentThroughputLabel_->setStyleSheet(
      "color: #cccccc; font-weight: bold; font-size: 24px;");
  currentLayout->addWidget(currentThroughputLabel_);
  mainLayout->addWidget(currentGroup);

  auto *resultGroup = new QGroupBox(tr("Optimization Results"));
  auto *resultLayout = new QVBoxLayout(resultGroup);

  auto *comparisonLayout = new QHBoxLayout;

  auto *beforeLayout = new QVBoxLayout;
  beforeLayout->addWidget(new QLabel(tr("Before:")));
  auto *beforeValue = new QLabel(tr("1000 f/s"));
  beforeValue->setStyleSheet("color: #f59e0b; font-size: 18px;");
  beforeLayout->addWidget(beforeValue);
  comparisonLayout->addLayout(beforeLayout);

  comparisonLayout->addStretch();

  auto *arrowLabel = new QLabel(tr("→"));
  arrowLabel->setStyleSheet("color: #888888; font-size: 24px;");
  comparisonLayout->addWidget(arrowLabel);

  comparisonLayout->addStretch();

  auto *afterLayout = new QVBoxLayout;
  afterLayout->addWidget(new QLabel(tr("After:")));
  optimizedThroughputLabel_ = new QLabel(tr("1450 f/s"));
  optimizedThroughputLabel_->setStyleSheet(
      "color: #22c55e; font-size: 18px;");
  afterLayout->addWidget(optimizedThroughputLabel_);
  comparisonLayout->addLayout(afterLayout);

  resultLayout->addLayout(comparisonLayout);

  improvementLabel_ = new QLabel(tr("Improvement: +45.0%"));
  improvementLabel_->setStyleSheet(
      "color: #60a5fa; font-weight: bold; font-size: 16px;");
  resultLayout->addWidget(improvementLabel_);

  mainLayout->addWidget(resultGroup);

  auto *bottleneckGroup = new QGroupBox(tr("Bottleneck Analysis"));
  auto *bottleneckLayout = new QVBoxLayout(bottleneckGroup);
  bottleneckEdit_ = new QTextEdit;
  bottleneckEdit_->setReadOnly(true);
  bottleneckEdit_->setMaximumHeight(80);
  bottleneckEdit_->setStyleSheet(
      "background-color: #1e1e2e; color: #cccccc;");
  bottleneckEdit_->setText(
      tr("• PDO mapping size below optimal\n"
         "• Frame coalescing disabled\n"
         "• Logical ring commands not utilized"));
  bottleneckLayout->addWidget(bottleneckEdit_);
  mainLayout->addWidget(bottleneckGroup);

  auto *recGroup = new QGroupBox(tr("Recommendations"));
  auto *recLayout = new QVBoxLayout(recGroup);
  recommendationsEdit_ = new QTextEdit;
  recommendationsEdit_->setReadOnly(true);
  recommendationsEdit_->setMaximumHeight(100);
  recommendationsEdit_->setStyleSheet(
      "background-color: #1e1e2e; color: #cccccc;");
  recLayout->addWidget(recommendationsEdit_);
  mainLayout->addWidget(recGroup);

  auto *buttonLayout = new QHBoxLayout;
  optimizeBtn_ = new QPushButton(tr("Analyze & Optimize"));
  connect(optimizeBtn_, &QPushButton::clicked, this,
          &ThroughputOptimizerWidget::optimizeRequested);
  buttonLayout->addWidget(optimizeBtn_);

  applyBtn_ = new QPushButton(tr("Apply Optimization"));
  applyBtn_->setEnabled(false);
  buttonLayout->addWidget(applyBtn_);

  mainLayout->addLayout(buttonLayout);
  mainLayout->addStretch();
}

void ThroughputOptimizerWidget::updateResult(
    const OptimizationResult &result) {
  lastResult_ = result;

  currentThroughputLabel_->setText(
      tr("%1 f/s").arg(result.before, 0, 'f', 0));
  optimizedThroughputLabel_->setText(
      tr("%1 f/s").arg(result.after, 0, 'f', 0));
  improvementLabel_->setText(
      tr("Improvement: +%1%").arg(result.improvement, 0, 'f', 1));

  QString recs;
  for (const auto &rec : result.recommendations) {
    recs += "• " + rec + "\n";
  }
  recommendationsEdit_->setText(recs);

  applyBtn_->setEnabled(true);
}
