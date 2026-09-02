// AlEventPlugin — implementation.  See header for interface documentation.

#include "AlEventPlugin.h"
#include "services/AlEventService.h"
#include "services/EventBus.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonObject>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

static constexpr int kColTime = 0;
static constexpr int kColSlave = 1;
static constexpr int kColName = 2;
static constexpr int kColCode = 3;
static constexpr int kColSeverity = 4;
static constexpr int kColDescription = 5;
static constexpr int kColCount = 6;

AlEventPlugin::AlEventPlugin(EventBus* bus, AlEventService* service, QObject* parent) : bus_(bus), service_(service) {
    if (parent)
        setParent(parent);
    buildUi();

    // Live updates arrive through the service (wired by MainWindow).
    connect(service_, &AlEventService::alEventUpdate, this, &AlEventPlugin::handleAlEventUpdate);
}

// ── Identity ──────────────────────────────────────────────────────────────
QString AlEventPlugin::id() const {
    return "alevent";
}
QString AlEventPlugin::displayName() const {
    return "AL Events";
}
QString AlEventPlugin::displayNameZh() const {
    return QStringLiteral("AL事件");
}
int AlEventPlugin::defaultOrder() const {
    return 65;
}
bool AlEventPlugin::visible() const {
    return true;
}

QWidget* AlEventPlugin::widget() {
    return container_;
}

// ── UI construction ───────────────────────────────────────────────────────
void AlEventPlugin::buildUi() {
    container_ = new QWidget;
    auto* rootLayout = new QVBoxLayout(container_);
    rootLayout->setContentsMargins(0, 0, 0, 0);

    // Toolbar row: severity filter + clear button.
    auto* toolbar = new QWidget;
    auto* toolbarLayout = new QHBoxLayout(toolbar);
    toolbarLayout->setContentsMargins(4, 2, 4, 2);

    filterCombo_ = new QComboBox;
    filterCombo_->addItems({tr("All"), tr("Error"), tr("Warning"), tr("Info")});
    toolbarLayout->addWidget(filterCombo_);

    auto* clearBtn = new QPushButton(tr("Clear"));
    toolbarLayout->addWidget(clearBtn);
    toolbarLayout->addStretch();

    rootLayout->addWidget(toolbar);

    // Event table.
    table_ = new QTableWidget;
    table_->setColumnCount(kColCount);
    table_->setHorizontalHeaderLabels(
        {tr("Time"), tr("Slave"), tr("Name"), tr("Code"), tr("Severity"), tr("Description")});
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSortingEnabled(false);

    rootLayout->addWidget(table_);

    // Signal connections.
    connect(filterCombo_, &QComboBox::currentIndexChanged, this, &AlEventPlugin::applySeverityFilter);
    connect(clearBtn, &QPushButton::clicked, service_, &AlEventService::clearEvents);
}

// ── Update handler ────────────────────────────────────────────────────────
void AlEventPlugin::handleAlEventUpdate(const QJsonObject& data) {
    populateTable(data);
    // Auto-scroll to the latest entry.
    table_->scrollToBottom();
}

void AlEventPlugin::populateTable(const QJsonObject& data) {
    const QJsonArray events = data.value("events").toArray();
    const int existingRows = table_->rowCount();
    table_->setRowCount(existingRows + events.size());

    for (int i = 0; i < events.size(); ++i) {
        const QJsonObject ev = events.at(i).toObject();
        const int row = existingRows + i;

        table_->setItem(row, kColTime, new QTableWidgetItem(ev.value("timestamp").toString()));
        table_->setItem(row, kColSlave, new QTableWidgetItem(ev.value("slavePosition").toString()));
        table_->setItem(row, kColName, new QTableWidgetItem(ev.value("name").toString()));
        table_->setItem(row, kColCode, new QTableWidgetItem(ev.value("code").toString()));
        table_->setItem(row, kColSeverity, new QTableWidgetItem(ev.value("severity").toString()));
        table_->setItem(row, kColDescription, new QTableWidgetItem(ev.value("description").toString()));
    }

    updateFilterVisibility();
}

// ── Severity filter ───────────────────────────────────────────────────────
void AlEventPlugin::applySeverityFilter() {
    updateFilterVisibility();
}

void AlEventPlugin::updateFilterVisibility() {
    const QString filter = filterCombo_->currentText();
    for (int r = 0; r < table_->rowCount(); ++r) {
        auto* sevItem = table_->item(r, kColSeverity);
        const bool match = filter == tr("All") || (sevItem && sevItem->text() == filter);
        table_->setRowHidden(r, !match);
    }
}
