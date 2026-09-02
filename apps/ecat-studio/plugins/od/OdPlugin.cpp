#include "OdPlugin.h"
#include "services/SdoCacheService.h"
#include "services/ServiceContainer.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>

OdPlugin::OdPlugin(ServiceContainer* container, QObject* parent) : container_(container) {
    if (parent)
        setParent(parent);
    buildUi();

    auto* cache = container_->sdoCache();
    connect(cache, &SdoCacheService::cacheUpdated, this, [this](int position) {
        updateSdoInspectorLabel(tr("Dictionary cache updated for slave %1").arg(position), QStringLiteral("ok"));
    });

    connect(cache, &SdoCacheService::cacheInvalidated, this, [this](int position) {
        updateSdoTargetTrailRowDetail(tr("Cache invalidated for slave %1").arg(position), QStringLiteral("warning"),
                                      QString());
    });
}

// ── Identity ──────────────────────────────────────────────────────────
QString OdPlugin::id() const {
    return "od";
}
QString OdPlugin::displayName() const {
    return "Object Dictionary";
}
QString OdPlugin::displayNameZh() const {
    return QStringLiteral("对象字典");
}
QIcon OdPlugin::icon() const {
    return QIcon::fromTheme("text-x-generic");
}
int OdPlugin::defaultOrder() const {
    return 20;
}
bool OdPlugin::visible() const {
    return true;
}

void OdPlugin::activate() {}
void OdPlugin::deactivate() {}
void OdPlugin::onSettingsChanged(const AppSettings&) {}
void OdPlugin::onConnectionChanged(bool) {}

QWidget* OdPlugin::widget() {
    return containerWidget_;
}

// ── Accessors ─────────────────────────────────────────────────────────
QTableWidget* OdPlugin::sdoTable() const {
    return sdoTable_;
}
QLineEdit* OdPlugin::sdoFilter() const {
    return sdoFilter_;
}
QComboBox* OdPlugin::sdoObjectFilter() const {
    return sdoObjectFilter_;
}
QComboBox* OdPlugin::sdoAccessFilter() const {
    return sdoAccessFilter_;
}
QLabel* OdPlugin::sdoSummaryLabel() const {
    return sdoSummaryLabel_;
}

QLineEdit* OdPlugin::sdoIndex() const {
    return sdoIndex_;
}
QLineEdit* OdPlugin::sdoSubIndex() const {
    return sdoSubIndex_;
}
QComboBox* OdPlugin::sdoType() const {
    return sdoType_;
}
QLineEdit* OdPlugin::sdoValue() const {
    return sdoValue_;
}
QLineEdit* OdPlugin::sdoWriteValue() const {
    return sdoWriteValue_;
}
QPushButton* OdPlugin::useSdoValueButton() const {
    return useSdoValueButton_;
}
QLabel* OdPlugin::sdoInspectorLabel() const {
    return sdoInspectorLabel_;
}

QTableWidget* OdPlugin::sdoTargetTable() const {
    return sdoTargetTable_;
}
QTableWidget* OdPlugin::sdoTargetTrailTable() const {
    return sdoTargetTrailTable_;
}
QLabel* OdPlugin::sdoTargetTrailDetailLabel() const {
    return sdoTargetTrailDetailLabel_;
}
QTableWidget* OdPlugin::objectBookmarkTable() const {
    return objectBookmarkTable_;
}
QLabel* OdPlugin::objectBookmarkDetailLabel() const {
    return objectBookmarkDetailLabel_;
}
QTableWidget* OdPlugin::sdoHistoryTable() const {
    return sdoHistoryTable_;
}

// ── UI construction ───────────────────────────────────────────────────
void OdPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* mainLayout = new QVBoxLayout(containerWidget_);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto* splitter = new QSplitter(Qt::Vertical);

    auto* topSplitter = new QSplitter(Qt::Horizontal);

    auto* odWidget = new QWidget;
    buildOdTab(odWidget);
    topSplitter->addWidget(odWidget);

    auto* inspectorWidget = new QWidget;
    buildInspectorPanel(inspectorWidget);
    topSplitter->addWidget(inspectorWidget);

    topSplitter->setStretchFactor(0, 3);
    topSplitter->setStretchFactor(1, 2);

    splitter->addWidget(topSplitter);

    auto* trailWidget = new QWidget;
    buildTargetTrailTab(trailWidget);
    splitter->addWidget(trailWidget);

    auto* bookmarkWidget = new QWidget;
    buildBookmarkTab(bookmarkWidget);
    splitter->addWidget(bookmarkWidget);

    auto* historyWidget = new QWidget;
    buildHistoryTab(historyWidget);
    splitter->addWidget(historyWidget);

    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);

    mainLayout->addWidget(splitter);
}

void OdPlugin::buildOdTab(QWidget* widget) {
    auto* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* filterRow = new QHBoxLayout;
    sdoObjectFilter_ = new QComboBox;
    sdoObjectFilter_->addItem(tr("All Objects"));
    sdoObjectFilter_->addItem(tr("CoE Objects"));
    sdoObjectFilter_->addItem(tr("CiA 402"));
    sdoObjectFilter_->addItem(tr("Identity"));
    sdoObjectFilter_->addItem(tr("PDO Mapping"));
    sdoObjectFilter_->addItem(tr("Error/Diag"));
    filterRow->addWidget(sdoObjectFilter_);

    sdoAccessFilter_ = new QComboBox;
    sdoAccessFilter_->addItem(tr("All Access"));
    sdoAccessFilter_->addItem(tr("Read/Write"));
    sdoAccessFilter_->addItem(tr("Read Only"));
    filterRow->addWidget(sdoAccessFilter_);

    sdoFilter_ = new QLineEdit;
    sdoFilter_->setPlaceholderText(tr("Filter OD rows..."));
    sdoFilter_->setClearButtonEnabled(true);
    filterRow->addWidget(sdoFilter_);

    sdoSummaryLabel_ = new QLabel;
    filterRow->addWidget(sdoSummaryLabel_);

    layout->addLayout(filterRow);

    sdoTable_ = new QTableWidget;
    sdoTable_->setColumnCount(9);
    sdoTable_->setHorizontalHeaderLabels({tr("Object"), tr("Index"), tr("Sub"), tr("Access"), tr("Bits"), tr("Type"),
                                          tr("Name"), tr("Last Value"), tr("Status")});
    sdoTable_->horizontalHeader()->setStretchLastSection(true);
    sdoTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    sdoTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    sdoTable_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    layout->addWidget(sdoTable_);

    connect(sdoFilter_, &QLineEdit::textChanged, this, &OdPlugin::sdoFilterChanged);
    connect(sdoTable_, &QTableWidget::currentCellChanged, this, &OdPlugin::sdoTableSelectionChanged);
}

void OdPlugin::buildInspectorPanel(QWidget* widget) {
    auto* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* addrRow = new QHBoxLayout;
    sdoIndex_ = new QLineEdit;
    sdoIndex_->setPlaceholderText(tr("Index (0x)"));
    sdoIndex_->setMaximumWidth(80);
    addrRow->addWidget(sdoIndex_);

    sdoSubIndex_ = new QLineEdit;
    sdoSubIndex_->setPlaceholderText(tr("Sub"));
    sdoSubIndex_->setMaximumWidth(50);
    addrRow->addWidget(sdoSubIndex_);

    sdoType_ = new QComboBox;
    sdoType_->addItems({"", "bool", "int8", "int16", "int32", "int64", "uint8", "uint16", "uint32", "uint64", "float",
                        "double", "string", "octet_string"});
    sdoType_->setMaximumWidth(100);
    addrRow->addWidget(sdoType_);

    sdoValue_ = new QLineEdit;
    sdoValue_->setReadOnly(true);
    sdoValue_->setObjectName("sdoReadValue");
    addrRow->addWidget(sdoValue_);

    useSdoValueButton_ = new QPushButton(tr("Use"));
    useSdoValueButton_->setObjectName("useSdoValueButton");
    addrRow->addWidget(useSdoValueButton_);

    layout->addLayout(addrRow);

    auto* writeRow = new QHBoxLayout;
    sdoWriteValue_ = new QLineEdit;
    sdoWriteValue_->setPlaceholderText(tr("Value to write"));
    writeRow->addWidget(sdoWriteValue_);

    auto* actionBtnRow = new QHBoxLayout;
    auto* runActionBtn = new QPushButton(tr("Run Row Action"));
    runActionBtn->setObjectName("runSdoTargetRowAction");
    connect(runActionBtn, &QPushButton::clicked, this, &OdPlugin::sdoTargetPanelRowActionRequested);
    actionBtnRow->addWidget(runActionBtn);

    auto* copyBtn = new QPushButton(tr("Copy Row"));
    copyBtn->setObjectName("copySdoTargetRowEvidence");
    connect(copyBtn, &QPushButton::clicked, this, &OdPlugin::sdoTargetPanelCopyRequested);
    actionBtnRow->addWidget(copyBtn);

    layout->addLayout(writeRow);
    layout->addLayout(actionBtnRow);

    sdoInspectorLabel_ = new QLabel;
    sdoInspectorLabel_->setWordWrap(true);
    layout->addWidget(sdoInspectorLabel_);

    sdoTargetTable_ = new QTableWidget;
    sdoTargetTable_->setColumnCount(3);
    sdoTargetTable_->setHorizontalHeaderLabels({tr("Key"), tr("Value"), tr("Action")});
    sdoTargetTable_->horizontalHeader()->setStretchLastSection(true);
    sdoTargetTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    sdoTargetTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(sdoTargetTable_, &QTableWidget::cellDoubleClicked, this,
            [this](int row, int) { emit sdoTargetPanelRowDoubleClicked(row); });
    connect(sdoTargetTable_, &QTableWidget::currentCellChanged, this, [this]() {
        emit sdoTargetPanelRowActionRequested();
        emit sdoTargetPanelCopyRequested();
    });
    layout->addWidget(sdoTargetTable_);
}

void OdPlugin::buildTargetTrailTab(QWidget* widget) {
    auto* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    sdoTargetTrailTable_ = new QTableWidget;
    sdoTargetTrailTable_->setColumnCount(9);
    sdoTargetTrailTable_->setHorizontalHeaderLabels({tr("Time"), tr("Slave"), tr("Index"), tr("Sub"), tr("Type"),
                                                     tr("Source"), tr("Value"), tr("Write"), tr("Detail")});
    sdoTargetTrailTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    sdoTargetTrailTable_->horizontalHeader()->setStretchLastSection(true);
    sdoTargetTrailTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    sdoTargetTrailTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(sdoTargetTrailTable_, &QTableWidget::currentCellChanged, this, &OdPlugin::sdoTargetTrailSelectionChanged);
    layout->addWidget(sdoTargetTrailTable_);

    sdoTargetTrailDetailLabel_ = new QLabel;
    sdoTargetTrailDetailLabel_->setWordWrap(true);
    layout->addWidget(sdoTargetTrailDetailLabel_);
}

void OdPlugin::buildBookmarkTab(QWidget* widget) {
    auto* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    objectBookmarkTable_ = new QTableWidget;
    objectBookmarkTable_->setColumnCount(10);
    objectBookmarkTable_->setHorizontalHeaderLabels({tr("Slave"), tr("Slave Name"), tr("Index"), tr("Sub"),
                                                     tr("Access"), tr("Type"), tr("Bits"), tr("Name"), tr("Last Value"),
                                                     tr("Source")});
    objectBookmarkTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    objectBookmarkTable_->horizontalHeader()->setStretchLastSection(true);
    objectBookmarkTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    objectBookmarkTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(objectBookmarkTable_, &QTableWidget::currentCellChanged, this, &OdPlugin::objectBookmarkSelectionChanged);
    layout->addWidget(objectBookmarkTable_);

    objectBookmarkDetailLabel_ = new QLabel;
    objectBookmarkDetailLabel_->setWordWrap(true);
    layout->addWidget(objectBookmarkDetailLabel_);
}

void OdPlugin::buildHistoryTab(QWidget* widget) {
    auto* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    sdoHistoryTable_ = new QTableWidget;
    sdoHistoryTable_->setColumnCount(9);
    sdoHistoryTable_->setHorizontalHeaderLabels({tr("Time"), tr("Action"), tr("Slave"), tr("Index"), tr("Sub"),
                                                 tr("Type"), tr("Value"), tr("Status"), tr("Detail")});
    sdoHistoryTable_->horizontalHeader()->setStretchLastSection(true);
    sdoHistoryTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    sdoHistoryTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(sdoHistoryTable_, &QTableWidget::currentCellChanged, this, &OdPlugin::sdoHistorySelectionChanged);
    layout->addWidget(sdoHistoryTable_);
}

// ── Table management ──────────────────────────────────────────────────
void OdPlugin::updateSdoTableSummary(int total, int visible, int withEvidence, int failed, int writable) {
    if (sdoSummaryLabel_) {
        sdoSummaryLabel_->setText(tr("%1 obj | %2 vis | %3 evidence | %4 fail | %5 rw")
                                      .arg(total)
                                      .arg(visible)
                                      .arg(withEvidence)
                                      .arg(failed)
                                      .arg(writable));
    }
}

void OdPlugin::ensureSdoTargetTrailTable() {
    if (!sdoTargetTrailTable_ || sdoTargetTrailTable_->columnCount() != 9) {
        if (sdoTargetTrailTable_) {
            sdoTargetTrailTable_->setColumnCount(9);
        }
    }
}

void OdPlugin::updateSdoTargetTrailRowDetail(const QString& text, const QString& severity, const QString& tooltip) {
    if (!sdoTargetTrailDetailLabel_)
        return;
    sdoTargetTrailDetailLabel_->setText(text);
    sdoTargetTrailDetailLabel_->setProperty("severity", severity);
    sdoTargetTrailDetailLabel_->setToolTip(tooltip);
}

void OdPlugin::ensureObjectBookmarkTable() {
    if (!objectBookmarkTable_ || objectBookmarkTable_->columnCount() != 10) {
        if (objectBookmarkTable_) {
            objectBookmarkTable_->setColumnCount(10);
        }
    }
}

void OdPlugin::updateObjectBookmarkRowDetail(const QString& text, const QString& severity, const QString& tooltip) {
    if (!objectBookmarkDetailLabel_)
        return;
    objectBookmarkDetailLabel_->setText(text);
    objectBookmarkDetailLabel_->setProperty("severity", severity);
    objectBookmarkDetailLabel_->setToolTip(tooltip);
}

void OdPlugin::ensureSdoHistoryTable() {
    if (!sdoHistoryTable_ || sdoHistoryTable_->columnCount() != 9) {
        if (sdoHistoryTable_) {
            sdoHistoryTable_->setColumnCount(9);
        }
    }
}

void OdPlugin::updateSdoTableEvidenceRow(int row, const QString& value, const QString& status, const QString& time,
                                         const QColor& statusColor, const QColor& valueBackground) {
    if (!sdoTable_ || row < 0 || row >= sdoTable_->rowCount())
        return;

    auto ensureItem = [this, row](int column) {
        auto* item = sdoTable_->item(row, column);
        if (!item) {
            item = new QTableWidgetItem;
            sdoTable_->setItem(row, column, item);
        }
        return item;
    };

    auto* valueItem = ensureItem(7);
    auto* statusItem = ensureItem(8);
    valueItem->setText(value);
    valueItem->setBackground(valueBackground);
    statusItem->setText(QString("%1  %2").arg(status, time));
    statusItem->setForeground(statusColor);
}

void OdPlugin::updateSdoTargetPanelRows(const QList<QPair<QString, QString>>& rows,
                                        const QMap<QString, QString>& rowColors,
                                        const QMap<QString, QString>& rowActions) {
    if (!sdoTargetTable_)
        return;

    const QSignalBlocker blocker(sdoTargetTable_);
    sdoTargetTable_->clearContents();
    sdoTargetTable_->setRowCount(rows.size());

    for (int r = 0; r < rows.size(); ++r) {
        auto* keyItem = new QTableWidgetItem(rows.at(r).first);
        auto* valueItem = new QTableWidgetItem(rows.at(r).second);
        const QString actionText = rowActions.value(rows.at(r).first, tr("Copy row"));
        auto* actionItem = new QTableWidgetItem(actionText);

        const QString colorKey = rows.at(r).first;
        if (rowColors.contains(colorKey)) {
            valueItem->setForeground(QColor(rowColors.value(colorKey)));
        }

        sdoTargetTable_->setItem(r, 0, keyItem);
        sdoTargetTable_->setItem(r, 1, valueItem);
        sdoTargetTable_->setItem(r, 2, actionItem);
    }
    sdoTargetTable_->resizeRowsToContents();
}

void OdPlugin::updateSdoInspectorLabel(const QString& text, const QString& state) {
    if (!sdoInspectorLabel_)
        return;
    sdoInspectorLabel_->setText(text);
    sdoInspectorLabel_->setToolTip(text);
    sdoInspectorLabel_->setProperty("state", state);
}
