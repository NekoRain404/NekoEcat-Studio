#include "ReplicationManagerPlugin.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

ReplicationManagerPlugin::ReplicationManagerPlugin(QObject *parent) {
  if (parent) setParent(parent);
  buildUi();
}

QString ReplicationManagerPlugin::id() const { return "replicationmanager"; }
QString ReplicationManagerPlugin::displayName() const { return "Replication Manager"; }
QString ReplicationManagerPlugin::displayNameZh() const { return "复制管理器"; }
int ReplicationManagerPlugin::defaultOrder() const { return 335; }
bool ReplicationManagerPlugin::visible() const { return false; }

void ReplicationManagerPlugin::activate() {}
void ReplicationManagerPlugin::deactivate() {}

QWidget *ReplicationManagerPlugin::widget() {
  if (!containerWidget_) buildUi();
  return containerWidget_;
}

void ReplicationManagerPlugin::addTarget(const ReplicationTarget &target) {
  targets_.append(target);
  rebuildTargetTable();
  emit targetAdded(target.id);
}

void ReplicationManagerPlugin::removeTarget(int index) {
  if (index >= 0 && index < targets_.size()) {
    QString tid = targets_[index].id;
    targets_.removeAt(index);
    rebuildTargetTable();
    emit targetRemoved(tid);
  }
}

int ReplicationManagerPlugin::targetCount() const { return targets_.size(); }

void ReplicationManagerPlugin::updateStatus(const ReplicationStatus &status) {
  for (int i = 0; i < statuses_.size(); ++i) {
    if (statuses_[i].targetId == status.targetId) {
      statuses_[i] = status;
      rebuildStatusTable();
      return;
    }
  }
  statuses_.append(status);
  rebuildStatusTable();
}

int ReplicationManagerPlugin::statusCount() const { return statuses_.size(); }

void ReplicationManagerPlugin::addHistoryEntry(const ReplicationHistoryEntry &entry) {
  history_.append(entry);
  rebuildHistoryTable();
}

int ReplicationManagerPlugin::historyCount() const { return history_.size(); }

void ReplicationManagerPlugin::addSetting(const ReplicationSetting &setting) {
  settings_.append(setting);
  rebuildSettingsTable();
}

void ReplicationManagerPlugin::updateSetting(int index, const QString &value) {
  if (index >= 0 && index < settings_.size()) {
    settings_[index].value = value;
    rebuildSettingsTable();
  }
}

int ReplicationManagerPlugin::settingCount() const { return settings_.size(); }

bool ReplicationManagerPlugin::exportReport(const QString &path) {
  if (path.isEmpty()) return false;
  QFile f(path);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

  QTextStream out(&f);
  out << "Replication Manager Report\n";
  out << "==========================\n\n";
  out << "Targets: " << targets_.size() << "\n";
  out << "History Entries: " << history_.size() << "\n";
  out << "Settings: " << settings_.size() << "\n\n";
  out << "--- Replication Targets ---\n";
  for (const auto &t : targets_) {
    out << t.name << " [" << t.type << "] " << t.endpoint << " "
        << (t.enabled ? "enabled" : "disabled") << "\n";
  }
  out << "\n--- Status ---\n";
  for (const auto &s : statuses_) {
    out << s.targetName << ": " << s.state << " " << s.progress << "%\n";
  }
  out << "\n--- History ---\n";
  for (const auto &h : history_) {
    out << h.timestamp.toString(Qt::ISODate) << " " << h.targetName << " "
        << h.result << " " << h.objectsReplicated << " objects\n";
  }
  return out.status() == QTextStream::Ok && f.flush();
}

QTableWidget *ReplicationManagerPlugin::targetTable() const { return targetTable_; }
QTableWidget *ReplicationManagerPlugin::statusTable() const { return statusTable_; }
QTableWidget *ReplicationManagerPlugin::historyTable() const { return historyTable_; }
QTableWidget *ReplicationManagerPlugin::settingsTable() const { return settingsTable_; }
QLabel *ReplicationManagerPlugin::statusLabel() const { return statusLabel_; }

void ReplicationManagerPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);
  tabs_ = new QTabWidget;

  auto *targetTab = new QWidget;
  auto *targetLayout = new QVBoxLayout(targetTab);
  auto *targetSearchRow = new QWidget;
  auto *targetSearchLayout = new QHBoxLayout(targetSearchRow);
  targetSearchEdit_ = new QLineEdit;
  targetSearchEdit_->setPlaceholderText("Search targets...");
  targetSearchLayout->addWidget(targetSearchEdit_);
  targetLayout->addWidget(targetSearchRow);

  targetTable_ = new QTableWidget;
  targetTable_->setColumnCount(6);
  targetTable_->setHorizontalHeaderLabels(
      {"ID", "Name", "Endpoint", "Type", "Enabled", "Last Replicated"});
  targetLayout->addWidget(targetTable_);

  auto *targetBtnRow = new QWidget;
  auto *targetBtnLayout = new QHBoxLayout(targetBtnRow);
  addTargetBtn_ = new QPushButton("Add Target");
  removeTargetBtn_ = new QPushButton("Remove Target");
  targetBtnLayout->addWidget(addTargetBtn_);
  targetBtnLayout->addWidget(removeTargetBtn_);
  targetLayout->addWidget(targetBtnRow);
  tabs_->addTab(targetTab, "Replication Targets");

  auto *statusTab = new QWidget;
  auto *statusLayout = new QVBoxLayout(statusTab);
  statusTable_ = new QTableWidget;
  statusTable_->setColumnCount(6);
  statusTable_->setHorizontalHeaderLabels(
      {"Target ID", "Name", "State", "Progress", "Last Update", "Message"});
  statusLayout->addWidget(statusTable_);

  refreshStatusBtn_ = new QPushButton("Refresh Status");
  statusLayout->addWidget(refreshStatusBtn_);
  tabs_->addTab(statusTab, "Replication Status");

  auto *historyTab = new QWidget;
  auto *historyLayout = new QVBoxLayout(historyTab);
  historyTable_ = new QTableWidget;
  historyTable_->setColumnCount(6);
  historyTable_->setHorizontalHeaderLabels(
      {"Timestamp", "Target ID", "Name", "Result", "Objects", "Details"});
  historyLayout->addWidget(historyTable_);

  clearHistoryBtn_ = new QPushButton("Clear History");
  historyLayout->addWidget(clearHistoryBtn_);
  tabs_->addTab(historyTab, "Replication History");

  auto *settingsTab = new QWidget;
  auto *settingsLayout = new QVBoxLayout(settingsTab);
  settingsTable_ = new QTableWidget;
  settingsTable_->setColumnCount(5);
  settingsTable_->setHorizontalHeaderLabels(
      {"ID", "Name", "Description", "Value", "Default"});
  settingsLayout->addWidget(settingsTable_);

  auto *settingsBtnRow = new QWidget;
  auto *settingsBtnLayout = new QHBoxLayout(settingsBtnRow);
  saveSettingsBtn_ = new QPushButton("Save Settings");
  resetSettingsBtn_ = new QPushButton("Reset to Defaults");
  exportReportBtn_ = new QPushButton("Export Replication Report");
  settingsBtnLayout->addWidget(saveSettingsBtn_);
  settingsBtnLayout->addWidget(resetSettingsBtn_);
  settingsBtnLayout->addWidget(exportReportBtn_);
  settingsLayout->addWidget(settingsBtnRow);
  tabs_->addTab(settingsTab, "Replication Settings");

  mainLayout->addWidget(tabs_);

  statusLabel_ = new QLabel("Ready");
  mainLayout->addWidget(statusLabel_);

  rebuildTargetTable();
  rebuildStatusTable();
  rebuildHistoryTable();
  rebuildSettingsTable();

  connect(addTargetBtn_, &QPushButton::clicked, this, [this]() {
    ReplicationTarget t;
    t.id = "t" + QString::number(targets_.size() + 1);
    t.name = "New Target";
    t.endpoint = "";
    t.type = "Full";
    t.enabled = true;
    t.lastReplicated = QDateTime::currentDateTime();
    addTarget(t);
  });
  connect(removeTargetBtn_, &QPushButton::clicked, this, [this]() {
    int row = targetTable_->currentRow();
    if (row >= 0) removeTarget(row);
  });
  connect(resetSettingsBtn_, &QPushButton::clicked, this, [this]() {
    for (int i = 0; i < settings_.size(); ++i) {
      settings_[i].value = settings_[i].defaultValue;
    }
    rebuildSettingsTable();
  });
  connect(exportReportBtn_, &QPushButton::clicked, this, [this]() {
    exportReport("/tmp/replication_report.txt");
  });
}

void ReplicationManagerPlugin::rebuildTargetTable() {
  if (!targetTable_) return;
  targetTable_->setRowCount(targets_.size());
  for (int i = 0; i < targets_.size(); ++i) {
    const auto &t = targets_[i];
    targetTable_->setItem(i, 0, new QTableWidgetItem(t.id));
    targetTable_->setItem(i, 1, new QTableWidgetItem(t.name));
    targetTable_->setItem(i, 2, new QTableWidgetItem(t.endpoint));
    targetTable_->setItem(i, 3, new QTableWidgetItem(t.type));
    targetTable_->setItem(i, 4, new QTableWidgetItem(t.enabled ? "Yes" : "No"));
    targetTable_->setItem(
        i, 5, new QTableWidgetItem(t.lastReplicated.toString(Qt::ISODate)));
  }
}

void ReplicationManagerPlugin::rebuildStatusTable() {
  if (!statusTable_) return;
  statusTable_->setRowCount(statuses_.size());
  for (int i = 0; i < statuses_.size(); ++i) {
    const auto &s = statuses_[i];
    statusTable_->setItem(i, 0, new QTableWidgetItem(s.targetId));
    statusTable_->setItem(i, 1, new QTableWidgetItem(s.targetName));
    statusTable_->setItem(i, 2, new QTableWidgetItem(s.state));
    statusTable_->setItem(i, 3, new QTableWidgetItem(QString::number(s.progress) + "%"));
    statusTable_->setItem(
        i, 4, new QTableWidgetItem(s.lastUpdate.toString(Qt::ISODate)));
    statusTable_->setItem(i, 5, new QTableWidgetItem(s.message));
  }
}

void ReplicationManagerPlugin::rebuildHistoryTable() {
  if (!historyTable_) return;
  historyTable_->setRowCount(history_.size());
  for (int i = 0; i < history_.size(); ++i) {
    const auto &h = history_[i];
    historyTable_->setItem(
        i, 0, new QTableWidgetItem(h.timestamp.toString(Qt::ISODate)));
    historyTable_->setItem(i, 1, new QTableWidgetItem(h.targetId));
    historyTable_->setItem(i, 2, new QTableWidgetItem(h.targetName));
    historyTable_->setItem(i, 3, new QTableWidgetItem(h.result));
    historyTable_->setItem(
        i, 4, new QTableWidgetItem(QString::number(h.objectsReplicated)));
    historyTable_->setItem(i, 5, new QTableWidgetItem(h.details));
  }
}

void ReplicationManagerPlugin::rebuildSettingsTable() {
  if (!settingsTable_) return;
  settingsTable_->setRowCount(settings_.size());
  for (int i = 0; i < settings_.size(); ++i) {
    const auto &s = settings_[i];
    settingsTable_->setItem(i, 0, new QTableWidgetItem(s.id));
    settingsTable_->setItem(i, 1, new QTableWidgetItem(s.name));
    settingsTable_->setItem(i, 2, new QTableWidgetItem(s.description));
    settingsTable_->setItem(i, 3, new QTableWidgetItem(s.value));
    settingsTable_->setItem(i, 4, new QTableWidgetItem(s.defaultValue));
  }
}
