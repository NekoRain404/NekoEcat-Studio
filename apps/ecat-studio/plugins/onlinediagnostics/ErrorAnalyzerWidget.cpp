#include "ErrorAnalyzerWidget.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QTime>
#include <QVBoxLayout>

ErrorAnalyzerWidget::ErrorAnalyzerWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
}

void ErrorAnalyzerWidget::buildUi() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    auto* summaryRow = new QHBoxLayout;
    summaryRow->setSpacing(16);

    totalLabel_ = new QLabel(tr("Total Errors: 0"));
    crcLabel_ = new QLabel(tr("CRC Errors: 0"));
    lostLabel_ = new QLabel(tr("Lost Frames: 0"));
    rateLabel_ = new QLabel(tr("Error Rate: 0/s"));

    auto styleLabel = [](QLabel* lbl) {
        QFont f = lbl->font();
        f.setPointSize(11);
        f.setBold(true);
        lbl->setFont(f);
        lbl->setStyleSheet("color: #e0e0e0; background: #2a2a3a; padding: 6px 12px; border-radius: 4px;");
    };

    styleLabel(totalLabel_);
    styleLabel(crcLabel_);
    styleLabel(lostLabel_);
    styleLabel(rateLabel_);

    summaryRow->addWidget(totalLabel_);
    summaryRow->addWidget(crcLabel_);
    summaryRow->addWidget(lostLabel_);
    summaryRow->addWidget(rateLabel_);
    summaryRow->addStretch();

    layout->addLayout(summaryRow);

    historyTable_ = new QTableWidget;
    historyTable_->setColumnCount(4);
    historyTable_->setHorizontalHeaderLabels({tr("Time"), tr("Type"), tr("Count"), tr("Severity")});
    historyTable_->horizontalHeader()->setStretchLastSection(true);
    historyTable_->verticalHeader()->setVisible(false);
    historyTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    historyTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    historyTable_->setShowGrid(false);
    historyTable_->setAlternatingRowColors(true);
    layout->addWidget(historyTable_, 1);
}

void ErrorAnalyzerWidget::updateErrors(const ErrorRate& rate) {
    totalLabel_->setText(tr("Total Errors: %1").arg(rate.totalErrors));
    crcLabel_->setText(tr("CRC Errors: %1").arg(rate.crcErrors));
    lostLabel_->setText(tr("Lost Frames: %1").arg(rate.lostErrors));
    rateLabel_->setText(tr("Error Rate: %1/s").arg(rate.rate, 0, 'f', 1));

    if (rate.totalErrors > 0) {
        int row = historyTable_->rowCount();
        if (row > 500) {
            historyTable_->removeRow(0);
            row = historyTable_->rowCount();
        }
        historyTable_->insertRow(row);

        QTime t = QTime::currentTime();
        historyTable_->setItem(row, 0, new QTableWidgetItem(t.toString("HH:mm:ss.zzz")));

        QString type = rate.crcErrors > 0 ? "CRC" : (rate.lostErrors > 0 ? "Lost" : "General");
        historyTable_->setItem(row, 1, new QTableWidgetItem(type));
        historyTable_->setItem(row, 2, new QTableWidgetItem(QString::number(rate.totalErrors)));

        QString severity = rate.rate > 10 ? tr("Critical") : (rate.rate > 1 ? tr("Warning") : tr("Info"));
        auto* sevItem = new QTableWidgetItem(severity);
        if (severity == tr("Critical"))
            sevItem->setForeground(QColor(255, 80, 80));
        else if (severity == tr("Warning"))
            sevItem->setForeground(QColor(255, 200, 60));
        else
            sevItem->setForeground(QColor(100, 200, 100));
        historyTable_->setItem(row, 3, sevItem);
    }
}
