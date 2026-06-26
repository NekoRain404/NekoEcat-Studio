#include "EsiBrowserPlugin.h"
#include "EsiParser.h"
#include "EsiDeviceMatcher.h"
#include "services/EsiService.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTabWidget>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QDir>
#include <QFileInfo>

EsiBrowserPlugin::EsiBrowserPlugin(EsiService *esiService, QObject *parent)
    : service_(esiService)
    , parser_(new EsiParser(this))
    , matcher_(new EsiDeviceMatcher(esiService, this))
{
    if (parent) setParent(parent);
    buildUi();

    connect(service_, &EsiService::esiImported, this,
            [this](int) { updateDeviceList(); });
    connect(parser_, &EsiParser::parseComplete, this,
            [this](int) { updateDeviceList(); });
    connect(matcher_, &EsiDeviceMatcher::matchFound, this,
            [this](const QString &, int, int) {
                matchReport_->append(tr("Match found"));
            });
}

QString EsiBrowserPlugin::id() const { return "esibrowser"; }
QString EsiBrowserPlugin::displayName() const { return "ESI Browser"; }
QString EsiBrowserPlugin::displayNameZh() const { return QStringLiteral("ESI 浏览器"); }
QIcon EsiBrowserPlugin::icon() const { return QIcon::fromTheme("document-properties"); }
int EsiBrowserPlugin::defaultOrder() const { return 22; }
bool EsiBrowserPlugin::visible() const { return true; }

void EsiBrowserPlugin::activate() {}
void EsiBrowserPlugin::deactivate() {}

void EsiBrowserPlugin::onConnectionChanged(bool connected) {
    if (connected) {
        summaryLabel_->setText(tr("Connected — ESI auto-match available"));
    } else {
        summaryLabel_->setText(tr("Disconnected — ESI browse only"));
    }
}

QWidget *EsiBrowserPlugin::widget() { return containerWidget_; }

void EsiBrowserPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto *layout = new QVBoxLayout(containerWidget_);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(10);

    auto *toolbar = new QHBoxLayout;
    toolbar->setSpacing(8);

    filterEdit_ = new QLineEdit;
    filterEdit_->setPlaceholderText(tr("Filter devices..."));
    filterEdit_->setClearButtonEnabled(true);
    toolbar->addWidget(filterEdit_);

    importBtn_ = new QPushButton(tr("Import ESI"));
    toolbar->addWidget(importBtn_);

    importDirBtn_ = new QPushButton(tr("Import Dir"));
    toolbar->addWidget(importDirBtn_);

    exportBtn_ = new QPushButton(tr("Export"));
    exportBtn_->setEnabled(false);
    toolbar->addWidget(exportBtn_);

    refreshBtn_ = new QPushButton(tr("Refresh"));
    toolbar->addWidget(refreshBtn_);

    matchBtn_ = new QPushButton(tr("Auto Match"));
    matchBtn_->setEnabled(false);
    toolbar->addWidget(matchBtn_);

    layout->addLayout(toolbar);

    mainSplitter_ = new QSplitter(Qt::Horizontal);

    esiTree_ = new QTreeWidget;
    esiTree_->setHeaderLabel(tr("ESI Devices"));
    esiTree_->setMinimumWidth(240);
    mainSplitter_->addWidget(esiTree_);

    rightSplitter_ = new QSplitter(Qt::Vertical);

    detailTabs_ = new QTabWidget;

    buildDetailPanel();
    buildPdoPanel();
    buildSyncManagerPanel();

    matchReport_ = new QTextEdit;
    matchReport_->setReadOnly(true);
    matchReport_->setPlaceholderText(tr("Auto-match report will appear here..."));

    rightSplitter_->addWidget(detailTabs_);
    rightSplitter_->addWidget(matchReport_);
    rightSplitter_->setStretchFactor(0, 3);
    rightSplitter_->setStretchFactor(1, 1);

    mainSplitter_->addWidget(rightSplitter_);
    mainSplitter_->setStretchFactor(0, 1);
    mainSplitter_->setStretchFactor(1, 2);

    layout->addWidget(mainSplitter_, 1);

    summaryLabel_ = new QLabel(tr("No ESI files loaded"));
    layout->addWidget(summaryLabel_);

    connect(importBtn_, &QPushButton::clicked, this, &EsiBrowserPlugin::importFile);
    connect(importDirBtn_, &QPushButton::clicked, this, &EsiBrowserPlugin::importDirectory);
    connect(exportBtn_, &QPushButton::clicked, this, &EsiBrowserPlugin::exportSelected);
    connect(refreshBtn_, &QPushButton::clicked, this, &EsiBrowserPlugin::refreshList);
    connect(matchBtn_, &QPushButton::clicked, this, &EsiBrowserPlugin::autoMatchDevices);
    connect(esiTree_, &QTreeWidget::itemSelectionChanged,
            this, &EsiBrowserPlugin::onTreeSelectionChanged);
    connect(filterEdit_, &QLineEdit::textChanged, this,
            [this](const QString &) { updateTreeFilter(filterEdit_->text()); });
}

void EsiBrowserPlugin::buildDetailPanel() {
    detailTable_ = new QTableWidget;
    detailTable_->setColumnCount(2);
    detailTable_->setHorizontalHeaderLabels({tr("Property"), tr("Value")});
    detailTable_->horizontalHeader()->setStretchLastSection(true);
    detailTable_->verticalHeader()->setVisible(false);
    detailTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    detailTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    detailTable_->setShowGrid(false);
    detailTable_->setAlternatingRowColors(true);
    detailTabs_->addTab(detailTable_, tr("Device Details"));
}

void EsiBrowserPlugin::buildPdoPanel() {
    pdoTable_ = new QTableWidget;
    pdoTable_->setColumnCount(5);
    pdoTable_->setHorizontalHeaderLabels({
        tr("PDO Index"), tr("PDO Name"), tr("Entry Index"),
        tr("Entry Name"), tr("Bit Size")
    });
    pdoTable_->horizontalHeader()->setStretchLastSection(true);
    pdoTable_->verticalHeader()->setVisible(false);
    pdoTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    pdoTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    pdoTable_->setShowGrid(false);
    pdoTable_->setAlternatingRowColors(true);
    detailTabs_->addTab(pdoTable_, tr("PDO Mapping"));
}

void EsiBrowserPlugin::buildSyncManagerPanel() {
    syncManagerTable_ = new QTableWidget;
    syncManagerTable_->setColumnCount(4);
    syncManagerTable_->setHorizontalHeaderLabels({
        tr("SM Index"), tr("Name"), tr("Direction"), tr("PDO Count")
    });
    syncManagerTable_->horizontalHeader()->setStretchLastSection(true);
    syncManagerTable_->verticalHeader()->setVisible(false);
    syncManagerTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    syncManagerTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    syncManagerTable_->setShowGrid(false);
    syncManagerTable_->setAlternatingRowColors(true);
    detailTabs_->addTab(syncManagerTable_, tr("Sync Managers"));
}

void EsiBrowserPlugin::importFile() {
    const QStringList paths = QFileDialog::getOpenFileNames(
        containerWidget_, tr("Import ESI XML"), QString(),
        "ESI XML (*.xml);;All Files (*)");
    if (paths.isEmpty()) return;
    for (const QString &path : paths) {
        service_->importEsi(path);
    }
    updateDeviceList();
}

void EsiBrowserPlugin::importDirectory() {
    const QString dir = QFileDialog::getExistingDirectory(
        containerWidget_, tr("Import ESI Directory"));
    if (dir.isEmpty()) return;

    QDir directory(dir);
    const QStringList xmlFiles = directory.entryList(
        {"*.xml"}, QDir::Files, QDir::Name);
    for (const QString &file : xmlFiles) {
        service_->importEsi(directory.absoluteFilePath(file));
    }
    updateDeviceList();
}

void EsiBrowserPlugin::refreshList() {
    updateDeviceList();
}

void EsiBrowserPlugin::exportSelected() {
    QTreeWidgetItem *item = esiTree_->currentItem();
    if (!item) return;
    QString deviceId = item->data(0, Qt::UserRole).toString();
    if (deviceId.isEmpty()) return;

    const QString path = QFileDialog::getSaveFileName(
        containerWidget_, tr("Export ESI XML"), QString(),
        "ESI XML (*.xml)");
    if (!path.isEmpty())
        service_->exportEsi(deviceId, path);
}

void EsiBrowserPlugin::autoMatchDevices() {
    matchReport_->clear();
    matchReport_->append(tr("=== ESI Auto-Match Report ===\n"));
    matchReport_->append(tr("Checking connected devices against ESI repository...\n"));

    auto devices = service_->listDevices();
    QVector<QPair<int, int>> connected;
    for (const auto &dev : devices) {
        connected.append({dev.vendorId, dev.productCode});
    }

    EsiDeviceMatcher::MatchReport report = matcher_->generateReport(connected);

    matchReport_->append(tr("Total devices: %1").arg(report.totalDevices));
    matchReport_->append(tr("Matched: %1").arg(report.matchedDevices));
    matchReport_->append(tr("Unmatched: %1\n").arg(report.unmatchedDevices));

    for (const auto &result : report.results) {
        QString status = result.matched ? tr("MATCH") : tr("NO MATCH");
        matchReport_->append(QStringLiteral("[%1] Vendor=0x%2 Product=0x%3 — %4")
            .arg(status)
            .arg(result.vendorId, 8, 16, QChar('0'))
            .arg(result.productCode, 8, 16, QChar('0'))
            .arg(result.esiDeviceName.isEmpty() ? tr("(unknown)") : result.esiDeviceName));

        if (!result.differences.isEmpty()) {
            for (const auto &diff : result.differences) {
                matchReport_->append(QStringLiteral("  ⚠ %1").arg(diff));
            }
        }
    }
}

void EsiBrowserPlugin::updateDeviceList() {
    esiTree_->clear();
    currentDevices_ = service_->listDevices();

    QTreeWidgetItem *vendorRoot = nullptr;
    QString lastVendor;
    int count = 0;

    for (int i = 0; i < currentDevices_.size(); ++i) {
        const auto &d = currentDevices_[i];
        QString vendorKey = QStringLiteral("0x%1").arg(d.vendorId, 8, 16, QChar('0'));

        if (vendorKey != lastVendor) {
            vendorRoot = new QTreeWidgetItem(esiTree_);
            vendorRoot->setText(0, vendorKey);
            vendorRoot->setExpanded(true);
            lastVendor = vendorKey;
        }

        if (vendorRoot) {
            auto *devItem = new QTreeWidgetItem(vendorRoot);
            devItem->setText(0, QStringLiteral("%1 (%2)").arg(d.name, d.type));
            devItem->setData(0, Qt::UserRole, i);
            devItem->setData(0, Qt::UserRole + 1, d.deviceId);
            ++count;
        }
    }

    exportBtn_->setEnabled(count > 0);
    matchBtn_->setEnabled(count > 0);
    summaryLabel_->setText(tr("%n device(s) in repository", nullptr, count));
}

void EsiBrowserPlugin::showDeviceDetail(int index) {
    detailTable_->setRowCount(0);
    pdoTable_->setRowCount(0);
    syncManagerTable_->setRowCount(0);

    if (index < 0 || index >= currentDevices_.size()) return;

    const EsiDeviceInfo &d = currentDevices_[index];
    currentDeviceIndex_ = index;

    auto addRow = [&](const QString &prop, const QString &val) {
        int r = detailTable_->rowCount();
        detailTable_->insertRow(r);
        detailTable_->setItem(r, 0, new QTableWidgetItem(prop));
        detailTable_->setItem(r, 1, new QTableWidgetItem(val));
    };

    addRow(tr("Name"), d.name);
    addRow(tr("Type"), d.type);
    addRow(tr("Vendor ID"),
           QStringLiteral("0x%1").arg(d.vendorId, 8, 16, QChar('0')));
    addRow(tr("Product Code"),
           QStringLiteral("0x%1").arg(d.productCode, 8, 16, QChar('0')));
    addRow(tr("Revision"),
           QStringLiteral("0x%1").arg(d.revisionNo, 8, 16, QChar('0')));
    addRow(tr("Description"), d.description);
    addRow(tr("Device ID"), d.deviceId);
    addRow(tr("Rx PDOs"), QString::number(d.rxPdos.size()));
    addRow(tr("Tx PDOs"), QString::number(d.txPdos.size()));
    addRow(tr("Sync Managers"), QString::number(d.syncManagers.size()));

    showPdoMapping(d);
    showSyncManagerConfig(d);
}

void EsiBrowserPlugin::showPdoMapping(const EsiDeviceInfo &dev) {
    pdoTable_->setRowCount(0);

    auto addPdoEntries = [&](const QString &dir, const QVector<EsiPdoAssignment> &pdos) {
        for (const auto &pdo : pdos) {
            for (const auto &entry : pdo.entries) {
                int r = pdoTable_->rowCount();
                pdoTable_->insertRow(r);
                pdoTable_->setItem(r, 0, new QTableWidgetItem(
                    QStringLiteral("0x%1 [%2]").arg(pdo.index, dir)));
                pdoTable_->setItem(r, 1, new QTableWidgetItem(pdo.name));
                pdoTable_->setItem(r, 2, new QTableWidgetItem(
                    QStringLiteral("0x%1:%2").arg(entry.index, entry.subIndex)));
                pdoTable_->setItem(r, 3, new QTableWidgetItem(entry.name));
                pdoTable_->setItem(r, 4, new QTableWidgetItem(
                    QStringLiteral("%1 bit").arg(entry.bitSize)));
            }
        }
    };

    addPdoEntries("Rx", dev.rxPdos);
    addPdoEntries("Tx", dev.txPdos);
}

void EsiBrowserPlugin::showSyncManagerConfig(const EsiDeviceInfo &dev) {
    syncManagerTable_->setRowCount(0);

    for (const auto &sm : dev.syncManagers) {
        int r = syncManagerTable_->rowCount();
        syncManagerTable_->insertRow(r);
        syncManagerTable_->setItem(r, 0, new QTableWidgetItem(
            QString::number(sm.index)));
        syncManagerTable_->setItem(r, 1, new QTableWidgetItem(sm.name));
        syncManagerTable_->setItem(r, 2, new QTableWidgetItem(sm.direction));
        syncManagerTable_->setItem(r, 3, new QTableWidgetItem(
            QString::number(sm.pdos)));
    }
}

void EsiBrowserPlugin::updateTreeFilter(const QString &filter) {
    QString needle = filter.trimmed().toLower();
    for (int i = 0; i < esiTree_->topLevelItemCount(); ++i) {
        QTreeWidgetItem *vendorItem = esiTree_->topLevelItem(i);
        bool vendorVisible = false;
        for (int j = 0; j < vendorItem->childCount(); ++j) {
            QTreeWidgetItem *devItem = vendorItem->child(j);
            bool match = needle.isEmpty() ||
                         devItem->text(0).toLower().contains(needle);
            devItem->setHidden(!match);
            if (match) vendorVisible = true;
        }
        vendorItem->setHidden(!vendorVisible);
    }
}

void EsiBrowserPlugin::onTreeSelectionChanged() {
    QTreeWidgetItem *item = esiTree_->currentItem();
    if (!item) {
        currentDeviceIndex_ = -1;
        return;
    }
    int idx = item->data(0, Qt::UserRole).toInt();
    if (idx >= 0) {
        showDeviceDetail(idx);
    }
}
