#include "CloudManagerPlugin.h"

#include <QDateTime>
#include <QFile>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>
#include <QTreeWidget>
#include <QVBoxLayout>

CloudManagerPlugin::CloudManagerPlugin(QObject *parent) {
  if (parent) setParent(parent);
  buildUi();
}

QString CloudManagerPlugin::id() const { return "cloudmanager"; }
QString CloudManagerPlugin::displayName() const { return "Cloud Manager"; }
QString CloudManagerPlugin::displayNameZh() const { return QStringLiteral("云管理器"); }
QIcon CloudManagerPlugin::icon() const { return QIcon::fromTheme("cloud"); }
int CloudManagerPlugin::defaultOrder() const { return 310; }
bool CloudManagerPlugin::visible() const { return true; }

void CloudManagerPlugin::activate() {}
void CloudManagerPlugin::deactivate() {}

QWidget *CloudManagerPlugin::widget() { return containerWidget_; }
QTableWidget *CloudManagerPlugin::connectionTable() const { return connectionTable_; }
QProgressBar *CloudManagerPlugin::syncProgress() const { return syncProgress_; }
QTreeWidget *CloudManagerPlugin::backupHistory() const { return backupHistory_; }
QTextEdit *CloudManagerPlugin::monitoringView() const { return monitoringView_; }

void CloudManagerPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QHBoxLayout(containerWidget_);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  auto *splitter = new QSplitter;

  auto *leftPanel = new QWidget;
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(4, 4, 4, 4);

  auto *connLabel = new QLabel(tr("Cloud Connections"));
  leftLayout->addWidget(connLabel);

  connectionTable_ = new QTableWidget;
  connectionTable_->setColumnCount(3);
  connectionTable_->setHorizontalHeaderLabels({tr("Name"), tr("Endpoint"), tr("Status")});
  connectionTable_->horizontalHeader()->setStretchLastSection(true);
  connectionTable_->setSelectionBehavior(QTableWidget::SelectRows);
  connectionTable_->setSelectionMode(QTableWidget::SingleSelection);
  leftLayout->addWidget(connectionTable_);

  auto *connBtnRow = new QHBoxLayout;
  addConnectionBtn_ = new QPushButton(tr("Add"));
  connBtnRow->addWidget(addConnectionBtn_);
  removeConnectionBtn_ = new QPushButton(tr("Remove"));
  connBtnRow->addWidget(removeConnectionBtn_);
  connBtnRow->addStretch();
  leftLayout->addLayout(connBtnRow);

  auto *syncLabel = new QLabel(tr("Cloud Sync"));
  leftLayout->addWidget(syncLabel);

  syncProgress_ = new QProgressBar;
  syncProgress_->setRange(0, 100);
  syncProgress_->setValue(0);
  leftLayout->addWidget(syncProgress_);

  syncBtn_ = new QPushButton(tr("Start Sync"));
  leftLayout->addWidget(syncBtn_);

  splitter->addWidget(leftPanel);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);
  rightLayout->setContentsMargins(4, 4, 4, 4);

  auto *backupLabel = new QLabel(tr("Cloud Backup"));
  rightLayout->addWidget(backupLabel);

  backupHistory_ = new QTreeWidget;
  backupHistory_->setHeaderLabels({tr("Timestamp"), tr("Status")});
  rightLayout->addWidget(backupHistory_);

  backupBtn_ = new QPushButton(tr("Create Backup"));
  rightLayout->addWidget(backupBtn_);

  auto *monitorLabel = new QLabel(tr("Cloud Monitoring"));
  rightLayout->addWidget(monitorLabel);

  monitoringView_ = new QTextEdit;
  monitoringView_->setReadOnly(true);
  monitoringView_->setPlaceholderText(tr("Cloud monitoring metrics..."));
  rightLayout->addWidget(monitoringView_, 1);

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

  connect(addConnectionBtn_, &QPushButton::clicked, this, [this]() {
    addConnection(tr("New Connection"), tr("cloud.example.com"));
  });
  connect(removeConnectionBtn_, &QPushButton::clicked, this, [this]() {
    auto *item = connectionTable_->currentItem();
    if (item) removeConnection(connectionTable_->item(item->row(), 0)->text());
  });
  connect(syncBtn_, &QPushButton::clicked, this, [this]() {
    statusLabel_->setText(tr("Syncing..."));
  });
  connect(backupBtn_, &QPushButton::clicked, this, [this]() {
    addBackupEntry(QDateTime::currentDateTime().toString(Qt::ISODate), tr("Completed"));
  });
  connect(exportBtn_, &QPushButton::clicked, this, &CloudManagerPlugin::exportRequested);
}

void CloudManagerPlugin::addConnection(const QString &name, const QString &endpoint) {
  int row = connectionTable_->rowCount();
  connectionTable_->insertRow(row);
  connectionTable_->setItem(row, 0, new QTableWidgetItem(name));
  connectionTable_->setItem(row, 1, new QTableWidgetItem(endpoint));
  connectionTable_->setItem(row, 2, new QTableWidgetItem(tr("Connected")));
  statusLabel_->setText(tr("Added: %1").arg(name));
  emit connectionAdded(name);
}

void CloudManagerPlugin::removeConnection(const QString &name) {
  for (int i = 0; i < connectionTable_->rowCount(); ++i) {
    if (connectionTable_->item(i, 0)->text() == name) {
      connectionTable_->removeRow(i);
      emit connectionRemoved(name);
      return;
    }
  }
}

void CloudManagerPlugin::clearConnections() { connectionTable_->setRowCount(0); }
int CloudManagerPlugin::connectionCount() const { return connectionTable_->rowCount(); }

void CloudManagerPlugin::setSyncProgress(int percent) {
  syncProgress_->setValue(percent);
  emit syncProgressChanged(percent);
}

int CloudManagerPlugin::syncProgressValue() const { return syncProgress_->value(); }

void CloudManagerPlugin::addBackupEntry(const QString &timestamp, const QString &status) {
  new QTreeWidgetItem(backupHistory_, {timestamp, status});
  statusLabel_->setText(tr("Backup: %1").arg(timestamp));
  emit backupAdded(timestamp);
}

void CloudManagerPlugin::clearBackupHistory() { backupHistory_->clear(); }
int CloudManagerPlugin::backupCount() const { return backupHistory_->topLevelItemCount(); }

void CloudManagerPlugin::setMonitoringText(const QString &text) {
  monitoringView_->setPlainText(text);
}

QString CloudManagerPlugin::monitoringText() const {
  return monitoringView_->toPlainText();
}

bool CloudManagerPlugin::exportCloudReport(const QString &filePath, const QString &format) {
  QJsonObject root;
  root["version"] = 1;
  root["format"] = format;
  root["monitoring"] = monitoringView_->toPlainText();

  QJsonArray conns;
  for (int i = 0; i < connectionTable_->rowCount(); ++i) {
    QJsonObject conn;
    conn["name"] = connectionTable_->item(i, 0)->text();
    conn["endpoint"] = connectionTable_->item(i, 1)->text();
    conn["status"] = connectionTable_->item(i, 2)->text();
    conns.append(conn);
  }
  root["connections"] = conns;

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly)) return false;
  file.write(QJsonDocument(root).toJson());
  return true;
}
