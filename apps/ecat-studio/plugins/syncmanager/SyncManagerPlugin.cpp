#include "SyncManagerPlugin.h"
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

SyncManagerPlugin::SyncManagerPlugin(QObject *parent) {
  if (parent) setParent(parent);
  buildUi();
}

QString SyncManagerPlugin::id() const { return "syncmanager"; }
QString SyncManagerPlugin::displayName() const { return "Sync Manager"; }
QString SyncManagerPlugin::displayNameZh() const { return "同步管理器"; }
int SyncManagerPlugin::defaultOrder() const { return 330; }
bool SyncManagerPlugin::visible() const { return true; }

void SyncManagerPlugin::activate() {}
void SyncManagerPlugin::deactivate() {}

QWidget *SyncManagerPlugin::widget() {
  if (!containerWidget_) buildUi();
  return containerWidget_;
}

void SyncManagerPlugin::addStatus(const SyncStatusEntry &status) {
  statuses_.append(status);
  rebuildStatusTable();
  emit syncStatusChanged(status.id, status.state);
}

void SyncManagerPlugin::removeStatus(int index) {
  if (index >= 0 && index < statuses_.size()) {
    statuses_.removeAt(index);
    rebuildStatusTable();
  }
}

int SyncManagerPlugin::statusCount() const { return statuses_.size(); }

void SyncManagerPlugin::addHistoryEntry(const SyncHistoryEntry &entry) {
  history_.append(entry);
  rebuildHistoryTable();
}

int SyncManagerPlugin::historyCount() const { return history_.size(); }

void SyncManagerPlugin::addSetting(const SyncSetting &setting) {
  settings_.append(setting);
  rebuildSettingsTable();
}

void SyncManagerPlugin::updateSetting(int index, const QString &value) {
  if (index >= 0 && index < settings_.size()) {
    settings_[index].value = value;
    rebuildSettingsTable();
  }
}

int SyncManagerPlugin::settingCount() const { return settings_.size(); }

void SyncManagerPlugin::addLog(const SyncLog &log) {
  logs_.append(log);
  filteredLogs_.append(logs_.size() - 1);
  rebuildLogTable();
}

int SyncManagerPlugin::logCount() const { return logs_.size(); }

void SyncManagerPlugin::filterLogs(const QString &level, const QString &source) {
  filteredLogs_.clear();
  for (int i = 0; i < logs_.size(); ++i) {
    const auto &e = logs_[i];
    bool matchLevel = level.isEmpty() || level == "All" || e.level == level;
    bool matchSource = source.isEmpty() || e.source.contains(source, Qt::CaseInsensitive);
    if (matchLevel && matchSource) filteredLogs_.append(i);
  }
  rebuildLogTable();
}

void SyncManagerPlugin::exportReport(const QString &path) {
  QFile f(path);
  if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&f);
    out << "Sync Manager Report\n";
    out << "====================\n\n";
    out << "Active Syncs: " << statuses_.size() << "\n";
    out << "History Entries: " << history_.size() << "\n";
    out << "Settings: " << settings_.size() << "\n";
    out << "Log Entries: " << logs_.size() << "\n\n";
    out << "--- Sync Status ---\n";
    for (const auto &s : statuses_) {
      out << s.name << " [" << s.type << "] " << s.state << " "
          << s.progress << "%\n";
    }
    out << "\n--- Sync History ---\n";
    for (const auto &h : history_) {
      out << h.timestamp.toString(Qt::ISODate) << " " << h.name << " "
          << h.result << " " << h.duration << "ms\n";
    }
    out << "\n--- Settings ---\n";
    for (const auto &s : settings_) {
      out << s.name << ": " << s.value << " (default: " << s.defaultValue << ")\n";
    }
  }
}

QTableWidget *SyncManagerPlugin::statusTable() const { return statusTable_; }
QTableWidget *SyncManagerPlugin::historyTable() const { return historyTable_; }
QTableWidget *SyncManagerPlugin::settingsTable() const { return settingsTable_; }
QTableWidget *SyncManagerPlugin::logTable() const { return logTable_; }
QLabel *SyncManagerPlugin::statusLabel() const { return statusLabel_; }

void SyncManagerPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);
  tabs_ = new QTabWidget;

  auto *statusTab = new QWidget;
  auto *statusLayout = new QVBoxLayout(statusTab);
  statusTable_ = new QTableWidget;
  statusTable_->setColumnCount(7);
  statusTable_->setHorizontalHeaderLabels(
      {"ID", "Name", "Type", "State", "Progress", "Last Sync", "Message"});
  statusLayout->addWidget(statusTable_);

  refreshStatusBtn_ = new QPushButton("Refresh Status");
  statusLayout->addWidget(refreshStatusBtn_);
  tabs_->addTab(statusTab, "Sync Status");

  auto *historyTab = new QWidget;
  auto *historyLayout = new QVBoxLayout(historyTab);
  auto *historySearchRow = new QWidget;
  auto *historySearchLayout = new QHBoxLayout(historySearchRow);
  historySearchEdit_ = new QLineEdit;
  historySearchEdit_->setPlaceholderText("Search history...");
  historySearchLayout->addWidget(historySearchEdit_);
  historyLayout->addWidget(historySearchRow);

  historyTable_ = new QTableWidget;
  historyTable_->setColumnCount(6);
  historyTable_->setHorizontalHeaderLabels(
      {"Timestamp", "Sync ID", "Name", "Result", "Duration (ms)", "Details"});
  historyLayout->addWidget(historyTable_);

  clearHistoryBtn_ = new QPushButton("Clear History");
  historyLayout->addWidget(clearHistoryBtn_);
  tabs_->addTab(historyTab, "Sync History");

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
  settingsBtnLayout->addWidget(saveSettingsBtn_);
  settingsBtnLayout->addWidget(resetSettingsBtn_);
  settingsLayout->addWidget(settingsBtnRow);
  tabs_->addTab(settingsTab, "Sync Settings");

  auto *logTab = new QWidget;
  auto *logLayout = new QVBoxLayout(logTab);
  auto *logFilterRow = new QWidget;
  auto *logFilterLayout = new QHBoxLayout(logFilterRow);
  logSourceFilter_ = new QLineEdit;
  logSourceFilter_->setPlaceholderText("Filter by source...");
  logLevelFilter_ = new QComboBox;
  logLevelFilter_->addItems({"All", "info", "warning", "error"});
  filterLogBtn_ = new QPushButton("Filter");
  logFilterLayout->addWidget(logSourceFilter_);
  logFilterLayout->addWidget(logLevelFilter_);
  logFilterLayout->addWidget(filterLogBtn_);
  logLayout->addWidget(logFilterRow);

  logTable_ = new QTableWidget;
  logTable_->setColumnCount(4);
  logTable_->setHorizontalHeaderLabels(
      {"Timestamp", "Source", "Level", "Message"});
  logLayout->addWidget(logTable_);

  exportReportBtn_ = new QPushButton("Export Sync Report");
  logLayout->addWidget(exportReportBtn_);
  tabs_->addTab(logTab, "Sync Logs");

  mainLayout->addWidget(tabs_);

  statusLabel_ = new QLabel("Ready");
  mainLayout->addWidget(statusLabel_);

  rebuildStatusTable();
  rebuildHistoryTable();
  rebuildSettingsTable();
  rebuildLogTable();

  connect(filterLogBtn_, &QPushButton::clicked, this, [this]() {
    filterLogs(logLevelFilter_->currentText(), logSourceFilter_->text());
  });
  connect(exportReportBtn_, &QPushButton::clicked, this, [this]() {
    exportReport("/tmp/sync_report.txt");
  });
  connect(resetSettingsBtn_, &QPushButton::clicked, this, [this]() {
    for (int i = 0; i < settings_.size(); ++i) {
      settings_[i].value = settings_[i].defaultValue;
    }
    rebuildSettingsTable();
  });
}

void SyncManagerPlugin::rebuildStatusTable() {
  if (!statusTable_) return;
  statusTable_->setRowCount(statuses_.size());
  for (int i = 0; i < statuses_.size(); ++i) {
    const auto &s = statuses_[i];
    statusTable_->setItem(i, 0, new QTableWidgetItem(s.id));
    statusTable_->setItem(i, 1, new QTableWidgetItem(s.name));
    statusTable_->setItem(i, 2, new QTableWidgetItem(s.type));
    statusTable_->setItem(i, 3, new QTableWidgetItem(s.state));
    statusTable_->setItem(i, 4, new QTableWidgetItem(QString::number(s.progress) + "%"));
    statusTable_->setItem(
        i, 5, new QTableWidgetItem(s.lastSync.toString(Qt::ISODate)));
    statusTable_->setItem(i, 6, new QTableWidgetItem(s.message));
  }
}

void SyncManagerPlugin::rebuildHistoryTable() {
  if (!historyTable_) return;
  historyTable_->setRowCount(history_.size());
  for (int i = 0; i < history_.size(); ++i) {
    const auto &h = history_[i];
    historyTable_->setItem(
        i, 0, new QTableWidgetItem(h.timestamp.toString(Qt::ISODate)));
    historyTable_->setItem(i, 1, new QTableWidgetItem(h.syncId));
    historyTable_->setItem(i, 2, new QTableWidgetItem(h.name));
    historyTable_->setItem(i, 3, new QTableWidgetItem(h.result));
    historyTable_->setItem(i, 4, new QTableWidgetItem(QString::number(h.duration)));
    historyTable_->setItem(i, 5, new QTableWidgetItem(h.details));
  }
}

void SyncManagerPlugin::rebuildSettingsTable() {
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

void SyncManagerPlugin::rebuildLogTable() {
  if (!logTable_) return;
  logTable_->setRowCount(filteredLogs_.size());
  for (int i = 0; i < filteredLogs_.size(); ++i) {
    const auto &e = logs_[filteredLogs_[i]];
    logTable_->setItem(
        i, 0, new QTableWidgetItem(e.timestamp.toString(Qt::ISODate)));
    logTable_->setItem(i, 1, new QTableWidgetItem(e.source));
    logTable_->setItem(i, 2, new QTableWidgetItem(e.level));
    logTable_->setItem(i, 3, new QTableWidgetItem(e.message));
  }
  if (statusLabel_)
    statusLabel_->setText(
        QString("Sync Logs: %1 entries shown").arg(filteredLogs_.size()));
}
