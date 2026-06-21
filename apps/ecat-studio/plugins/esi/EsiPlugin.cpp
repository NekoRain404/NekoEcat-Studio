#include "EsiPlugin.h"
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
#include <QVBoxLayout>

EsiPlugin::EsiPlugin(EsiService *service, QObject *parent)
    : service_(service) {
  if (parent) setParent(parent);
  buildUi();

  connect(service_, &EsiService::esiImported, this,
          [this](int) { updateDeviceList(); });
}

QString EsiPlugin::id() const { return "esi"; }
QString EsiPlugin::displayName() const { return "ESI Repository"; }
QString EsiPlugin::displayNameZh() const { return QStringLiteral("ESI 仓库"); }
QIcon EsiPlugin::icon() const { return QIcon::fromTheme("document-properties"); }
int EsiPlugin::defaultOrder() const { return 90; }
bool EsiPlugin::visible() const { return true; }

void EsiPlugin::activate() {}
void EsiPlugin::deactivate() {}

QWidget *EsiPlugin::widget() { return containerWidget_; }

void EsiPlugin::buildUi() {
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

  exportBtn_ = new QPushButton(tr("Export"));
  exportBtn_->setEnabled(false);
  toolbar->addWidget(exportBtn_);

  refreshBtn_ = new QPushButton(tr("Refresh"));
  toolbar->addWidget(refreshBtn_);

  layout->addLayout(toolbar);

  auto *splitter = new QSplitter(Qt::Horizontal);

  deviceList_ = new QListWidget;
  deviceList_->setMinimumWidth(200);
  splitter->addWidget(deviceList_);

  detailTable_ = new QTableWidget;
  detailTable_->setColumnCount(2);
  detailTable_->setHorizontalHeaderLabels({tr("Property"), tr("Value")});
  detailTable_->horizontalHeader()->setStretchLastSection(true);
  detailTable_->verticalHeader()->setVisible(false);
  detailTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  detailTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  detailTable_->setShowGrid(false);
  detailTable_->setAlternatingRowColors(true);
  splitter->addWidget(detailTable_);

  layout->addWidget(splitter, 1);

  summaryLabel_ = new QLabel(tr("No ESI files loaded"));
  layout->addWidget(summaryLabel_);

  connect(importBtn_, &QPushButton::clicked, this, &EsiPlugin::importFile);
  connect(exportBtn_, &QPushButton::clicked, this, &EsiPlugin::exportSelected);
  connect(refreshBtn_, &QPushButton::clicked, this, &EsiPlugin::refreshList);
  connect(deviceList_, &QListWidget::currentRowChanged, this,
          &EsiPlugin::showDeviceDetail);
  connect(filterEdit_, &QLineEdit::textChanged, this, [this](const QString &) {
    updateDeviceList();
  });
}

void EsiPlugin::importFile() {
  const QString path = QFileDialog::getOpenFileName(
      containerWidget_, tr("Import ESI XML"), QString(),
      "ESI XML (*.xml);;All Files (*)");
  if (path.isEmpty()) return;
  service_->importEsi(path);
}

void EsiPlugin::refreshList() { updateDeviceList(); }

void EsiPlugin::exportSelected() {
  int row = deviceList_->currentRow();
  if (row < 0) return;
  auto devices = service_->listDevices();
  int filteredRow = 0;
  QString needle = filterEdit_->text().trimmed().toLower();
  for (int i = 0; i < devices.size(); ++i) {
    if (!needle.isEmpty() &&
        !devices[i].name.toLower().contains(needle) &&
        !devices[i].type.toLower().contains(needle))
      continue;
    if (filteredRow == row) {
      const QString path = QFileDialog::getSaveFileName(
          containerWidget_, tr("Export ESI XML"), QString(),
          "ESI XML (*.xml)");
      if (!path.isEmpty())
        service_->exportEsi(devices[i].deviceId, path);
      return;
    }
    ++filteredRow;
  }
}

void EsiPlugin::updateDeviceList() {
  deviceList_->clear();
  auto devices = service_->listDevices();
  QString needle = filterEdit_->text().trimmed().toLower();
  int count = 0;
  for (const auto &d : devices) {
    if (!needle.isEmpty() &&
        !d.name.toLower().contains(needle) &&
        !d.type.toLower().contains(needle))
      continue;
    deviceList_->addItem(QStringLiteral("%1 (%2)").arg(d.name, d.type));
    ++count;
  }
  exportBtn_->setEnabled(count > 0);
  summaryLabel_->setText(tr("%n device(s) in repository", nullptr, count));
}

void EsiPlugin::showDeviceDetail(int index) {
  detailTable_->setRowCount(0);
  if (index < 0) return;

  auto devices = service_->listDevices();
  QString needle = filterEdit_->text().trimmed().toLower();
  int filteredRow = 0;
  for (const auto &d : devices) {
    if (!needle.isEmpty() &&
        !d.name.toLower().contains(needle) &&
        !d.type.toLower().contains(needle))
      continue;
    if (filteredRow == index) {
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
      addRow(tr("Rx PDOs"), QString::number(d.rxPdos.size()));
      addRow(tr("Tx PDOs"), QString::number(d.txPdos.size()));
      addRow(tr("Sync Managers"), QString::number(d.syncManagers.size()));
      return;
    }
    ++filteredRow;
  }
}
