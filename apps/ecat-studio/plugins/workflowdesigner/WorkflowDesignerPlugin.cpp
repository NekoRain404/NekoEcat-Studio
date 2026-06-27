#include "WorkflowDesignerPlugin.h"

#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>
#include <QTreeWidget>
#include <QVBoxLayout>

WorkflowDesignerPlugin::WorkflowDesignerPlugin(QObject *parent) {
  if (parent) setParent(parent);
  buildUi();
}

QString WorkflowDesignerPlugin::id() const { return "workflowdesigner"; }
QString WorkflowDesignerPlugin::displayName() const { return "Workflow Designer"; }
QString WorkflowDesignerPlugin::displayNameZh() const { return QStringLiteral("工作流设计器"); }
QIcon WorkflowDesignerPlugin::icon() const { return QIcon::fromTheme("system-run"); }
int WorkflowDesignerPlugin::defaultOrder() const { return 250; }
bool WorkflowDesignerPlugin::visible() const { return true; }

void WorkflowDesignerPlugin::activate() {}
void WorkflowDesignerPlugin::deactivate() {}

QWidget *WorkflowDesignerPlugin::widget() { return containerWidget_; }
QWidget *WorkflowDesignerPlugin::canvas() const { return canvas_; }
QTreeWidget *WorkflowDesignerPlugin::nodePalette() const { return nodePalette_; }
QTextEdit *WorkflowDesignerPlugin::propertyEditor() const { return propertyEditor_; }
QTableWidget *WorkflowDesignerPlugin::executionMonitor() const { return executionMonitor_; }

void WorkflowDesignerPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QHBoxLayout(containerWidget_);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  auto *splitter = new QSplitter;

  auto *leftPanel = new QWidget;
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(4, 4, 4, 4);

  auto *paletteLabel = new QLabel(tr("Node Palette"));
  leftLayout->addWidget(paletteLabel);

  nodePalette_ = new QTreeWidget;
  nodePalette_->setHeaderLabel(tr("Nodes"));

  auto *ethercatCategory = new QTreeWidgetItem(nodePalette_, {tr("EtherCAT Nodes")});
  new QTreeWidgetItem(ethercatCategory, {tr("Master Node")});
  new QTreeWidgetItem(ethercatCategory, {tr("Slave Node")});
  new QTreeWidgetItem(ethercatCategory, {tr("Bus Monitor")});

  auto *controlCategory = new QTreeWidgetItem(nodePalette_, {tr("Control Flow")});
  new QTreeWidgetItem(controlCategory, {tr("Start")});
  new QTreeWidgetItem(controlCategory, {tr("End")});
  new QTreeWidgetItem(controlCategory, {tr("Condition")});
  new QTreeWidgetItem(controlCategory, {tr("Loop")});

  auto *actionCategory = new QTreeWidgetItem(nodePalette_, {tr("Actions")});
  new QTreeWidgetItem(actionCategory, {tr("SDO Read")});
  new QTreeWidgetItem(actionCategory, {tr("SDO Write")});
  new QTreeWidgetItem(actionCategory, {tr("PDO Exchange")});
  new QTreeWidgetItem(actionCategory, {tr("Delay")});
  new QTreeWidgetItem(actionCategory, {tr("Log Message")});

  nodePalette_->expandAll();
  leftLayout->addWidget(nodePalette_);

  auto *addRow = new QHBoxLayout;
  nodeNameInput_ = new QLineEdit;
  nodeNameInput_->setPlaceholderText(tr("Node name..."));
  addRow->addWidget(nodeNameInput_);
  addNodeButton_ = new QPushButton(tr("Add"));
  addRow->addWidget(addNodeButton_);
  leftLayout->addLayout(addRow);

  removeNodeButton_ = new QPushButton(tr("Remove Selected"));
  leftLayout->addWidget(removeNodeButton_);

  splitter->addWidget(leftPanel);

  canvas_ = new QWidget;
  canvas_->setMinimumSize(400, 300);
  canvas_->setStyleSheet("background-color: white; border: 1px solid #ccc;");
  splitter->addWidget(canvas_);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);
  rightLayout->setContentsMargins(4, 4, 4, 4);

  auto *propLabel = new QLabel(tr("Properties"));
  rightLayout->addWidget(propLabel);

  propertyEditor_ = new QTextEdit;
  propertyEditor_->setPlaceholderText(tr("Select a node to edit properties..."));
  rightLayout->addWidget(propertyEditor_);

  auto *monitorLabel = new QLabel(tr("Execution Monitor"));
  rightLayout->addWidget(monitorLabel);

  executionMonitor_ = new QTableWidget(0, 3);
  executionMonitor_->setHorizontalHeaderLabels({tr("Node"), tr("Status"), tr("Duration")});
  executionMonitor_->horizontalHeader()->setStretchLastSection(true);
  rightLayout->addWidget(executionMonitor_);

  statusLabel_ = new QLabel(tr("Status: Idle"));
  rightLayout->addWidget(statusLabel_);

  auto *buttonRow = new QHBoxLayout;
  exportButton_ = new QPushButton(tr("Export"));
  buttonRow->addWidget(exportButton_);
  importButton_ = new QPushButton(tr("Import"));
  buttonRow->addWidget(importButton_);
  clearButton_ = new QPushButton(tr("Clear All"));
  buttonRow->addWidget(clearButton_);
  rightLayout->addLayout(buttonRow);

  splitter->addWidget(rightPanel);
  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 3);
  splitter->setStretchFactor(2, 1);

  mainLayout->addWidget(splitter);

  connect(addNodeButton_, &QPushButton::clicked, this, [this]() {
    QString name = nodeNameInput_->text().trimmed();
    if (name.isEmpty()) return;
    addNode("Custom", name);
    nodeNameInput_->clear();
  });

  connect(removeNodeButton_, &QPushButton::clicked, this, [this]() {
    if (!nodes_.isEmpty()) {
      removeNode(nodes_.last().id);
    }
  });

  connect(clearButton_, &QPushButton::clicked, this, &WorkflowDesignerPlugin::clearNodes);
  connect(exportButton_, &QPushButton::clicked, this, &WorkflowDesignerPlugin::exportRequested);
  connect(importButton_, &QPushButton::clicked, this, [this]() {
    QString path = QFileDialog::getOpenFileName(containerWidget_, tr("Import Workflow"), QString(), "JSON (*.json)");
    if (!path.isEmpty()) importWorkflow(path);
  });
}

void WorkflowDesignerPlugin::addNode(const QString &category, const QString &name) {
  WorkflowNode node;
  node.id = QString("node_%1").arg(nextNodeId_++);
  node.name = name;
  node.category = category;
  node.description = "";
  node.x = 0;
  node.y = 0;
  nodes_.append(node);

  int row = executionMonitor_->rowCount();
  executionMonitor_->insertRow(row);
  executionMonitor_->setItem(row, 0, new QTableWidgetItem(name));
  executionMonitor_->setItem(row, 1, new QTableWidgetItem(tr("Ready")));
  executionMonitor_->setItem(row, 2, new QTableWidgetItem("-"));

  emit nodeAdded(node.id, name);
}

void WorkflowDesignerPlugin::removeNode(const QString &nodeId) {
  for (int i = 0; i < nodes_.size(); ++i) {
    if (nodes_[i].id == nodeId) {
      nodes_.removeAt(i);
      executionMonitor_->removeRow(i);
      emit nodeRemoved(nodeId);
      return;
    }
  }
}

void WorkflowDesignerPlugin::clearNodes() {
  nodes_.clear();
  connections_.clear();
  executionMonitor_->setRowCount(0);
  nextNodeId_ = 1;
}

int WorkflowDesignerPlugin::nodeCount() const { return nodes_.size(); }

void WorkflowDesignerPlugin::addConnection(const QString &fromId, const QString &toId, const QString &label) {
  WorkflowConnection conn;
  conn.fromNodeId = fromId;
  conn.toNodeId = toId;
  conn.label = label;
  connections_.append(conn);
  emit connectionAdded(fromId, toId);
}

void WorkflowDesignerPlugin::removeConnection(int index) {
  if (index >= 0 && index < connections_.size()) {
    connections_.removeAt(index);
    emit connectionRemoved(index);
  }
}

int WorkflowDesignerPlugin::connectionCount() const { return connections_.size(); }

void WorkflowDesignerPlugin::updateNodeStatus(const QString &nodeId, const QString &status) {
  for (int i = 0; i < nodes_.size(); ++i) {
    if (nodes_[i].id == nodeId) {
      if (i < executionMonitor_->rowCount()) {
        executionMonitor_->item(i, 1)->setText(status);
      }
      return;
    }
  }
}

void WorkflowDesignerPlugin::setExecutionStatus(const QString &status) {
  executionStatus_ = status;
  statusLabel_->setText(tr("Status: %1").arg(status));
  emit executionStatusChanged(status);
}

QString WorkflowDesignerPlugin::executionStatus() const { return executionStatus_; }

bool WorkflowDesignerPlugin::exportWorkflow(const QString &filePath) {
  if (filePath.isEmpty()) return false;

  QJsonObject root;
  root["version"] = 1;
  root["executionStatus"] = executionStatus_;

  QJsonArray nodesArray;
  for (const auto &node : nodes_) {
    QJsonObject nodeObj;
    nodeObj["id"] = node.id;
    nodeObj["name"] = node.name;
    nodeObj["category"] = node.category;
    nodeObj["description"] = node.description;
    nodeObj["x"] = node.x;
    nodeObj["y"] = node.y;
    nodesArray.append(nodeObj);
  }
  root["nodes"] = nodesArray;

  QJsonArray connectionsArray;
  for (const auto &conn : connections_) {
    QJsonObject connObj;
    connObj["from"] = conn.fromNodeId;
    connObj["to"] = conn.toNodeId;
    connObj["label"] = conn.label;
    connectionsArray.append(connObj);
  }
  root["connections"] = connectionsArray;

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly)) return false;
  const QByteArray bytes = QJsonDocument(root).toJson();
  if (file.write(bytes) != bytes.size() || !file.flush()) return false;
  return true;
}

bool WorkflowDesignerPlugin::importWorkflow(const QString &filePath) {
  if (filePath.isEmpty()) return false;

  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly)) return false;
  QJsonParseError parseError;
  QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
  if (parseError.error != QJsonParseError::NoError || !doc.isObject()) return false;

  QJsonObject root = doc.object();
  if (!root.value("nodes").isArray() || !root.value("connections").isArray())
    return false;

  QVector<WorkflowNode> importedNodes;
  QVector<WorkflowConnection> importedConnections;

  for (const auto &n : root["nodes"].toArray()) {
    if (!n.isObject()) return false;
    QJsonObject nodeObj = n.toObject();
    WorkflowNode node;
    node.id = nodeObj["id"].toString();
    node.name = nodeObj["name"].toString();
    node.category = nodeObj["category"].toString();
    node.description = nodeObj["description"].toString();
    node.x = nodeObj["x"].toInt();
    node.y = nodeObj["y"].toInt();
    if (node.id.isEmpty() || node.name.isEmpty()) return false;
    importedNodes.append(node);
  }

  for (const auto &c : root["connections"].toArray()) {
    if (!c.isObject()) return false;
    QJsonObject connObj = c.toObject();
    WorkflowConnection conn;
    conn.fromNodeId = connObj["from"].toString();
    conn.toNodeId = connObj["to"].toString();
    conn.label = connObj["label"].toString();
    if (conn.fromNodeId.isEmpty() || conn.toNodeId.isEmpty()) return false;
    importedConnections.append(conn);
  }

  const QString importedExecutionStatus =
      root.contains("executionStatus") ? root["executionStatus"].toString() : QString();

  clearNodes();

  if (root.contains("executionStatus"))
    setExecutionStatus(importedExecutionStatus);

  for (const auto &node : importedNodes) {
    nodes_.append(node);
    int row = executionMonitor_->rowCount();
    executionMonitor_->insertRow(row);
    executionMonitor_->setItem(row, 0, new QTableWidgetItem(node.name));
    executionMonitor_->setItem(row, 1, new QTableWidgetItem(tr("Ready")));
    executionMonitor_->setItem(row, 2, new QTableWidgetItem("-"));
  }

  connections_ = importedConnections;

  return true;
}
