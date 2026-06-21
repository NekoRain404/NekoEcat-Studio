#include "SyncOptimizerWidget.h"
#include "services/DcSyncOptimizerService.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>

SyncOptimizerWidget::SyncOptimizerWidget(QWidget *parent) : QWidget(parent) {
    buildUi();
}

void SyncOptimizerWidget::buildUi() {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);

    auto *comparisonGroup = new QGroupBox(tr("Before / After Comparison"));
    auto *compLayout = new QHBoxLayout(comparisonGroup);

    auto *beforeGroup = new QGroupBox(tr("Current"));
    auto *beforeLayout = new QVBoxLayout(beforeGroup);
    beforeTable_ = new QTableWidget;
    beforeTable_->setColumnCount(2);
    beforeTable_->setHorizontalHeaderLabels({tr("Parameter"), tr("Value")});
    beforeTable_->horizontalHeader()->setStretchLastSection(true);
    beforeTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    beforeTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    beforeLayout->addWidget(beforeTable_);
    compLayout->addWidget(beforeGroup);

    auto *afterGroup = new QGroupBox(tr("Optimized"));
    auto *afterLayout = new QVBoxLayout(afterGroup);
    afterTable_ = new QTableWidget;
    afterTable_->setColumnCount(2);
    afterTable_->setHorizontalHeaderLabels({tr("Parameter"), tr("Value")});
    afterTable_->horizontalHeader()->setStretchLastSection(true);
    afterTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    afterTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    afterLayout->addWidget(afterTable_);
    compLayout->addWidget(afterGroup);

    mainLayout->addWidget(comparisonGroup);

    improvementLabel_ = new QLabel(tr("Improvement: --"));
    improvementLabel_->setStyleSheet("font-weight: bold; font-size: 14px; color: #22c55e;");
    mainLayout->addWidget(improvementLabel_);

    recommendationsLabel_ = new QLabel;
    recommendationsLabel_->setWordWrap(true);
    recommendationsLabel_->setStyleSheet("color: #94a3b8;");
    mainLayout->addWidget(recommendationsLabel_);

    applyBtn_ = new QPushButton(tr("Apply Optimization"));
    applyBtn_->setStyleSheet(
        "QPushButton { background: #3b82f6; color: white; padding: 8px 16px;"
        "border-radius: 4px; font-weight: bold; }"
        "QPushButton:hover { background: #2563eb; }");
    connect(applyBtn_, &QPushButton::clicked, this,
            &SyncOptimizerWidget::applyRequested);
    mainLayout->addWidget(applyBtn_);
}

void SyncOptimizerWidget::displayResult(const DcSyncOptimizationResult &result) {
    beforeTable_->setRowCount(0);
    const auto beforeKeys = result.before.keys();
    for (const auto &key : beforeKeys) {
        int row = beforeTable_->rowCount();
        beforeTable_->insertRow(row);
        beforeTable_->setItem(row, 0, new QTableWidgetItem(key));
        beforeTable_->setItem(
            row, 1,
            new QTableWidgetItem(
                result.before[key].toVariant().toString()));
    }

    afterTable_->setRowCount(0);
    const auto afterKeys = result.after.keys();
    for (const auto &key : afterKeys) {
        int row = afterTable_->rowCount();
        afterTable_->insertRow(row);
        afterTable_->setItem(row, 0, new QTableWidgetItem(key));
        afterTable_->setItem(
            row, 1,
            new QTableWidgetItem(
                result.after[key].toVariant().toString()));
    }

    improvementLabel_->setText(
        tr("Improvement: %1%").arg(result.improvement, 0, 'f', 1));

    QString recs;
    for (const auto &rec : result.recommendations)
        recs += "  " + rec + "\n";
    recommendationsLabel_->setText(tr("Recommendations:\n") + recs);
}

void SyncOptimizerWidget::clear() {
    beforeTable_->setRowCount(0);
    afterTable_->setRowCount(0);
    improvementLabel_->setText(tr("Improvement: --"));
    recommendationsLabel_->clear();
}
