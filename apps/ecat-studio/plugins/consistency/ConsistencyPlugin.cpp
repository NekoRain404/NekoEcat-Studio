#include "ConsistencyPlugin.h"
#include "adapters/ConsistencyTableAdapter.h"
#include "services/ServiceContainer.h"

#include <QComboBox>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QVBoxLayout>

ConsistencyPlugin::ConsistencyPlugin(ServiceContainer* container, QObject* parent) : container_(container) {
    if (parent)
        setParent(parent);
    buildUi();
}

QString ConsistencyPlugin::id() const {
    return "consistency";
}
QString ConsistencyPlugin::displayName() const {
    return "Consistency";
}
QString ConsistencyPlugin::displayNameZh() const {
    return QStringLiteral("一致性");
}
QIcon ConsistencyPlugin::icon() const {
    return QIcon::fromTheme("dialog-ok-apply");
}
int ConsistencyPlugin::defaultOrder() const {
    return 67;
}
bool ConsistencyPlugin::visible() const {
    return true;
}

void ConsistencyPlugin::activate() {}
void ConsistencyPlugin::deactivate() {}
void ConsistencyPlugin::onSettingsChanged(const AppSettings&) {}
void ConsistencyPlugin::onConnectionChanged(bool) {}

QWidget* ConsistencyPlugin::widget() {
    return containerWidget_;
}

QTableWidget* ConsistencyPlugin::consistencyTable() const {
    return table_;
}
QLineEdit* ConsistencyPlugin::consistencyFilter() const {
    return filter_;
}
QComboBox* ConsistencyPlugin::consistencyScopeFilter() const {
    return scopeFilter_;
}
QLabel* ConsistencyPlugin::consistencySummaryLabel() const {
    return summaryLabel_;
}

void ConsistencyPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* layout = new QVBoxLayout(containerWidget_);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* filterRow = new QHBoxLayout;
    scopeFilter_ = new QComboBox;
    scopeFilter_->addItem(tr("All"), QString(kConsistencyScopeAll));
    scopeFilter_->addItem(tr("Errors"), QString(kConsistencyScopeError));
    scopeFilter_->addItem(tr("Warnings"), QString(kConsistencyScopeWarning));
    scopeFilter_->addItem(tr("Topology"), QString(kConsistencyScopeTopology));
    scopeFilter_->addItem(tr("Startup"), QString(kConsistencyScopeStartup));
    scopeFilter_->addItem(tr("I/O"), QString(kConsistencyScopeIo));
    scopeFilter_->addItem(tr("Ready"), QString(kConsistencyScopeReady));
    filterRow->addWidget(scopeFilter_);

    filter_ = new QLineEdit;
    filter_->setPlaceholderText(tr("Filter consistency rows..."));
    filter_->setClearButtonEnabled(true);
    filterRow->addWidget(filter_);

    summaryLabel_ = new QLabel;
    filterRow->addWidget(summaryLabel_);

    layout->addLayout(filterRow);

    table_ = new QTableWidget;
    table_->setColumnCount(7);
    table_->setHorizontalHeaderLabels(
        {tr("Level"), tr("Scope"), tr("Target"), tr("Evidence"), tr("Expected"), tr("Actual"), tr("Action")});
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(table_);

    connect(filter_, &QLineEdit::textChanged, this, [this]() {
        const QString scope = scopeFilter_ ? scopeFilter_->currentData().toString() : QStringLiteral("all");
        filterConsistencyTableRows(table_, scope, filter_->text().trimmed());
    });
    connect(scopeFilter_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
        const QString scope = scopeFilter_->currentData().toString();
        filterConsistencyTableRows(table_, scope, filter_->text().trimmed());
    });
}

void ConsistencyPlugin::updateConsistencyView(const QList<QStringList>& rows) {
    const QStringList headers = {tr("Level"),    tr("Scope"),  tr("Target"), tr("Evidence"),
                                 tr("Expected"), tr("Actual"), tr("Action")};
    table_->setUpdatesEnabled(false);
    table_->clear();
    table_->setColumnCount(headers.size());
    table_->setHorizontalHeaderLabels(headers);
    table_->setRowCount(rows.size());
    for (int r = 0; r < rows.size(); ++r) {
        for (int c = 0; c < headers.size(); ++c) {
            table_->setItem(r, c, new QTableWidgetItem(rows[r].value(c)));
        }
    }
    table_->resizeColumnsToContents();
    table_->setUpdatesEnabled(true);

    for (int r = 0; r < table_->rowCount(); ++r) {
        const auto state = consistencyTableRowState(table_, r);
        const ConsistencyIssueLevel level = consistencyIssueLevelFromText(state.level);
        QColor foreground;
        switch (level) {
            case ConsistencyIssueLevel::Error:
                foreground = QColor("#ef4444");
                break;
            case ConsistencyIssueLevel::Warning:
                foreground = QColor("#f59e0b");
                break;
            case ConsistencyIssueLevel::Ready:
                foreground = QColor("#22c55e");
                break;
            case ConsistencyIssueLevel::Info:
            case ConsistencyIssueLevel::Empty:
                foreground = QColor("#60a5fa");
                break;
        }
        if (foreground.isValid()) {
            if (auto* item = table_->item(r, 0)) {
                item->setForeground(foreground);
            }
        }
    }

    const ConsistencyIssueCounts counts = consistencyTableIssueCounts(table_);
    if (summaryLabel_) {
        summaryLabel_->setText(tr("%1 rows | errors %2 | warnings %3 | info %4 | "
                                  "ready %5")
                                   .arg(table_->rowCount())
                                   .arg(counts.errors)
                                   .arg(counts.warnings)
                                   .arg(counts.infos)
                                   .arg(counts.ready));
    }
}

ConsistencyIssueCounts ConsistencyPlugin::consistencyIssueCounts() const {
    return consistencyTableIssueCounts(table_);
}
