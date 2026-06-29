#include "DataLoggerPlugin.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

DataLoggerPlugin::DataLoggerPlugin(QObject *parent) {
  if (parent) setParent(parent);
  buildUi();
}

QString DataLoggerPlugin::id() const { return "datalogger"; }
QString DataLoggerPlugin::displayName() const { return "Data Logger"; }
QString DataLoggerPlugin::displayNameZh() const { return "数据记录器"; }
int DataLoggerPlugin::defaultOrder() const { return 245; }
bool DataLoggerPlugin::visible() const { return false; }

void DataLoggerPlugin::activate() {}
void DataLoggerPlugin::deactivate() {}

QWidget *DataLoggerPlugin::widget() {
  if (!containerWidget_) buildUi();
  return containerWidget_;
}

void DataLoggerPlugin::addFilter(const LogFilter &filter) {
  filters_.append(filter);
  rebuildFiltersTable();
}

void DataLoggerPlugin::removeFilter(int index) {
  if (index >= 0 && index < filters_.size()) {
    filters_.removeAt(index);
    rebuildFiltersTable();
  }
}

int DataLoggerPlugin::filterCount() const { return filters_.size(); }

void DataLoggerPlugin::addLogEntry(const LogEntry &entry) {
  logEntries_.append(entry);
  updateLogViewer();
  emit logEntryAdded(entry);
  if (statusLabel_) statusLabel_->setText(tr("%1 entries").arg(logEntries_.size()));
}

int DataLoggerPlugin::logEntryCount() const { return logEntries_.size(); }

void DataLoggerPlugin::searchLogs(const QString &query) {
  activeSearchQuery_ = query;
  updateLogViewer();
}

void DataLoggerPlugin::filterByLevel(const QString &level) {
  activeLevelFilter_ = level;
  updateLogViewer();
}

void DataLoggerPlugin::clearLogEntries() {
  logEntries_.clear();
  updateLogViewer();
  if (statusLabel_) statusLabel_->setText(tr("%1 entries").arg(logEntries_.size()));
}

void DataLoggerPlugin::addLogFile(const LogFile &file) {
  logFiles_.append(file);
  rebuildLogFilesTable();
}

void DataLoggerPlugin::removeLogFile(int index) {
  if (index >= 0 && index < logFiles_.size()) {
    logFiles_.removeAt(index);
    rebuildLogFilesTable();
  }
}

int DataLoggerPlugin::logFileCount() const { return logFiles_.size(); }

void DataLoggerPlugin::setMaxFileSize(qint64 bytes) {
  maxFileSize_ = bytes;
  if (maxSizeEdit_) maxSizeEdit_->setText(QString::number(bytes));
}

qint64 DataLoggerPlugin::maxFileSize() const { return maxFileSize_; }

void DataLoggerPlugin::setMaxFileCount(int count) {
  maxFileCount_ = count;
  if (maxFilesEdit_) maxFilesEdit_->setText(QString::number(count));
}

int DataLoggerPlugin::maxFileCount() const { return maxFileCount_; }

QString DataLoggerPlugin::exportLogData() const {
  QJsonObject root;
  QJsonArray entries;
  for (const auto &e : logEntries_) {
    QJsonObject obj;
    obj["timestamp"] = e.timestamp.toString(Qt::ISODate);
    obj["source"] = e.source;
    obj["level"] = e.level;
    obj["message"] = e.message;
    entries.append(obj);
  }
  root["entries"] = entries;
  QJsonObject config;
  config["maxFileSize"] = maxFileSize_;
  config["maxFileCount"] = maxFileCount_;
  root["config"] = config;
  return QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact));
}

QMap<QString, int> DataLoggerPlugin::getSourceStatistics() const {
  QMap<QString, int> stats;
  for (const auto &e : logEntries_) {
    stats[e.source]++;
  }
  return stats;
}

QTabWidget *DataLoggerPlugin::tabs() const { return tabs_; }
QTableWidget *DataLoggerPlugin::filtersTable() const { return filtersTable_; }
QTableWidget *DataLoggerPlugin::logFilesTable() const { return logFilesTable_; }
QTextEdit *DataLoggerPlugin::logViewer() const { return logViewer_; }
QTableWidget *DataLoggerPlugin::statisticsTable() const { return statisticsTable_; }
QLabel *DataLoggerPlugin::statusLabel() const { return statusLabel_; }

void DataLoggerPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);

  tabs_ = new QTabWidget;

  auto *configTab = new QWidget;
  auto *configLayout = new QVBoxLayout(configTab);

  filtersTable_ = new QTableWidget;
  filtersTable_->setColumnCount(4);
  filtersTable_->setHorizontalHeaderLabels({"Name", "Source", "Level", "Enabled"});
  configLayout->addWidget(filtersTable_);

  auto *filterBtnRow = new QWidget;
  auto *filterBtnLayout = new QHBoxLayout(filterBtnRow);
  addFilterBtn_ = new QPushButton("Add");
  removeFilterBtn_ = new QPushButton("Remove");
  toggleFilterBtn_ = new QPushButton("Toggle");
  filterBtnLayout->addWidget(addFilterBtn_);
  filterBtnLayout->addWidget(removeFilterBtn_);
  filterBtnLayout->addWidget(toggleFilterBtn_);
  configLayout->addWidget(filterBtnRow);

  auto *rotationRow = new QWidget;
  auto *rotationLayout = new QHBoxLayout(rotationRow);
  rotationLayout->addWidget(new QLabel("Max Size (bytes):"));
  maxSizeEdit_ = new QLineEdit(QString::number(maxFileSize_));
  rotationLayout->addWidget(maxSizeEdit_);
  rotationLayout->addWidget(new QLabel("Max Files:"));
  maxFilesEdit_ = new QLineEdit(QString::number(maxFileCount_));
  rotationLayout->addWidget(maxFilesEdit_);
  configLayout->addWidget(rotationRow);

  tabs_->addTab(configTab, "Configuration");

  auto *filesTab = new QWidget;
  auto *filesLayout = new QVBoxLayout(filesTab);

  logFilesTable_ = new QTableWidget;
  logFilesTable_->setColumnCount(4);
  logFilesTable_->setHorizontalHeaderLabels({"Path", "Size", "Created", "Entries"});
  filesLayout->addWidget(logFilesTable_);

  auto *filesBtnRow = new QWidget;
  auto *filesBtnLayout = new QHBoxLayout(filesBtnRow);
  refreshFilesBtn_ = new QPushButton("Refresh");
  deleteFileBtn_ = new QPushButton("Delete");
  filesBtnLayout->addWidget(refreshFilesBtn_);
  filesBtnLayout->addWidget(deleteFileBtn_);
  filesLayout->addWidget(filesBtnRow);

  tabs_->addTab(filesTab, "Log Files");

  auto *viewerTab = new QWidget;
  auto *viewerLayout = new QVBoxLayout(viewerTab);

  auto *searchRow = new QWidget;
  auto *searchLayout = new QHBoxLayout(searchRow);
  searchEdit_ = new QLineEdit;
  searchEdit_->setPlaceholderText("Search logs...");
  levelFilterCombo_ = new QComboBox;
  levelFilterCombo_->addItems({"All", "debug", "info", "warning", "error"});
  searchLayout->addWidget(searchEdit_);
  searchLayout->addWidget(levelFilterCombo_);
  viewerLayout->addWidget(searchRow);

  logViewer_ = new QTextEdit;
  logViewer_->setReadOnly(true);
  viewerLayout->addWidget(logViewer_);

  tabs_->addTab(viewerTab, "Log Viewer");

  auto *analysisTab = new QWidget;
  auto *analysisLayout = new QVBoxLayout(analysisTab);

  statisticsTable_ = new QTableWidget;
  statisticsTable_->setColumnCount(3);
  statisticsTable_->setHorizontalHeaderLabels({"Source", "Count", "Last Seen"});
  analysisLayout->addWidget(statisticsTable_);

  auto *statsBtnRow = new QWidget;
  auto *statsBtnLayout = new QHBoxLayout(statsBtnRow);
  refreshStatsBtn_ = new QPushButton("Refresh");
  statsBtnLayout->addWidget(refreshStatsBtn_);
  analysisLayout->addWidget(statsBtnRow);

  tabs_->addTab(analysisTab, "Analysis");

  mainLayout->addWidget(tabs_);

  statusLabel_ = new QLabel("Ready");
  mainLayout->addWidget(statusLabel_);

  connect(addFilterBtn_, &QPushButton::clicked, this, [this]() {
    LogFilter f;
    f.name = "New Filter";
    f.source = "*";
    f.level = "info";
    f.enabled = true;
    addFilter(f);
  });
  connect(removeFilterBtn_, &QPushButton::clicked, this, [this]() {
    int row = filtersTable_->currentRow();
    if (row >= 0) removeFilter(row);
  });
  connect(toggleFilterBtn_, &QPushButton::clicked, this, [this]() {
    int row = filtersTable_->currentRow();
    if (row >= 0 && row < filters_.size()) {
      filters_[row].enabled = !filters_[row].enabled;
      rebuildFiltersTable();
    }
  });
  connect(refreshStatsBtn_, &QPushButton::clicked, this, [this]() {
    rebuildStatisticsTable();
  });
  connect(searchEdit_, &QLineEdit::textChanged, this, [this](const QString &text) {
    searchLogs(text);
  });
  connect(levelFilterCombo_, &QComboBox::currentTextChanged, this, [this](const QString &level) {
    filterByLevel(level == "All" ? QString() : level);
  });
}

void DataLoggerPlugin::rebuildFiltersTable() {
  if (!filtersTable_) return;
  filtersTable_->setRowCount(filters_.size());
  for (int i = 0; i < filters_.size(); ++i) {
    const auto &f = filters_[i];
    filtersTable_->setItem(i, 0, new QTableWidgetItem(f.name));
    filtersTable_->setItem(i, 1, new QTableWidgetItem(f.source));
    filtersTable_->setItem(i, 2, new QTableWidgetItem(f.level));
    filtersTable_->setItem(i, 3, new QTableWidgetItem(f.enabled ? "Yes" : "No"));
  }
}

void DataLoggerPlugin::rebuildLogFilesTable() {
  if (!logFilesTable_) return;
  logFilesTable_->setRowCount(logFiles_.size());
  for (int i = 0; i < logFiles_.size(); ++i) {
    const auto &f = logFiles_[i];
    logFilesTable_->setItem(i, 0, new QTableWidgetItem(f.path));
    logFilesTable_->setItem(i, 1, new QTableWidgetItem(QString::number(f.sizeBytes)));
    logFilesTable_->setItem(i, 2, new QTableWidgetItem(f.createdAt.toString(Qt::ISODate)));
    logFilesTable_->setItem(i, 3, new QTableWidgetItem(QString::number(f.entryCount)));
  }
}

void DataLoggerPlugin::rebuildStatisticsTable() {
  if (!statisticsTable_) return;
  auto stats = getSourceStatistics();
  auto it = stats.constBegin();
  statisticsTable_->setRowCount(stats.size());
  int row = 0;
  while (it != stats.constEnd()) {
    QDateTime lastSeen;
    for (const auto &e : logEntries_) {
      if (e.source == it.key() && e.timestamp > lastSeen) lastSeen = e.timestamp;
    }
    statisticsTable_->setItem(row, 0, new QTableWidgetItem(it.key()));
    statisticsTable_->setItem(row, 1, new QTableWidgetItem(QString::number(it.value())));
    statisticsTable_->setItem(row, 2, new QTableWidgetItem(lastSeen.toString(Qt::ISODate)));
    ++it;
    ++row;
  }
}

void DataLoggerPlugin::updateLogViewer() {
  if (!logViewer_) return;
  logViewer_->clear();
  for (const auto &e : logEntries_) {
    if (!activeLevelFilter_.isEmpty() && e.level != activeLevelFilter_) continue;
    if (!activeSearchQuery_.isEmpty() &&
        !e.message.contains(activeSearchQuery_, Qt::CaseInsensitive) &&
        !e.source.contains(activeSearchQuery_, Qt::CaseInsensitive)) continue;
    logViewer_->append(e.timestamp.toString(Qt::ISODate) + " [" + e.level.toUpper() + "] " +
                       e.source + ": " + e.message);
  }
}
