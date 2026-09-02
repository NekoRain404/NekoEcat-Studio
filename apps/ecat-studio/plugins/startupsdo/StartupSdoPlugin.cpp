#include "StartupSdoPlugin.h"
#include "services/ServiceContainer.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

StartupSdoPlugin::StartupSdoPlugin(ServiceContainer* container, QObject* parent) : container_(container) {
    if (parent)
        setParent(parent);
    buildUi();
}

// ── Identity ──────────────────────────────────────────────────────────
QString StartupSdoPlugin::id() const {
    return "startupsdo";
}
QString StartupSdoPlugin::displayName() const {
    return "Startup SDO";
}
QString StartupSdoPlugin::displayNameZh() const {
    return QStringLiteral("启动SDO");
}
QIcon StartupSdoPlugin::icon() const {
    return QIcon::fromTheme("system-run");
}
int StartupSdoPlugin::defaultOrder() const {
    return 35;
}
bool StartupSdoPlugin::visible() const {
    return true;
}

void StartupSdoPlugin::activate() {}
void StartupSdoPlugin::deactivate() {}
void StartupSdoPlugin::onSettingsChanged(const AppSettings&) {}
void StartupSdoPlugin::onConnectionChanged(bool) {}

QWidget* StartupSdoPlugin::widget() {
    return containerWidget_;
}

// ── Accessors ─────────────────────────────────────────────────────────
QTableWidget* StartupSdoPlugin::startupSdoTable() const {
    return startupSdoTable_;
}

QCheckBox* StartupSdoPlugin::startupWatchDiffsOnly() const {
    return startupWatchDiffsOnly_;
}

QLabel* StartupSdoPlugin::startupWatchSummaryLabel() const {
    return startupWatchSummaryLabel_;
}

QLabel* StartupSdoPlugin::startupSdoDetailLabel() const {
    return startupSdoDetailLabel_;
}

// ── UI construction ───────────────────────────────────────────────────
void StartupSdoPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* mainLayout = new QVBoxLayout(containerWidget_);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto* toolbarWidget = new QWidget;
    buildToolbar(toolbarWidget);
    mainLayout->addWidget(toolbarWidget);

    auto* tableWidget = new QWidget;
    buildTable(tableWidget);
    mainLayout->addWidget(tableWidget, 1);

    connect(startupSdoTable_, &QTableWidget::currentCellChanged, this,
            &StartupSdoPlugin::startupSdoTableSelectionChanged);
}

void StartupSdoPlugin::buildToolbar(QWidget* parent) {
    auto* layout = new QHBoxLayout(parent);
    layout->setContentsMargins(4, 2, 4, 2);

    startupWatchDiffsOnly_ = new QCheckBox(tr("Diffs Only"));
    layout->addWidget(startupWatchDiffsOnly_);

    startupWatchSummaryLabel_ = new QLabel;
    layout->addWidget(startupWatchSummaryLabel_);

    layout->addStretch();

    startupSdoDetailLabel_ = new QLabel;
    startupSdoDetailLabel_->setWordWrap(true);
    layout->addWidget(startupSdoDetailLabel_);
}

void StartupSdoPlugin::buildTable(QWidget* parent) {
    auto* layout = new QVBoxLayout(parent);
    layout->setContentsMargins(0, 0, 0, 0);

    startupSdoTable_ = new QTableWidget;
    startupSdoTable_->setColumnCount(9);
    startupSdoTable_->setHorizontalHeaderLabels({tr("Slave"), tr("Index"), tr("Sub"), tr("Value"), tr("Type"),
                                                 tr("Status"), tr("Detail"), tr("Watch Value"), tr("Watch Delta")});
    startupSdoTable_->horizontalHeader()->setStretchLastSection(true);
    startupSdoTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    startupSdoTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    startupSdoTable_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    layout->addWidget(startupSdoTable_);
}

// ── Table Management ──────────────────────────────────────────────────
void StartupSdoPlugin::ensureStartupSdoTable() {
    if (!startupSdoTable_)
        return;
    if (startupSdoTable_->columnCount() != 9) {
        startupSdoTable_->setColumnCount(9);
    }
    startupSdoTable_->setHorizontalHeaderLabels({tr("Slave"), tr("Index"), tr("Sub"), tr("Value"), tr("Type"),
                                                 tr("Status"), tr("Detail"), tr("Watch Value"), tr("Watch Delta")});
    for (int row = 0; row < startupSdoTable_->rowCount(); ++row) {
        for (int col = 0; col < startupSdoTable_->columnCount(); ++col) {
            if (!startupSdoTable_->item(row, col)) {
                startupSdoTable_->setItem(row, col, new QTableWidgetItem);
            }
        }
    }
}

void StartupSdoPlugin::updateStartupSdoControls(bool connected) {
    if (!startupSdoTable_)
        return;
    const int rows = startupSdoTable_->rowCount();
    const int row = startupSdoTable_->currentRow();
    const bool hasCurrentVisibleRow = row >= 0 && row < rows && !startupSdoTable_->isRowHidden(row);

    QVector<int> selectedRows;
    for (int r = 0; r < rows; ++r) {
        if (startupSdoTable_->selectionModel()->isRowSelected(r, QModelIndex())) {
            selectedRows.append(r);
        }
    }
    const bool hasSelectedRows = !selectedRows.isEmpty();

    auto setEnabled = [this](const char* name, bool enabled) {
        if (auto* button = containerWidget_->findChild<QPushButton*>(name)) {
            button->setEnabled(enabled);
        }
    };
    setEnabled("removeStartupSdo", hasSelectedRows);
    setEnabled("moveStartupSdoUp", hasCurrentVisibleRow && row > 0);
    setEnabled("moveStartupSdoDown", hasCurrentVisibleRow && row < rows - 1);
    setEnabled("preflightStartupSdo", rows > 0);
    setEnabled("verifyStartupSdo", connected && rows > 0);
    setEnabled("verifySelectedStartupSdo", connected && hasSelectedRows);
    setEnabled("applyStartupSdo", connected && rows > 0);
    setEnabled("applySelectedStartupSdo", connected && hasSelectedRows);

    if (startupWatchDiffsOnly_) {
        startupWatchDiffsOnly_->setEnabled(rows > 0);
    }
}

void StartupSdoPlugin::filterStartupSdoTable(bool diffsOnly) {
    if (!startupSdoTable_)
        return;
    // The actual filtering logic remains in MainWindow since it depends on
    // watch data. This method is called by MainWindow after filtering.
    Q_UNUSED(diffsOnly);
}
