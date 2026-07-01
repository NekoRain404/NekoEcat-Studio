#include "DeviceManagerPlugin.h"
#include "services/DeviceManagerService.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

DeviceManagerPlugin::DeviceManagerPlugin(DeviceManagerService *service,
                                         QObject *parent)
    : service_(service) {
  if (parent) setParent(parent);
  buildUi();

  connect(service_, &DeviceManagerService::deviceListChanged, this,
          [this]() { updateDisplay(); });
}

QString DeviceManagerPlugin::id() const { return "devicemanager"; }
QString DeviceManagerPlugin::displayName() const { return "Device Manager"; }
QString DeviceManagerPlugin::displayNameZh() const {
  return QStringLiteral("设备管理");
}
QIcon DeviceManagerPlugin::icon() const {
  return QIcon::fromTheme("computer");
}
int DeviceManagerPlugin::defaultOrder() const { return 95; }
bool DeviceManagerPlugin::visible() const { return false; }

void DeviceManagerPlugin::activate() {}
void DeviceManagerPlugin::deactivate() {}

QWidget *DeviceManagerPlugin::widget() { return containerWidget_; }

void DeviceManagerPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *layout = new QVBoxLayout(containerWidget_);
  layout->setContentsMargins(14, 14, 14, 14);
  layout->setSpacing(10);

  auto *toolbar = new QHBoxLayout;
  toolbar->setSpacing(8);

  scanBtn_ = new QPushButton(tr("Scan Devices"));
  toolbar->addWidget(scanBtn_);

  configureBtn_ = new QPushButton(tr("Configure"));
  toolbar->addWidget(configureBtn_);

  resetBtn_ = new QPushButton(tr("Reset"));
  toolbar->addWidget(resetBtn_);

  statusLabel_ = new QLabel(tr("Idle"));
  toolbar->addWidget(statusLabel_);

  toolbar->addStretch();

  deviceCountLabel_ = new QLabel(tr("Devices: 0"));
  toolbar->addWidget(deviceCountLabel_);

  layout->addLayout(toolbar);

  deviceTable_ = new QTableWidget;
  deviceTable_->setColumnCount(6);
  deviceTable_->setHorizontalHeaderLabels(
      {tr("Address"), tr("Name"), tr("State"), tr("Vendor"), tr("Product"),
       tr("Revision")});
  deviceTable_->horizontalHeader()->setStretchLastSection(true);
  deviceTable_->verticalHeader()->setVisible(false);
  deviceTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  deviceTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  deviceTable_->setShowGrid(false);
  deviceTable_->setAlternatingRowColors(true);
  layout->addWidget(deviceTable_, 1);

  connect(scanBtn_, &QPushButton::clicked, this, [this]() {
    statusLabel_->setText(tr("Scanning..."));
    service_->startScan();
  });

  connect(configureBtn_, &QPushButton::clicked, this, [this]() {
    int row = deviceTable_->currentRow();
    if (row >= 0) service_->configureDevice(row);
  });

  connect(resetBtn_, &QPushButton::clicked, this, [this]() {
    service_->resetDevices();
    updateDisplay();
  });
}

void DeviceManagerPlugin::updateDisplay() {
  auto devices = service_->allDevices();

  deviceTable_->setRowCount(0);
  for (const auto &d : devices) {
    int r = deviceTable_->rowCount();
    deviceTable_->insertRow(r);
    deviceTable_->setItem(r, 0,
                          new QTableWidgetItem(QString::number(d.address)));
    deviceTable_->setItem(r, 1, new QTableWidgetItem(d.name));
    deviceTable_->setItem(r, 2, new QTableWidgetItem(d.state));
    deviceTable_->setItem(r, 3, new QTableWidgetItem(d.vendor));
    deviceTable_->setItem(r, 4, new QTableWidgetItem(d.product));
    deviceTable_->setItem(r, 5,
                          new QTableWidgetItem(QString::number(d.revision)));
  }

  deviceCountLabel_->setText(tr("Devices: %1").arg(devices.size()));
  statusLabel_->setText(tr("Idle"));
}
