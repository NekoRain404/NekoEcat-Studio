#include "EdgeComputingPlugin.h"

#include <QFile>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>
#include <QTreeWidget>
#include <QVBoxLayout>

EdgeComputingPlugin::EdgeComputingPlugin(QObject *parent) {
  if (parent) setParent(parent);
  buildUi();
}

QString EdgeComputingPlugin::id() const { return "edgecomputing"; }
QString EdgeComputingPlugin::displayName() const { return "Edge Computing"; }
QString EdgeComputingPlugin::displayNameZh() const { return QStringLiteral("边缘计算"); }
QIcon EdgeComputingPlugin::icon() const { return QIcon::fromTheme("computer"); }
int EdgeComputingPlugin::defaultOrder() const { return 320; }
bool EdgeComputingPlugin::visible() const { return true; }

void EdgeComputingPlugin::activate() {}
void EdgeComputingPlugin::deactivate() {}

QWidget *EdgeComputingPlugin::widget() { return containerWidget_; }
QTableWidget *EdgeComputingPlugin::deviceTable() const { return deviceTable_; }
QTreeWidget *EdgeComputingPlugin::processingJobs() const { return processingJobs_; }
QTextEdit *EdgeComputingPlugin::analyticsView() const { return analyticsView_; }
QTableWidget *EdgeComputingPlugin::storageTable() const { return storageTable_; }

void EdgeComputingPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QHBoxLayout(containerWidget_);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  auto *splitter = new QSplitter;

  auto *leftPanel = new QWidget;
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(4, 4, 4, 4);

  auto *devLabel = new QLabel(tr("Edge Devices"));
  leftLayout->addWidget(devLabel);

  deviceTable_ = new QTableWidget;
  deviceTable_->setColumnCount(3);
  deviceTable_->setHorizontalHeaderLabels({tr("Name"), tr("Type"), tr("Status")});
  deviceTable_->horizontalHeader()->setStretchLastSection(true);
  deviceTable_->setSelectionBehavior(QTableWidget::SelectRows);
  deviceTable_->setSelectionMode(QTableWidget::SingleSelection);
  leftLayout->addWidget(deviceTable_);

  auto *devBtnRow = new QHBoxLayout;
  addDeviceBtn_ = new QPushButton(tr("Add Device"));
  devBtnRow->addWidget(addDeviceBtn_);
  removeDeviceBtn_ = new QPushButton(tr("Remove"));
  devBtnRow->addWidget(removeDeviceBtn_);
  devBtnRow->addStretch();
  leftLayout->addLayout(devBtnRow);

  auto *procLabel = new QLabel(tr("Edge Processing"));
  leftLayout->addWidget(procLabel);

  processingJobs_ = new QTreeWidget;
  processingJobs_->setHeaderLabels({tr("Job ID"), tr("Description")});
  leftLayout->addWidget(processingJobs_);

  addJobBtn_ = new QPushButton(tr("Add Job"));
  leftLayout->addWidget(addJobBtn_);

  splitter->addWidget(leftPanel);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);
  rightLayout->setContentsMargins(4, 4, 4, 4);

  auto *analyticsLabel = new QLabel(tr("Edge Analytics"));
  rightLayout->addWidget(analyticsLabel);

  analyticsView_ = new QTextEdit;
  analyticsView_->setReadOnly(true);
  analyticsView_->setPlaceholderText(tr("Edge analytics results..."));
  rightLayout->addWidget(analyticsView_);

  auto *storageLabel = new QLabel(tr("Edge Storage"));
  rightLayout->addWidget(storageLabel);

  storageTable_ = new QTableWidget;
  storageTable_->setColumnCount(3);
  storageTable_->setHorizontalHeaderLabels({tr("Name"), tr("Size"), tr("Usage")});
  storageTable_->horizontalHeader()->setStretchLastSection(true);
  rightLayout->addWidget(storageTable_);

  auto *exportRow = new QHBoxLayout;
  exportBtn_ = new QPushButton(tr("Export Report"));
  exportRow->addWidget(exportBtn_);
  exportRow->addStretch();
  rightLayout->addLayout(exportRow);

  statusLabel_ = new QLabel(tr("Ready"));
  rightLayout->addWidget(statusLabel_);

  splitter->addWidget(rightPanel);
  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 1);

  mainLayout->addWidget(splitter);

  connect(addDeviceBtn_, &QPushButton::clicked, this, [this]() {
    addDevice(tr("Edge Node"), tr("Gateway"), tr("Online"));
  });
  connect(removeDeviceBtn_, &QPushButton::clicked, this, [this]() {
    auto *item = deviceTable_->currentItem();
    if (item) removeDevice(deviceTable_->item(item->row(), 0)->text());
  });
  connect(addJobBtn_, &QPushButton::clicked, this, [this]() {
    addProcessingJob(tr("JOB-%1").arg(processingJobCount() + 1), tr("New processing task"));
  });
  connect(exportBtn_, &QPushButton::clicked, this, &EdgeComputingPlugin::exportRequested);
}

void EdgeComputingPlugin::addDevice(const QString &name, const QString &type, const QString &status) {
  int row = deviceTable_->rowCount();
  deviceTable_->insertRow(row);
  deviceTable_->setItem(row, 0, new QTableWidgetItem(name));
  deviceTable_->setItem(row, 1, new QTableWidgetItem(type));
  deviceTable_->setItem(row, 2, new QTableWidgetItem(status));
  statusLabel_->setText(tr("Device added: %1").arg(name));
  emit deviceAdded(name);
}

void EdgeComputingPlugin::removeDevice(const QString &name) {
  for (int i = 0; i < deviceTable_->rowCount(); ++i) {
    if (deviceTable_->item(i, 0)->text() == name) {
      deviceTable_->removeRow(i);
      emit deviceRemoved(name);
      return;
    }
  }
}

void EdgeComputingPlugin::clearDevices() { deviceTable_->setRowCount(0); }
int EdgeComputingPlugin::deviceCount() const { return deviceTable_->rowCount(); }

void EdgeComputingPlugin::addProcessingJob(const QString &jobId, const QString &description) {
  new QTreeWidgetItem(processingJobs_, {jobId, description});
  statusLabel_->setText(tr("Job added: %1").arg(jobId));
  emit processingJobAdded(jobId);
}

void EdgeComputingPlugin::removeProcessingJob(const QString &jobId) {
  for (int i = 0; i < processingJobs_->topLevelItemCount(); ++i) {
    if (processingJobs_->topLevelItem(i)->text(0) == jobId) {
      delete processingJobs_->takeTopLevelItem(i);
      return;
    }
  }
}

void EdgeComputingPlugin::clearProcessingJobs() { processingJobs_->clear(); }
int EdgeComputingPlugin::processingJobCount() const { return processingJobs_->topLevelItemCount(); }

void EdgeComputingPlugin::setAnalyticsText(const QString &text) {
  analyticsView_->setPlainText(text);
  emit analyticsUpdated();
}

QString EdgeComputingPlugin::analyticsText() const {
  return analyticsView_->toPlainText();
}

void EdgeComputingPlugin::addStorageEntry(const QString &name, const QString &size, const QString &usage) {
  int row = storageTable_->rowCount();
  storageTable_->insertRow(row);
  storageTable_->setItem(row, 0, new QTableWidgetItem(name));
  storageTable_->setItem(row, 1, new QTableWidgetItem(size));
  storageTable_->setItem(row, 2, new QTableWidgetItem(usage));
}

void EdgeComputingPlugin::clearStorageEntries() { storageTable_->setRowCount(0); }
int EdgeComputingPlugin::storageEntryCount() const { return storageTable_->rowCount(); }

bool EdgeComputingPlugin::exportEdgeReport(const QString &filePath, const QString &format) {
  QJsonObject root;
  root["version"] = 1;
  root["format"] = format;
  root["analytics"] = analyticsView_->toPlainText();

  QJsonArray devices;
  for (int i = 0; i < deviceTable_->rowCount(); ++i) {
    QJsonObject dev;
    dev["name"] = deviceTable_->item(i, 0)->text();
    dev["type"] = deviceTable_->item(i, 1)->text();
    dev["status"] = deviceTable_->item(i, 2)->text();
    devices.append(dev);
  }
  root["devices"] = devices;

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly)) return false;
  file.write(QJsonDocument(root).toJson());
  return true;
}
