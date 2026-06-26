#include "DeploymentManagerPlugin.h"

#include <QDateTime>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>

DeploymentManagerPlugin::DeploymentManagerPlugin(QObject *parent) {
  if (parent) setParent(parent);
  buildUi();
}

QString DeploymentManagerPlugin::id() const { return "deploymentmanager"; }
QString DeploymentManagerPlugin::displayName() const { return "Deployment Manager"; }
QString DeploymentManagerPlugin::displayNameZh() const { return QStringLiteral("部署管理器"); }
QIcon DeploymentManagerPlugin::icon() const { return QIcon::fromTheme("system-software-install"); }
int DeploymentManagerPlugin::defaultOrder() const { return 310; }
bool DeploymentManagerPlugin::visible() const { return true; }
void DeploymentManagerPlugin::activate() {}
void DeploymentManagerPlugin::deactivate() {}

QWidget *DeploymentManagerPlugin::widget() { return container_; }
QTableWidget *DeploymentManagerPlugin::targetTable() const { return targetTable_; }
QTableWidget *DeploymentManagerPlugin::packageTable() const { return packageTable_; }
QTableWidget *DeploymentManagerPlugin::historyTable() const { return historyTable_; }
QTextEdit *DeploymentManagerPlugin::statusLog() const { return statusLog_; }
QLabel *DeploymentManagerPlugin::statusLabel() const { return statusLabel_; }

int DeploymentManagerPlugin::targetCount() const { return targets_.size(); }
int DeploymentManagerPlugin::packageCount() const { return packages_.size(); }
int DeploymentManagerPlugin::historyCount() const { return records_.size(); }

void DeploymentManagerPlugin::buildUi() {
  container_ = new QWidget;
  auto *mainLayout = new QHBoxLayout(container_);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  auto *splitter = new QSplitter;

  auto *leftPanel = new QWidget;
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(4, 4, 4, 4);

  leftLayout->addWidget(new QLabel(tr("Deployment Targets")));
  targetTable_ = new QTableWidget(0, 4);
  targetTable_->setHorizontalHeaderLabels({tr("Name"), tr("Address"), tr("Config"), tr("Status")});
  targetTable_->horizontalHeader()->setStretchLastSection(true);
  leftLayout->addWidget(targetTable_);

  leftLayout->addWidget(new QLabel(tr("Deployment Packages")));
  packageTable_ = new QTableWidget(0, 4);
  packageTable_->setHorizontalHeaderLabels({tr("Name"), tr("Version"), tr("Description"), tr("Created")});
  packageTable_->horizontalHeader()->setStretchLastSection(true);
  leftLayout->addWidget(packageTable_);

  splitter->addWidget(leftPanel);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);
  rightLayout->setContentsMargins(4, 4, 4, 4);

  rightLayout->addWidget(new QLabel(tr("Deployment History")));
  historyTable_ = new QTableWidget(0, 5);
  historyTable_->setHorizontalHeaderLabels({tr("Target"), tr("Package"), tr("Version"), tr("Status"), tr("Time")});
  historyTable_->horizontalHeader()->setStretchLastSection(true);
  rightLayout->addWidget(historyTable_);

  rightLayout->addWidget(new QLabel(tr("Deployment Status")));
  statusLog_ = new QTextEdit;
  statusLog_->setReadOnly(true);
  statusLog_->setPlaceholderText(tr("Select a deployment to view details..."));
  rightLayout->addWidget(statusLog_);

  statusLabel_ = new QLabel(tr("Status: Ready"));
  rightLayout->addWidget(statusLabel_);

  auto *btnRow = new QHBoxLayout;
  deployBtn_ = new QPushButton(tr("Deploy"));
  rollbackBtn_ = new QPushButton(tr("Rollback"));
  exportBtn_ = new QPushButton(tr("Export Log"));
  clearBtn_ = new QPushButton(tr("Clear History"));
  btnRow->addWidget(deployBtn_);
  btnRow->addWidget(rollbackBtn_);
  btnRow->addWidget(exportBtn_);
  btnRow->addWidget(clearBtn_);
  rightLayout->addLayout(btnRow);

  splitter->addWidget(rightPanel);
  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 1);
  mainLayout->addWidget(splitter);

  connect(deployBtn_, &QPushButton::clicked, this, [this]() {
    if (!targets_.isEmpty() && !packages_.isEmpty())
      deploy(0, 0);
  });
  connect(rollbackBtn_, &QPushButton::clicked, this, [this]() {
    if (!records_.isEmpty())
      rollback(records_.size() - 1);
  });
  connect(exportBtn_, &QPushButton::clicked, this, [this]() {
    QString path = QFileDialog::getSaveFileName(container_, tr("Export Log"), QString(), "JSON (*.json)");
    if (!path.isEmpty()) exportLog(path);
  });
  connect(clearBtn_, &QPushButton::clicked, this, &DeploymentManagerPlugin::clearHistory);

  refreshStatus();
}

void DeploymentManagerPlugin::addTarget(const DeploymentMgrTarget &target) {
  DeploymentMgrTarget t = target;
  if (t.id.isEmpty()) t.id = QString("dt_%1").arg(nextId_++);
  targets_.append(t);

  int row = targetTable_->rowCount();
  targetTable_->insertRow(row);
  targetTable_->setItem(row, 0, new QTableWidgetItem(t.name));
  targetTable_->setItem(row, 1, new QTableWidgetItem(t.address));
  targetTable_->setItem(row, 2, new QTableWidgetItem(t.config));
  targetTable_->setItem(row, 3, new QTableWidgetItem(t.status));

  emit targetAdded(t.id, t.name);
  refreshStatus();
}

void DeploymentManagerPlugin::removeTarget(int index) {
  if (index < 0 || index >= targets_.size()) return;
  QString id = targets_[index].id;
  targets_.removeAt(index);
  targetTable_->removeRow(index);
  emit targetRemoved(id);
  refreshStatus();
}

void DeploymentManagerPlugin::updateTargetStatus(int index, const QString &status) {
  if (index < 0 || index >= targets_.size()) return;
  targets_[index].status = status;
  targetTable_->setItem(index, 3, new QTableWidgetItem(status));
  emit targetStatusChanged(targets_[index].id, status);
}

void DeploymentManagerPlugin::addPackage(const DeploymentMgrPackage &pkg) {
  DeploymentMgrPackage p = pkg;
  if (p.id.isEmpty()) p.id = QString("dp_%1").arg(nextId_++);
  if (p.createdAt.isEmpty()) p.createdAt = QDateTime::currentDateTime().toString(Qt::ISODate);
  packages_.append(p);

  int row = packageTable_->rowCount();
  packageTable_->insertRow(row);
  packageTable_->setItem(row, 0, new QTableWidgetItem(p.name));
  packageTable_->setItem(row, 1, new QTableWidgetItem(p.version));
  packageTable_->setItem(row, 2, new QTableWidgetItem(p.description));
  packageTable_->setItem(row, 3, new QTableWidgetItem(p.createdAt));

  emit packageAdded(p.id, p.name);
  refreshStatus();
}

void DeploymentManagerPlugin::removePackage(int index) {
  if (index < 0 || index >= packages_.size()) return;
  QString id = packages_[index].id;
  packages_.removeAt(index);
  packageTable_->removeRow(index);
  emit packageRemoved(id);
  refreshStatus();
}

void DeploymentManagerPlugin::deploy(int targetIndex, int packageIndex) {
  if (targetIndex < 0 || targetIndex >= targets_.size()) return;
  if (packageIndex < 0 || packageIndex >= packages_.size()) return;

  const auto &t = targets_[targetIndex];
  const auto &p = packages_[packageIndex];

  emit deploymentStarted(t.id, p.id);

  DeploymentMgrRecord rec;
  rec.id = QString("rec_%1").arg(nextId_++);
  rec.targetName = t.name;
  rec.packageName = p.name;
  rec.version = p.version;
  rec.status = "Rejected";
  rec.timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
  rec.log = tr("Deployment of %1 v%2 to %3 requires a deployment backend acknowledgement.")
                .arg(p.name, p.version, t.name);
  records_.append(rec);

  int row = historyTable_->rowCount();
  historyTable_->insertRow(row);
  historyTable_->setItem(row, 0, new QTableWidgetItem(rec.targetName));
  historyTable_->setItem(row, 1, new QTableWidgetItem(rec.packageName));
  historyTable_->setItem(row, 2, new QTableWidgetItem(rec.version));
  historyTable_->setItem(row, 3, new QTableWidgetItem(rec.status));
  historyTable_->setItem(row, 4, new QTableWidgetItem(rec.timestamp));

  refreshStatus();
}

void DeploymentManagerPlugin::rollback(int historyIndex) {
  if (historyIndex < 0 || historyIndex >= records_.size()) return;
  const auto &orig = records_[historyIndex];
  if (orig.status == QStringLiteral("Success")) {
    emit rollbackRequested(orig.id);
  }
}

void DeploymentManagerPlugin::clearHistory() {
  records_.clear();
  historyTable_->setRowCount(0);
  refreshStatus();
}

bool DeploymentManagerPlugin::exportLog(const QString &filePath) {
  QJsonObject root;
  root["version"] = 1;
  root["totalDeployments"] = records_.size();
  QJsonArray arr;
  for (const auto &r : records_) {
    QJsonObject obj;
    obj["id"] = r.id;
    obj["targetName"] = r.targetName;
    obj["packageName"] = r.packageName;
    obj["version"] = r.version;
    obj["status"] = r.status;
    obj["timestamp"] = r.timestamp;
    obj["log"] = r.log;
    arr.append(obj);
  }
  root["deployments"] = arr;

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly)) return false;
  file.write(QJsonDocument(root).toJson());
  return true;
}

void DeploymentManagerPlugin::refreshStatus() {
  QString s;
  s += tr("Targets: %1\n").arg(targets_.size());
  s += tr("Packages: %1\n").arg(packages_.size());
  s += tr("Deployments: %1\n").arg(records_.size());
  statusLog_->setPlainText(s);
  statusLabel_->setText(tr("Status: Ready"));
}
