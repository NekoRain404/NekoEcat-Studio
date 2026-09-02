#include "DiagnosticsPlugin.h"
#include "services/ServiceContainer.h"

#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

DiagnosticsPlugin::DiagnosticsPlugin(ServiceContainer* container, QObject* parent) : container_(container) {
    if (parent)
        setParent(parent);
    buildUi();
}

QString DiagnosticsPlugin::id() const {
    return "diagnostics";
}
QString DiagnosticsPlugin::displayName() const {
    return "Diagnostics";
}
QString DiagnosticsPlugin::displayNameZh() const {
    return QStringLiteral("诊断");
}
QIcon DiagnosticsPlugin::icon() const {
    return QIcon::fromTheme("dialog-warning");
}
int DiagnosticsPlugin::defaultOrder() const {
    return 70;
}
bool DiagnosticsPlugin::visible() const {
    return true;
}

void DiagnosticsPlugin::activate() {}
void DiagnosticsPlugin::deactivate() {}
void DiagnosticsPlugin::onSettingsChanged(const AppSettings&) {}
void DiagnosticsPlugin::onConnectionChanged(bool) {}

QWidget* DiagnosticsPlugin::widget() {
    return containerWidget_;
}

QTableWidget* DiagnosticsPlugin::diagnosticsTable() const {
    return table_;
}
QLineEdit* DiagnosticsPlugin::diagnosticsFilter() const {
    return filter_;
}
QComboBox* DiagnosticsPlugin::diagnosticsLevelFilter() const {
    return levelFilter_;
}
QLabel* DiagnosticsPlugin::diagnosticsSummaryLabel() const {
    return summaryLabel_;
}
QLabel* DiagnosticsPlugin::topologyBaselineLabel() const {
    return baselineLabel_;
}
QPushButton* DiagnosticsPlugin::captureBaselineButton() const {
    return captureBtn_;
}
QPushButton* DiagnosticsPlugin::clearBaselineButton() const {
    return clearBtn_;
}

void DiagnosticsPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* layout = new QVBoxLayout(containerWidget_);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* filterRow = new QHBoxLayout;
    levelFilter_ = new QComboBox;
    levelFilter_->addItem(tr("All"), QString());
    levelFilter_->addItem(tr("Error"), QStringLiteral("Error"));
    levelFilter_->addItem(tr("Warning"), QStringLiteral("Warning"));
    levelFilter_->addItem(tr("Info"), QStringLiteral("Info"));
    filterRow->addWidget(levelFilter_);

    filter_ = new QLineEdit;
    filter_->setPlaceholderText(tr("Filter diagnostics..."));
    filter_->setClearButtonEnabled(true);
    filterRow->addWidget(filter_);

    summaryLabel_ = new QLabel;
    filterRow->addWidget(summaryLabel_);

    layout->addLayout(filterRow);

    table_ = new QTableWidget;
    table_->setColumnCount(3);
    table_->setHorizontalHeaderLabels({tr("Timestamp"), tr("Level"), tr("Message")});
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(table_);

    auto* baselineRow = new QHBoxLayout;
    baselineLabel_ = new QLabel(tr("No topology baseline"));
    baselineRow->addWidget(baselineLabel_);

    captureBtn_ = new QPushButton(tr("Capture Baseline"));
    baselineRow->addWidget(captureBtn_);

    clearBtn_ = new QPushButton(tr("Clear Baseline"));
    baselineRow->addWidget(clearBtn_);

    layout->addLayout(baselineRow);

    connect(filter_, &QLineEdit::textChanged, this, &DiagnosticsPlugin::filterDiagnosticsTable);
    connect(levelFilter_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &DiagnosticsPlugin::filterDiagnosticsTable);
}

void DiagnosticsPlugin::filterDiagnosticsTable() {
    if (!table_)
        return;
    const QString needle = filter_ ? filter_->text().trimmed() : QString();
    const QString level = levelFilter_ ? levelFilter_->currentData().toString() : QString();

    for (int row = 0; row < table_->rowCount(); ++row) {
        const QString rowLevel = table_->item(row, 1) ? table_->item(row, 1)->text() : QString();
        bool match = level.isEmpty() || rowLevel == level;
        if (match && !needle.isEmpty()) {
            match = false;
            for (int col = 0; col < table_->columnCount() && !match; ++col) {
                const auto* item = table_->item(row, col);
                match = item && item->text().contains(needle, Qt::CaseInsensitive);
            }
        }
        table_->setRowHidden(row, !match);
    }
    updateDiagnosticsSummary();
}

void DiagnosticsPlugin::updateDiagnosticsSummary() {
    if (!summaryLabel_ || !table_)
        return;
    int visible = 0;
    int errors = 0;
    int warnings = 0;
    for (int row = 0; row < table_->rowCount(); ++row) {
        if (!table_->isRowHidden(row))
            ++visible;
        const QString level = table_->item(row, 1) ? table_->item(row, 1)->text() : QString();
        if (level == "Error")
            ++errors;
        else if (level == "Warning")
            ++warnings;
    }
    summaryLabel_->setText(tr("%1 shown | %2 errors | %3 warnings").arg(visible).arg(errors).arg(warnings));
}

void DiagnosticsPlugin::exportDiagnosticsReport(QWidget* parentWidget) {
    const QString path = QFileDialog::getSaveFileName(parentWidget, tr("Export Diagnostics Report"), QString(),
                                                      "Markdown (*.md);;Text (*.txt)");
    if (path.isEmpty())
        return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << "# Diagnostics Events\n\n";
    out << "| " << tr("Timestamp") << " | " << tr("Level") << " | " << tr("Message") << " |\n";
    out << "| --- | --- | --- |\n";
    for (int row = 0; row < table_->rowCount(); ++row) {
        const QString ts = table_->item(row, 0) ? table_->item(row, 0)->text() : QString();
        const QString lvl = table_->item(row, 1) ? table_->item(row, 1)->text() : QString();
        const QString msg = table_->item(row, 2) ? table_->item(row, 2)->text() : QString();
        out << "| " << ts << " | " << lvl << " | " << msg << " |\n";
    }
}
