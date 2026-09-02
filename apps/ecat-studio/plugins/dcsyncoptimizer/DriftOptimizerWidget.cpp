#include "DriftOptimizerWidget.h"
#include "services/DcSyncOptimizerService.h"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>

DriftOptimizerWidget::DriftOptimizerWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
}

void DriftOptimizerWidget::buildUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);

    auto* algoGroup = new QGroupBox(tr("Compensation Algorithm"));
    auto* algoLayout = new QFormLayout(algoGroup);

    algorithmCombo_ = new QComboBox;
    algorithmCombo_->addItems({"PID", "Linear Regression", "Kalman Filter"});
    algoLayout->addRow(tr("Algorithm:"), algorithmCombo_);

    kpSpin_ = new QDoubleSpinBox;
    kpSpin_->setRange(0.0, 10.0);
    kpSpin_->setSingleStep(0.05);
    kpSpin_->setValue(0.5);
    algoLayout->addRow(tr("Kp:"), kpSpin_);

    kiSpin_ = new QDoubleSpinBox;
    kiSpin_->setRange(0.0, 5.0);
    kiSpin_->setSingleStep(0.01);
    kiSpin_->setValue(0.1);
    algoLayout->addRow(tr("Ki:"), kiSpin_);

    kdSpin_ = new QDoubleSpinBox;
    kdSpin_->setRange(0.0, 5.0);
    kdSpin_->setSingleStep(0.01);
    kdSpin_->setValue(0.05);
    algoLayout->addRow(tr("Kd:"), kdSpin_);

    historyWindowSpin_ = new QSpinBox;
    historyWindowSpin_->setRange(16, 4096);
    historyWindowSpin_->setValue(256);
    algoLayout->addRow(tr("History Window:"), historyWindowSpin_);

    thresholdSpin_ = new QDoubleSpinBox;
    thresholdSpin_->setRange(100.0, 1000000.0);
    thresholdSpin_->setValue(5000.0);
    thresholdSpin_->setSuffix(tr(" ns"));
    algoLayout->addRow(tr("Drift Threshold:"), thresholdSpin_);

    mainLayout->addWidget(algoGroup);

    auto* resultsGroup = new QGroupBox(tr("Optimization Results"));
    auto* resultsLayout = new QVBoxLayout(resultsGroup);

    paramsTable_ = new QTableWidget;
    paramsTable_->setColumnCount(2);
    paramsTable_->setHorizontalHeaderLabels({tr("Parameter"), tr("Value")});
    paramsTable_->horizontalHeader()->setStretchLastSection(true);
    paramsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    paramsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    resultsLayout->addWidget(paramsTable_);

    improvementLabel_ = new QLabel(tr("Improvement: --"));
    improvementLabel_->setStyleSheet("font-weight: bold; font-size: 14px; color: #22c55e;");
    resultsLayout->addWidget(improvementLabel_);

    recommendationsLabel_ = new QLabel;
    recommendationsLabel_->setWordWrap(true);
    recommendationsLabel_->setStyleSheet("color: #94a3b8;");
    resultsLayout->addWidget(recommendationsLabel_);

    mainLayout->addWidget(resultsGroup);

    applyBtn_ = new QPushButton(tr("Apply Optimization"));
    applyBtn_->setStyleSheet("QPushButton { background: #3b82f6; color: white; padding: 8px 16px;"
                             "border-radius: 4px; font-weight: bold; }"
                             "QPushButton:hover { background: #2563eb; }");
    connect(applyBtn_, &QPushButton::clicked, this, &DriftOptimizerWidget::applyRequested);
    mainLayout->addWidget(applyBtn_);
}

void DriftOptimizerWidget::displayResult(const DcSyncOptimizationResult& result) {
    paramsTable_->setRowCount(0);
    const auto keys = result.after.keys();
    for (const auto& key : keys) {
        int row = paramsTable_->rowCount();
        paramsTable_->insertRow(row);
        paramsTable_->setItem(row, 0, new QTableWidgetItem(key));
        paramsTable_->setItem(row, 1, new QTableWidgetItem(result.after[key].toVariant().toString()));
    }

    improvementLabel_->setText(tr("Improvement: %1%").arg(result.improvement, 0, 'f', 1));

    QString recs;
    for (const auto& rec : result.recommendations)
        recs += "  " + rec + "\n";
    recommendationsLabel_->setText(tr("Recommendations:\n") + recs);
}

void DriftOptimizerWidget::clear() {
    paramsTable_->setRowCount(0);
    improvementLabel_->setText(tr("Improvement: --"));
    recommendationsLabel_->clear();
}
