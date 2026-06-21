#include "DigitalTwinStudioPlugin.h"

#include <QDateTime>
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
#include <QVBoxLayout>

DigitalTwinStudioPlugin::DigitalTwinStudioPlugin(QObject *parent) {
  if (parent) setParent(parent);
  buildUi();
}

QString DigitalTwinStudioPlugin::id() const { return "digitaltwin"; }
QString DigitalTwinStudioPlugin::displayName() const { return "Digital Twin Studio"; }
QString DigitalTwinStudioPlugin::displayNameZh() const { return QStringLiteral("数字孪生工作室"); }
QIcon DigitalTwinStudioPlugin::icon() const { return QIcon::fromTheme("view-grid"); }
int DigitalTwinStudioPlugin::defaultOrder() const { return 320; }
bool DigitalTwinStudioPlugin::visible() const { return true; }

void DigitalTwinStudioPlugin::activate() {}
void DigitalTwinStudioPlugin::deactivate() {}

QWidget *DigitalTwinStudioPlugin::widget() { return containerWidget_; }
QTableWidget *DigitalTwinStudioPlugin::nodeTable() const { return nodeTable_; }
QTableWidget *DigitalTwinStudioPlugin::connectionTable() const { return connectionTable_; }
QTextEdit *DigitalTwinStudioPlugin::snapshotView() const { return snapshotView_; }

void DigitalTwinStudioPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QHBoxLayout(containerWidget_);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  auto *splitter = new QSplitter;

  auto *leftPanel = new QWidget;
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(4, 4, 4, 4);

  auto *nodeLabel = new QLabel(tr("Twin Nodes"));
  leftLayout->addWidget(nodeLabel);

  nodeTable_ = new QTableWidget;
  nodeTable_->setColumnCount(3);
  nodeTable_->setHorizontalHeaderLabels({tr("Name"), tr("Type"), tr("Status")});
  nodeTable_->horizontalHeader()->setStretchLastSection(true);
  nodeTable_->setSelectionBehavior(QTableWidget::SelectRows);
  nodeTable_->setSelectionMode(QTableWidget::SingleSelection);
  leftLayout->addWidget(nodeTable_);

  auto *nodeBtnRow = new QHBoxLayout;
  addNodeBtn_ = new QPushButton(tr("Add"));
  nodeBtnRow->addWidget(addNodeBtn_);
  removeNodeBtn_ = new QPushButton(tr("Remove"));
  nodeBtnRow->addWidget(removeNodeBtn_);
  nodeBtnRow->addStretch();
  leftLayout->addLayout(nodeBtnRow);

  auto *connLabel = new QLabel(tr("Connections"));
  leftLayout->addWidget(connLabel);

  connectionTable_ = new QTableWidget;
  connectionTable_->setColumnCount(2);
  connectionTable_->setHorizontalHeaderLabels({tr("Source"), tr("Target")});
  connectionTable_->horizontalHeader()->setStretchLastSection(true);
  leftLayout->addWidget(connectionTable_);

  auto *connBtnRow = new QHBoxLayout;
  addConnBtn_ = new QPushButton(tr("Add Connection"));
  connBtnRow->addWidget(addConnBtn_);
  connBtnRow->addStretch();
  leftLayout->addLayout(connBtnRow);

  splitter->addWidget(leftPanel);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);
  rightLayout->setContentsMargins(4, 4, 4, 4);

  auto *snapLabel = new QLabel(tr("Snapshots"));
  rightLayout->addWidget(snapLabel);

  snapshotView_ = new QTextEdit;
  snapshotView_->setReadOnly(true);
  snapshotView_->setPlaceholderText(tr("Digital twin snapshots..."));
  rightLayout->addWidget(snapshotView_, 1);

  auto *btnRow = new QHBoxLayout;
  snapshotBtn_ = new QPushButton(tr("Take Snapshot"));
  btnRow->addWidget(snapshotBtn_);
  exportBtn_ = new QPushButton(tr("Export Report"));
  btnRow->addWidget(exportBtn_);
  btnRow->addStretch();
  rightLayout->addLayout(btnRow);

  statusLabel_ = new QLabel(tr("Ready"));
  rightLayout->addWidget(statusLabel_);

  splitter->addWidget(rightPanel);
  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 1);

  mainLayout->addWidget(splitter);

  connect(addNodeBtn_, &QPushButton::clicked, this, [this]() {
    addNode(tr("New Node"), tr("sensor"));
  });
  connect(removeNodeBtn_, &QPushButton::clicked, this, [this]() {
    auto *item = nodeTable_->currentItem();
    if (item) removeNode(nodeTable_->item(item->row(), 0)->text());
  });
  connect(addConnBtn_, &QPushButton::clicked, this, [this]() {
    addConnectionEntry(tr("NodeA"), tr("NodeB"));
  });
  connect(snapshotBtn_, &QPushButton::clicked, this, &DigitalTwinStudioPlugin::takeSnapshot);
  connect(exportBtn_, &QPushButton::clicked, this, &DigitalTwinStudioPlugin::snapshotTaken);
}

void DigitalTwinStudioPlugin::addNode(const QString &name, const QString &type) {
  int row = nodeTable_->rowCount();
  nodeTable_->insertRow(row);
  nodeTable_->setItem(row, 0, new QTableWidgetItem(name));
  nodeTable_->setItem(row, 1, new QTableWidgetItem(type));
  nodeTable_->setItem(row, 2, new QTableWidgetItem(tr("Active")));
  statusLabel_->setText(tr("Added node: %1").arg(name));
  emit nodeAdded(name);
}

void DigitalTwinStudioPlugin::removeNode(const QString &name) {
  for (int i = 0; i < nodeTable_->rowCount(); ++i) {
    if (nodeTable_->item(i, 0)->text() == name) {
      nodeTable_->removeRow(i);
      emit nodeRemovedSignal(name);
      return;
    }
  }
}

int DigitalTwinStudioPlugin::nodeCount() const { return nodeTable_->rowCount(); }
void DigitalTwinStudioPlugin::clearNodes() { nodeTable_->setRowCount(0); }

void DigitalTwinStudioPlugin::addConnectionEntry(const QString &source, const QString &target) {
  int row = connectionTable_->rowCount();
  connectionTable_->insertRow(row);
  connectionTable_->setItem(row, 0, new QTableWidgetItem(source));
  connectionTable_->setItem(row, 1, new QTableWidgetItem(target));
  statusLabel_->setText(tr("Connected: %1 -> %2").arg(source, target));
  emit connectionEntryAdded(source, target);
}

void DigitalTwinStudioPlugin::clearConnections() { connectionTable_->setRowCount(0); }
int DigitalTwinStudioPlugin::connectionEntryCount() const { return connectionTable_->rowCount(); }

void DigitalTwinStudioPlugin::takeSnapshot() {
  snapshotCount_++;
  QString text = tr("Snapshot #%1 at %2\nNodes: %3, Connections: %4")
      .arg(snapshotCount_)
      .arg(QDateTime::currentDateTime().toString(Qt::ISODate))
      .arg(nodeCount())
      .arg(connectionEntryCount());
  snapshotView_->append(text);
  emit snapshotTaken();
}

int DigitalTwinStudioPlugin::snapshotCount() const { return snapshotCount_; }
void DigitalTwinStudioPlugin::clearSnapshots() { snapshotCount_ = 0; snapshotView_->clear(); }

bool DigitalTwinStudioPlugin::exportReport(const QString &filePath, const QString &format) {
  QJsonObject root;
  root["version"] = 1;
  root["format"] = format;
  root["snapshots"] = snapshotView_->toPlainText();

  QJsonArray nodes;
  for (int i = 0; i < nodeTable_->rowCount(); ++i) {
    QJsonObject n;
    n["name"] = nodeTable_->item(i, 0)->text();
    n["type"] = nodeTable_->item(i, 1)->text();
    n["status"] = nodeTable_->item(i, 2)->text();
    nodes.append(n);
  }
  root["nodes"] = nodes;

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly)) return false;
  file.write(QJsonDocument(root).toJson());
  return true;
}
