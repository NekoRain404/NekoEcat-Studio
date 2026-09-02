#include "ErrorCorrelationWidget.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>

ErrorCorrelationWidget::ErrorCorrelationWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
}

void ErrorCorrelationWidget::buildUi() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    summaryLabel_ = new QLabel(tr("No correlation data"));
    layout->addWidget(summaryLabel_);

    correlationTable_ = new QTableWidget;
    correlationTable_->setColumnCount(4);
    correlationTable_->setHorizontalHeaderLabels({tr("Type A"), tr("Type B"), tr("Correlation"), tr("Relationship")});
    correlationTable_->horizontalHeader()->setStretchLastSection(true);
    correlationTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    correlationTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(correlationTable_);

    auto* splitter = new QHBoxLayout;

    auto* rcaLabel = new QLabel(tr("Root Cause Analysis"));
    auto* rcaLayout = new QVBoxLayout;
    rcaLayout->addWidget(rcaLabel);
    rootCauseText_ = new QTextEdit;
    rootCauseText_->setReadOnly(true);
    rcaLayout->addWidget(rootCauseText_);
    splitter->addLayout(rcaLayout);

    auto* recLabel = new QLabel(tr("Recommendations"));
    auto* recLayout = new QVBoxLayout;
    recLayout->addWidget(recLabel);
    recommendationsText_ = new QTextEdit;
    recommendationsText_->setReadOnly(true);
    recLayout->addWidget(recommendationsText_);
    splitter->addLayout(recLayout);

    layout->addLayout(splitter);
}

void ErrorCorrelationWidget::setCorrelationData(const QVector<CorrelationDisplayEntry>& entries) {
    correlationTable_->setRowCount(entries.size());
    for (int i = 0; i < entries.size(); ++i) {
        correlationTable_->setItem(i, 0, new QTableWidgetItem(entries[i].typeA));
        correlationTable_->setItem(i, 1, new QTableWidgetItem(entries[i].typeB));
        correlationTable_->setItem(i, 2, new QTableWidgetItem(QString::number(entries[i].value, 'f', 2)));
        correlationTable_->setItem(i, 3, new QTableWidgetItem(entries[i].relationship));
    }
    summaryLabel_->setText(tr("%1 correlation pairs found").arg(entries.size()));
}

void ErrorCorrelationWidget::setRootCause(const RootCauseDisplay& rca) {
    QString text;
    text += tr("Error Type: %1\n").arg(rca.errorType);
    text += tr("Root Cause: %1\n").arg(rca.rootCause);
    text += tr("Confidence: %1%\n\n").arg(rca.confidence * 100, 0, 'f', 1);
    text += tr("Contributing Factors:\n");
    for (const auto& f : rca.factors)
        text += QStringLiteral("  - %1\n").arg(f);
    text += tr("\nRecommended Actions:\n");
    for (const auto& a : rca.actions)
        text += QStringLiteral("  - %1\n").arg(a);
    rootCauseText_->setText(text);
}

void ErrorCorrelationWidget::setRecommendations(const QStringList& recommendations) {
    QString text;
    for (int i = 0; i < recommendations.size(); ++i)
        text += QStringLiteral("%1. %2\n").arg(i + 1).arg(recommendations[i]);
    recommendationsText_->setText(text);
}

void ErrorCorrelationWidget::clear() {
    correlationTable_->setRowCount(0);
    rootCauseText_->clear();
    recommendationsText_->clear();
    summaryLabel_->setText(tr("No correlation data"));
}

QTableWidget* ErrorCorrelationWidget::correlationTable() const {
    return correlationTable_;
}

QTextEdit* ErrorCorrelationWidget::rootCauseText() const {
    return rootCauseText_;
}

QTextEdit* ErrorCorrelationWidget::recommendationsText() const {
    return recommendationsText_;
}
