#include "IntegrationHubPlugin.h"
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

IntegrationHubPlugin::IntegrationHubPlugin(QObject *parent) {
  if (parent) setParent(parent);
  auto now = QDateTime::currentDateTime();
  connections_ = {
      {"c1", "PLC Gateway", "OPC-UA", "opc.tcp://192.168.1.100:4840", "Connected", now},
      {"c2", "SCADA System", "MQTT", "mqtt://192.168.1.101:1883", "Connected", now},
      {"c3", "ERP Connector", "REST", "https://192.168.1.102/api", "Disconnected", now},
  };
  mappings_ = {
      {"m1", "Slave1.PDO.Input", "PLC.DataBlock1.Word0", "Direct", true},
      {"m2", "Slave2.PDO.Output", "PLC.DataBlock2.Word0", "Scale(x*10)", true},
      {"m3", "SCADA.Tag1", "Slave1.Status", "Enum->Int", false},
  };
  syncStatuses_ = {
      {"c1", "PLC Gateway", "Synced", 100, now, "All data up to date"},
      {"c2", "SCADA System", "Syncing", 67, now, "Transferring batch 3/5"},
      {"c3", "ERP Connector", "Error", 0, now, "Connection refused"},
  };
  logs_ = {
      {now, "PLC Gateway", "info", "Sync completed", "128 bytes transferred"},
      {now, "SCADA System", "warning", "Slow response", "Latency > 200ms"},
      {now, "ERP Connector", "error", "Connection failed", "ECONNREFUSED"},
  };
  filteredLogs_ = {0, 1, 2};
  buildUi();
}

QString IntegrationHubPlugin::id() const { return "integrationhub"; }
QString IntegrationHubPlugin::displayName() const { return "Integration Hub"; }
QString IntegrationHubPlugin::displayNameZh() const { return "集成中心"; }
int IntegrationHubPlugin::defaultOrder() const { return 325; }
bool IntegrationHubPlugin::visible() const { return true; }

void IntegrationHubPlugin::activate() {}
void IntegrationHubPlugin::deactivate() {}

QWidget *IntegrationHubPlugin::widget() {
  if (!containerWidget_) buildUi();
  return containerWidget_;
}

void IntegrationHubPlugin::addConnection(const SystemConnection &conn) {
  connections_.append(conn);
  rebuildConnectionTable();
  emit connectionAdded(conn.id);
}

void IntegrationHubPlugin::removeConnection(int index) {
  if (index >= 0 && index < connections_.size()) {
    QString cid = connections_[index].id;
    connections_.removeAt(index);
    rebuildConnectionTable();
    emit connectionRemoved(cid);
  }
}

int IntegrationHubPlugin::connectionCount() const { return connections_.size(); }

void IntegrationHubPlugin::addMapping(const DataMapping &mapping) {
  mappings_.append(mapping);
  rebuildMappingTable();
}

void IntegrationHubPlugin::removeMapping(int index) {
  if (index >= 0 && index < mappings_.size()) {
    mappings_.removeAt(index);
    rebuildMappingTable();
  }
}

int IntegrationHubPlugin::mappingCount() const { return mappings_.size(); }

void IntegrationHubPlugin::updateSyncStatus(const SyncStatus &status) {
  for (int i = 0; i < syncStatuses_.size(); ++i) {
    if (syncStatuses_[i].connectionId == status.connectionId) {
      syncStatuses_[i] = status;
      rebuildSyncTable();
      return;
    }
  }
  syncStatuses_.append(status);
  rebuildSyncTable();
}

int IntegrationHubPlugin::syncStatusCount() const { return syncStatuses_.size(); }

void IntegrationHubPlugin::addLog(const IntegrationLog &log) {
  logs_.append(log);
  filteredLogs_.append(logs_.size() - 1);
  rebuildLogTable();
}

int IntegrationHubPlugin::logCount() const { return logs_.size(); }

void IntegrationHubPlugin::filterLogs(const QString &level, const QString &source) {
  filteredLogs_.clear();
  for (int i = 0; i < logs_.size(); ++i) {
    const auto &e = logs_[i];
    bool matchLevel = level.isEmpty() || level == "All" || e.level == level;
    bool matchSource = source.isEmpty() || e.source.contains(source, Qt::CaseInsensitive);
    if (matchLevel && matchSource) filteredLogs_.append(i);
  }
  rebuildLogTable();
}

bool IntegrationHubPlugin::exportReport(const QString &path) {
  if (path.isEmpty()) return false;
  QFile f(path);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

  QTextStream out(&f);
  out << "Integration Hub Report\n";
  out << "======================\n\n";
  out << "Connections: " << connections_.size() << "\n";
  out << "Mappings: " << mappings_.size() << "\n";
  out << "Log Entries: " << logs_.size() << "\n\n";
  out << "--- Connections ---\n";
  for (const auto &c : connections_) {
    out << c.name << " [" << c.type << "] " << c.status << "\n";
  }
  out << "\n--- Data Mappings ---\n";
  for (const auto &m : mappings_) {
    out << m.source << " -> " << m.destination << " (" << m.transformation
        << ") " << (m.enabled ? "enabled" : "disabled") << "\n";
  }
  out << "\n--- Integration Log ---\n";
  for (const auto &e : logs_) {
    out << e.timestamp.toString(Qt::ISODate) << " [" << e.level << "] "
        << e.source << ": " << e.message << "\n";
  }
  return out.status() == QTextStream::Ok && f.flush();
}

QTableWidget *IntegrationHubPlugin::connectionTable() const { return connectionTable_; }
QTableWidget *IntegrationHubPlugin::mappingTable() const { return mappingTable_; }
QTableWidget *IntegrationHubPlugin::syncTable() const { return syncTable_; }
QTableWidget *IntegrationHubPlugin::logTable() const { return logTable_; }
QLabel *IntegrationHubPlugin::statusLabel() const { return statusLabel_; }

void IntegrationHubPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);
  tabs_ = new QTabWidget;

  auto *connTab = new QWidget;
  auto *connLayout = new QVBoxLayout(connTab);
  auto *connSearchRow = new QWidget;
  auto *connSearchLayout = new QHBoxLayout(connSearchRow);
  connSearchEdit_ = new QLineEdit;
  connSearchEdit_->setPlaceholderText("Search connections...");
  connSearchLayout->addWidget(connSearchEdit_);
  connLayout->addWidget(connSearchRow);

  connectionTable_ = new QTableWidget;
  connectionTable_->setColumnCount(6);
  connectionTable_->setHorizontalHeaderLabels(
      {"ID", "Name", "Type", "Endpoint", "Status", "Last Sync"});
  connLayout->addWidget(connectionTable_);

  auto *connBtnRow = new QWidget;
  auto *connBtnLayout = new QHBoxLayout(connBtnRow);
  addConnBtn_ = new QPushButton("Add Connection");
  removeConnBtn_ = new QPushButton("Remove Connection");
  testConnBtn_ = new QPushButton("Test Connection");
  connBtnLayout->addWidget(addConnBtn_);
  connBtnLayout->addWidget(removeConnBtn_);
  connBtnLayout->addWidget(testConnBtn_);
  connLayout->addWidget(connBtnRow);
  tabs_->addTab(connTab, "Connections");

  auto *mapTab = new QWidget;
  auto *mapLayout = new QVBoxLayout(mapTab);
  mappingTable_ = new QTableWidget;
  mappingTable_->setColumnCount(5);
  mappingTable_->setHorizontalHeaderLabels(
      {"ID", "Source", "Destination", "Transformation", "Enabled"});
  mapLayout->addWidget(mappingTable_);

  auto *mapBtnRow = new QWidget;
  auto *mapBtnLayout = new QHBoxLayout(mapBtnRow);
  addMappingBtn_ = new QPushButton("Add Mapping");
  removeMappingBtn_ = new QPushButton("Remove Mapping");
  mapBtnLayout->addWidget(addMappingBtn_);
  mapBtnLayout->addWidget(removeMappingBtn_);
  mapLayout->addWidget(mapBtnRow);
  tabs_->addTab(mapTab, "Data Mapping");

  auto *syncTab = new QWidget;
  auto *syncLayout = new QVBoxLayout(syncTab);
  syncTable_ = new QTableWidget;
  syncTable_->setColumnCount(6);
  syncTable_->setHorizontalHeaderLabels(
      {"Connection ID", "Name", "State", "Progress", "Last Update", "Message"});
  syncLayout->addWidget(syncTable_);

  refreshSyncBtn_ = new QPushButton("Refresh Sync Status");
  syncLayout->addWidget(refreshSyncBtn_);
  tabs_->addTab(syncTab, "Sync Status");

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
  logTable_->setColumnCount(5);
  logTable_->setHorizontalHeaderLabels(
      {"Timestamp", "Source", "Level", "Message", "Details"});
  logLayout->addWidget(logTable_);

  exportReportBtn_ = new QPushButton("Export Integration Report");
  logLayout->addWidget(exportReportBtn_);
  tabs_->addTab(logTab, "Integration Logs");

  mainLayout->addWidget(tabs_);

  statusLabel_ = new QLabel("Ready");
  mainLayout->addWidget(statusLabel_);

  rebuildConnectionTable();
  rebuildMappingTable();
  rebuildSyncTable();
  rebuildLogTable();

  connect(addConnBtn_, &QPushButton::clicked, this, [this]() {
    SystemConnection c;
    c.id = "c" + QString::number(connections_.size() + 1);
    c.name = "New Connection";
    c.type = "REST";
    c.endpoint = "";
    c.status = "Disconnected";
    c.lastSync = QDateTime::currentDateTime();
    addConnection(c);
  });
  connect(removeConnBtn_, &QPushButton::clicked, this, [this]() {
    int row = connectionTable_->currentRow();
    if (row >= 0) removeConnection(row);
  });
  connect(addMappingBtn_, &QPushButton::clicked, this, [this]() {
    DataMapping m;
    m.id = "m" + QString::number(mappings_.size() + 1);
    m.source = "source";
    m.destination = "destination";
    m.transformation = "Direct";
    m.enabled = true;
    addMapping(m);
  });
  connect(removeMappingBtn_, &QPushButton::clicked, this, [this]() {
    int row = mappingTable_->currentRow();
    if (row >= 0) removeMapping(row);
  });
  connect(filterLogBtn_, &QPushButton::clicked, this, [this]() {
    filterLogs(logLevelFilter_->currentText(), logSourceFilter_->text());
  });
  connect(exportReportBtn_, &QPushButton::clicked, this, [this]() {
    exportReport("/tmp/integration_report.txt");
  });
}

void IntegrationHubPlugin::rebuildConnectionTable() {
  if (!connectionTable_) return;
  connectionTable_->setRowCount(connections_.size());
  for (int i = 0; i < connections_.size(); ++i) {
    const auto &c = connections_[i];
    connectionTable_->setItem(i, 0, new QTableWidgetItem(c.id));
    connectionTable_->setItem(i, 1, new QTableWidgetItem(c.name));
    connectionTable_->setItem(i, 2, new QTableWidgetItem(c.type));
    connectionTable_->setItem(i, 3, new QTableWidgetItem(c.endpoint));
    connectionTable_->setItem(i, 4, new QTableWidgetItem(c.status));
    connectionTable_->setItem(
        i, 5, new QTableWidgetItem(c.lastSync.toString(Qt::ISODate)));
  }
}

void IntegrationHubPlugin::rebuildMappingTable() {
  if (!mappingTable_) return;
  mappingTable_->setRowCount(mappings_.size());
  for (int i = 0; i < mappings_.size(); ++i) {
    const auto &m = mappings_[i];
    mappingTable_->setItem(i, 0, new QTableWidgetItem(m.id));
    mappingTable_->setItem(i, 1, new QTableWidgetItem(m.source));
    mappingTable_->setItem(i, 2, new QTableWidgetItem(m.destination));
    mappingTable_->setItem(i, 3, new QTableWidgetItem(m.transformation));
    mappingTable_->setItem(i, 4, new QTableWidgetItem(m.enabled ? "Yes" : "No"));
  }
}

void IntegrationHubPlugin::rebuildSyncTable() {
  if (!syncTable_) return;
  syncTable_->setRowCount(syncStatuses_.size());
  for (int i = 0; i < syncStatuses_.size(); ++i) {
    const auto &s = syncStatuses_[i];
    syncTable_->setItem(i, 0, new QTableWidgetItem(s.connectionId));
    syncTable_->setItem(i, 1, new QTableWidgetItem(s.connectionName));
    syncTable_->setItem(i, 2, new QTableWidgetItem(s.state));
    syncTable_->setItem(i, 3, new QTableWidgetItem(QString::number(s.progress) + "%"));
    syncTable_->setItem(
        i, 4, new QTableWidgetItem(s.lastUpdate.toString(Qt::ISODate)));
    syncTable_->setItem(i, 5, new QTableWidgetItem(s.message));
  }
}

void IntegrationHubPlugin::rebuildLogTable() {
  if (!logTable_) return;
  logTable_->setRowCount(filteredLogs_.size());
  for (int i = 0; i < filteredLogs_.size(); ++i) {
    const auto &e = logs_[filteredLogs_[i]];
    logTable_->setItem(
        i, 0, new QTableWidgetItem(e.timestamp.toString(Qt::ISODate)));
    logTable_->setItem(i, 1, new QTableWidgetItem(e.source));
    logTable_->setItem(i, 2, new QTableWidgetItem(e.level));
    logTable_->setItem(i, 3, new QTableWidgetItem(e.message));
    logTable_->setItem(i, 4, new QTableWidgetItem(e.details));
  }
  if (statusLabel_)
    statusLabel_->setText(
        QString("Integration Logs: %1 entries shown").arg(filteredLogs_.size()));
}
