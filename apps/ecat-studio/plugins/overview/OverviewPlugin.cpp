#include "OverviewPlugin.h"
#include "services/ServiceContainer.h"

#include <QGridLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QTableWidget>
#include <QTabWidget>
#include <QVBoxLayout>

OverviewPlugin::OverviewPlugin(ServiceContainer* container, QObject* parent) : container_(container) {
    if (parent)
        setParent(parent);
    buildUi();
}

// ── Identity ──────────────────────────────────────────────────────────
QString OverviewPlugin::id() const {
    return "overview";
}
QString OverviewPlugin::displayName() const {
    return "Overview";
}
QString OverviewPlugin::displayNameZh() const {
    return QStringLiteral("总览");
}
QIcon OverviewPlugin::icon() const {
    return QIcon::fromTheme("view-list-details");
}
int OverviewPlugin::defaultOrder() const {
    return 5;
}
bool OverviewPlugin::visible() const {
    return true;
}

void OverviewPlugin::activate() {}
void OverviewPlugin::deactivate() {}
void OverviewPlugin::onSettingsChanged(const AppSettings&) {}
void OverviewPlugin::onConnectionChanged(bool) {}

QWidget* OverviewPlugin::widget() {
    return containerWidget_;
}

// ── Sub-tab accessors ─────────────────────────────────────────────────
QTabWidget* OverviewPlugin::overviewTabs() const {
    return tabs_;
}
int OverviewPlugin::detailsTabIndex() const {
    return 0;
}
int OverviewPlugin::briefTabIndex() const {
    return 1;
}
int OverviewPlugin::workflowTabIndex() const {
    return 2;
}
int OverviewPlugin::matrixTabIndex() const {
    return 3;
}

// ── Table accessors ───────────────────────────────────────────────────
QTableWidget* OverviewPlugin::metricTable() const {
    return metricTable_;
}
QTableWidget* OverviewPlugin::identityTable() const {
    return identityTable_;
}
QTableWidget* OverviewPlugin::portTable() const {
    return portTable_;
}
QTableWidget* OverviewPlugin::mailboxTable() const {
    return mailboxTable_;
}
QTableWidget* OverviewPlugin::sessionBriefTable() const {
    return sessionBriefTable_;
}
QTableWidget* OverviewPlugin::workflowTable() const {
    return workflowTable_;
}
QTableWidget* OverviewPlugin::slaveEvidenceMatrixTable() const {
    return slaveEvidenceMatrixTable_;
}

// ── UI construction ───────────────────────────────────────────────────
void OverviewPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* layout = new QVBoxLayout(containerWidget_);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(10);

    tabs_ = new QTabWidget;
    tabs_->setObjectName("overviewModeTabs");
    tabs_->setMovable(false);

    auto setupTable = [](QTableWidget* table) {
        table->setAlternatingRowColors(true);
        table->verticalHeader()->setVisible(false);
        table->horizontalHeader()->setStretchLastSection(true);
        table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::ExtendedSelection);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setShowGrid(false);
        table->setWordWrap(false);
        table->setCornerButtonEnabled(false);
        table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        table->verticalHeader()->setDefaultSectionSize(30);
        table->setContextMenuPolicy(Qt::CustomContextMenu);
    };

    // ── Details tab ─────────────────────────────────────────────────────
    auto* detailsPage = new QWidget;
    auto* detailsLayout = new QVBoxLayout(detailsPage);
    detailsLayout->setContentsMargins(10, 10, 10, 10);
    detailsLayout->setSpacing(10);

    auto* overviewGrid = new QGridLayout;
    overviewGrid->setHorizontalSpacing(12);
    overviewGrid->setVerticalSpacing(10);

    metricTable_ = new QTableWidget;
    setupTable(metricTable_);
    identityTable_ = new QTableWidget;
    setupTable(identityTable_);
    portTable_ = new QTableWidget;
    setupTable(portTable_);
    mailboxTable_ = new QTableWidget;
    setupTable(mailboxTable_);

    overviewGrid->addWidget(metricTable_, 0, 0);
    overviewGrid->addWidget(identityTable_, 0, 1);
    overviewGrid->addWidget(portTable_, 1, 0);
    overviewGrid->addWidget(mailboxTable_, 1, 1);
    overviewGrid->setRowStretch(0, 1);
    overviewGrid->setRowStretch(1, 1);
    detailsLayout->addLayout(overviewGrid, 1);

    // ── Brief tab ───────────────────────────────────────────────────────
    auto* briefPage = new QWidget;
    auto* briefLayout = new QVBoxLayout(briefPage);
    briefLayout->setContentsMargins(10, 10, 10, 10);
    briefLayout->setSpacing(10);

    sessionBriefTable_ = new QTableWidget;
    setupTable(sessionBriefTable_);
    sessionBriefTable_->setMinimumHeight(320);
    briefLayout->addWidget(sessionBriefTable_, 1);

    // ── Workflow tab ────────────────────────────────────────────────────
    auto* workflowPage = new QWidget;
    auto* workflowLayout = new QVBoxLayout(workflowPage);
    workflowLayout->setContentsMargins(10, 10, 10, 10);
    workflowLayout->setSpacing(10);

    workflowTable_ = new QTableWidget;
    setupTable(workflowTable_);
    workflowTable_->setMinimumHeight(420);
    workflowLayout->addWidget(workflowTable_, 1);

    // ── Matrix tab ──────────────────────────────────────────────────────
    auto* matrixPage = new QWidget;
    auto* matrixLayout = new QVBoxLayout(matrixPage);
    matrixLayout->setContentsMargins(10, 10, 10, 10);
    matrixLayout->setSpacing(10);

    slaveEvidenceMatrixTable_ = new QTableWidget;
    setupTable(slaveEvidenceMatrixTable_);
    slaveEvidenceMatrixTable_->setMinimumHeight(420);
    matrixLayout->addWidget(slaveEvidenceMatrixTable_, 1);

    // ── Add tabs ────────────────────────────────────────────────────────
    tabs_->addTab(detailsPage, tr("Details"));
    tabs_->addTab(briefPage, tr("Brief"));
    tabs_->addTab(workflowPage, tr("Workflow"));
    tabs_->addTab(matrixPage, tr("Matrix"));

    layout->addWidget(tabs_, 1);
}
