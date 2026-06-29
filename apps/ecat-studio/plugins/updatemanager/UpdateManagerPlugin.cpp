#include "UpdateManagerPlugin.h"

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

UpdateManagerPlugin::UpdateManagerPlugin(QObject *parent) {
  if (parent) setParent(parent);
  buildUi();
}

QString UpdateManagerPlugin::id() const { return "updatemanager"; }
QString UpdateManagerPlugin::displayName() const { return "Update Manager"; }
QString UpdateManagerPlugin::displayNameZh() const { return QStringLiteral("更新管理器"); }
QIcon UpdateManagerPlugin::icon() const { return QIcon::fromTheme("system-software-update"); }
int UpdateManagerPlugin::defaultOrder() const { return 315; }
bool UpdateManagerPlugin::visible() const { return false; }
void UpdateManagerPlugin::activate() {}
void UpdateManagerPlugin::deactivate() {}

QWidget *UpdateManagerPlugin::widget() { return container_; }
QTableWidget *UpdateManagerPlugin::availableTable() const { return availableTable_; }
QTableWidget *UpdateManagerPlugin::historyTable() const { return historyTable_; }
QTextEdit *UpdateManagerPlugin::settingsPanel() const { return settingsPanel_; }
QTextEdit *UpdateManagerPlugin::statusPanel() const { return statusPanel_; }
QLabel *UpdateManagerPlugin::statusLabel() const { return statusLabel_; }

int UpdateManagerPlugin::availableCount() const { return available_.size(); }
int UpdateManagerPlugin::historyCount() const { return history_.size(); }

void UpdateManagerPlugin::buildUi() {
  container_ = new QWidget;
  auto *mainLayout = new QHBoxLayout(container_);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  auto *splitter = new QSplitter;

  auto *leftPanel = new QWidget;
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(4, 4, 4, 4);

  leftLayout->addWidget(new QLabel(tr("Available Updates")));
  availableTable_ = new QTableWidget(0, 5);
  availableTable_->setHorizontalHeaderLabels({tr("Name"), tr("Current"), tr("Available"), tr("Description"), tr("Status")});
  availableTable_->horizontalHeader()->setStretchLastSection(true);
  leftLayout->addWidget(availableTable_);

  leftLayout->addWidget(new QLabel(tr("Update Settings")));
  settingsPanel_ = new QTextEdit;
  settingsPanel_->setReadOnly(true);
  settingsPanel_->setPlainText(tr("Auto-check: Enabled\nCheck interval: 24 hours\nChannel: Stable"));
  leftLayout->addWidget(settingsPanel_);

  splitter->addWidget(leftPanel);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);
  rightLayout->setContentsMargins(4, 4, 4, 4);

  rightLayout->addWidget(new QLabel(tr("Update History")));
  historyTable_ = new QTableWidget(0, 6);
  historyTable_->setHorizontalHeaderLabels({tr("Name"), tr("From"), tr("To"), tr("Status"), tr("Time"), tr("Log")});
  historyTable_->horizontalHeader()->setStretchLastSection(true);
  rightLayout->addWidget(historyTable_);

  rightLayout->addWidget(new QLabel(tr("Update Status")));
  statusPanel_ = new QTextEdit;
  statusPanel_->setReadOnly(true);
  statusPanel_->setPlaceholderText(tr("No updates in progress..."));
  rightLayout->addWidget(statusPanel_);

  statusLabel_ = new QLabel(tr("Status: Ready"));
  rightLayout->addWidget(statusLabel_);

  auto *btnRow = new QHBoxLayout;
  applyBtn_ = new QPushButton(tr("Apply Update"));
  rollbackBtn_ = new QPushButton(tr("Rollback"));
  refreshBtn_ = new QPushButton(tr("Refresh"));
  exportBtn_ = new QPushButton(tr("Export Log"));
  btnRow->addWidget(applyBtn_);
  btnRow->addWidget(rollbackBtn_);
  btnRow->addWidget(refreshBtn_);
  btnRow->addWidget(exportBtn_);
  rightLayout->addLayout(btnRow);

  splitter->addWidget(rightPanel);
  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 1);
  mainLayout->addWidget(splitter);

  connect(applyBtn_, &QPushButton::clicked, this, [this]() {
    if (!available_.isEmpty()) applyUpdate(0);
  });
  connect(rollbackBtn_, &QPushButton::clicked, this, [this]() {
    if (!history_.isEmpty()) rollbackUpdate(history_.size() - 1);
  });
  connect(refreshBtn_, &QPushButton::clicked, this, &UpdateManagerPlugin::refreshStatus);
  connect(exportBtn_, &QPushButton::clicked, this, [this]() {
    QString path = QFileDialog::getSaveFileName(container_, tr("Export Log"), QString(), "JSON (*.json)");
    if (!path.isEmpty()) exportUpdateLog(path);
  });

  refreshStatus();
}

void UpdateManagerPlugin::addAvailableUpdate(const UpdateEntry &entry) {
  UpdateEntry e = entry;
  if (e.id.isEmpty()) e.id = QString("upd_%1").arg(nextId_++);
  available_.append(e);

  int row = availableTable_->rowCount();
  availableTable_->insertRow(row);
  availableTable_->setItem(row, 0, new QTableWidgetItem(e.name));
  availableTable_->setItem(row, 1, new QTableWidgetItem(e.currentVersion));
  availableTable_->setItem(row, 2, new QTableWidgetItem(e.availableVersion));
  availableTable_->setItem(row, 3, new QTableWidgetItem(e.description));
  availableTable_->setItem(row, 4, new QTableWidgetItem(e.status));

  emit updateAvailable(e.id, e.name);
  refreshStatus();
}

void UpdateManagerPlugin::removeAvailableUpdate(int index) {
  if (index < 0 || index >= available_.size()) return;
  available_.removeAt(index);
  availableTable_->removeRow(index);
  refreshStatus();
}

void UpdateManagerPlugin::applyUpdate(int index) {
  if (index < 0 || index >= available_.size()) return;
  const auto &e = available_[index];

  UpdateRecord rec;
  rec.id = QString("urec_%1").arg(nextId_++);
  rec.name = e.name;
  rec.fromVersion = e.currentVersion;
  rec.toVersion = e.availableVersion;
  rec.status = "Rejected";
  rec.timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
  rec.log = tr("Update backend required before applying %1 from %2 to %3.")
                .arg(e.name, e.currentVersion, e.availableVersion);
  history_.append(rec);

  int row = historyTable_->rowCount();
  historyTable_->insertRow(row);
  historyTable_->setItem(row, 0, new QTableWidgetItem(rec.name));
  historyTable_->setItem(row, 1, new QTableWidgetItem(rec.fromVersion));
  historyTable_->setItem(row, 2, new QTableWidgetItem(rec.toVersion));
  historyTable_->setItem(row, 3, new QTableWidgetItem(rec.status));
  historyTable_->setItem(row, 4, new QTableWidgetItem(rec.timestamp));
  historyTable_->setItem(row, 5, new QTableWidgetItem(rec.log));

  refreshStatus();
}

void UpdateManagerPlugin::rollbackUpdate(int historyIndex) {
  if (historyIndex < 0 || historyIndex >= history_.size()) return;
  const auto &orig = history_[historyIndex];
  if (orig.status != QStringLiteral("Success")) return;

  UpdateRecord rec;
  rec.id = QString("urec_%1").arg(nextId_++);
  rec.name = orig.name;
  rec.fromVersion = orig.toVersion;
  rec.toVersion = orig.fromVersion;
  rec.status = "Rolled Back";
  rec.timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
  rec.log = tr("Rolled back %1 from %2 to %3.").arg(orig.name, orig.toVersion, orig.fromVersion);
  history_.append(rec);

  int row = historyTable_->rowCount();
  historyTable_->insertRow(row);
  historyTable_->setItem(row, 0, new QTableWidgetItem(rec.name));
  historyTable_->setItem(row, 1, new QTableWidgetItem(rec.fromVersion));
  historyTable_->setItem(row, 2, new QTableWidgetItem(rec.toVersion));
  historyTable_->setItem(row, 3, new QTableWidgetItem(rec.status));
  historyTable_->setItem(row, 4, new QTableWidgetItem(rec.timestamp));
  historyTable_->setItem(row, 5, new QTableWidgetItem(rec.log));

  emit rollbackRequested(orig.id);
  refreshStatus();
}

void UpdateManagerPlugin::clearHistory() {
  history_.clear();
  historyTable_->setRowCount(0);
  refreshStatus();
}

bool UpdateManagerPlugin::exportUpdateLog(const QString &filePath) {
  if (filePath.isEmpty()) return false;

  QJsonObject root;
  root["version"] = 1;
  root["totalUpdates"] = history_.size();
  QJsonArray arr;
  for (const auto &r : history_) {
    QJsonObject obj;
    obj["id"] = r.id;
    obj["name"] = r.name;
    obj["fromVersion"] = r.fromVersion;
    obj["toVersion"] = r.toVersion;
    obj["status"] = r.status;
    obj["timestamp"] = r.timestamp;
    obj["log"] = r.log;
    arr.append(obj);
  }
  root["updates"] = arr;

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly)) return false;
  const QByteArray bytes = QJsonDocument(root).toJson();
  return file.write(bytes) == bytes.size() && file.flush();
}

void UpdateManagerPlugin::refreshStatus() {
  QString s;
  s += tr("Available updates: %1\n").arg(available_.size());
  s += tr("Update history: %1\n").arg(history_.size());
  statusPanel_->setPlainText(s);
  statusLabel_->setText(tr("Status: Ready"));
}
