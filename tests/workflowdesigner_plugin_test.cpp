// TestWorkflowDesignerPlugin — Tests for Workflow Designer Plugin (full UI)
//
// Test coverage:
//   - Plugin identity (id, display names, order, visibility)
//   - Widget and canvas creation
//   - Node palette and property editor
//   - Execution monitor
//   - Add, remove, and clear nodes
//   - Node connections and status
//   - Execution status tracking
//   - Export and import workflows
//   - Signal emissions

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QTextEdit>
#include <QTreeWidget>
#include <QtTest/QtTest>

#include "plugins/workflowdesigner/WorkflowDesignerPlugin.h"

class TestWorkflowDesignerPlugin : public QObject {
  Q_OBJECT
private slots:
  void initTestCase();
  void cleanupTestCase();
  // Plugin reports correct id, display names, order, and visibility
  // Plugin reports correct identity fields
  void identity();
  // Widget is not null after creation
  // Widget is created and non-null
  void widgetNotNull();
  // Canvas widget exists and has minimum width
  // Canvas widget meets minimum width requirement
  void canvas();
  // Node palette has categories and items
  // Node palette has available node types
  void nodePalette();
  // Property editor widget exists
  // Property editor widget is created
  void propertyEditor();
  // Execution monitor has correct column count
  // Execution monitor table has 3 columns
  void executionMonitor();
  // Add and remove nodes and verify count
  // Nodes can be added and removed by ID
  void addAndRemoveNodes();
  // Clear all nodes resets count to zero
  // clearNodes removes all nodes, connections, and monitor rows
  void clearNodes();
  // Add and remove connections between nodes
  // Connections can be added and removed between nodes
  void connections();
  // Update and verify node status
  // Node status updates reflected in execution monitor
  void nodeStatus();
  // Track execution status across nodes
  // Execution status can be set and retrieved
  void executionStatus();
  // Export and import workflow definitions
  // Export and import workflow preserves nodes and connections
  void exportImport();
  // Malformed workflow imports fail without clearing current state
  void rejectInvalidImport();
  // Verify signal emissions on state changes
  // Signals emitted on node add/remove, connection, and status change
  void signalEmissions();

private:
  WorkflowDesignerPlugin *plugin_ = nullptr;
};

void TestWorkflowDesignerPlugin::initTestCase() {
  plugin_ = new WorkflowDesignerPlugin(this);
}

void TestWorkflowDesignerPlugin::cleanupTestCase() {
  delete plugin_;
  plugin_ = nullptr;
}

void TestWorkflowDesignerPlugin::identity() {
  QCOMPARE(plugin_->id(), QString("workflowdesigner"));
  QCOMPARE(plugin_->displayName(), QString("Workflow Designer"));
  QCOMPARE(plugin_->displayNameZh(), QString("工作流设计器"));
  QCOMPARE(plugin_->defaultOrder(), 250);
  QVERIFY(plugin_->visible());
}

void TestWorkflowDesignerPlugin::widgetNotNull() {
  QVERIFY(plugin_->widget() != nullptr);
}

void TestWorkflowDesignerPlugin::canvas() {
  QVERIFY(plugin_->canvas() != nullptr);
  QVERIFY(plugin_->canvas()->minimumWidth() >= 400);
}

void TestWorkflowDesignerPlugin::nodePalette() {
  QVERIFY(plugin_->nodePalette() != nullptr);
  QVERIFY(plugin_->nodePalette()->topLevelItemCount() > 0);
}

void TestWorkflowDesignerPlugin::propertyEditor() {
  QVERIFY(plugin_->propertyEditor() != nullptr);
}

void TestWorkflowDesignerPlugin::executionMonitor() {
  QVERIFY(plugin_->executionMonitor() != nullptr);
  QCOMPARE(plugin_->executionMonitor()->columnCount(), 3);
}

void TestWorkflowDesignerPlugin::addAndRemoveNodes() {
  plugin_->clearNodes();
  QCOMPARE(plugin_->nodeCount(), 0);

  plugin_->addNode("EtherCAT", "Master Node");
  QCOMPARE(plugin_->nodeCount(), 1);

  plugin_->addNode("Actions", "SDO Read");
  QCOMPARE(plugin_->nodeCount(), 2);

  plugin_->addNode("Control", "Start");
  QCOMPARE(plugin_->nodeCount(), 3);

  QCOMPARE(plugin_->executionMonitor()->rowCount(), 3);

  plugin_->removeNode("node_1");
  QCOMPARE(plugin_->nodeCount(), 2);

  plugin_->removeNode("nonexistent");
  QCOMPARE(plugin_->nodeCount(), 2);

  plugin_->clearNodes();
}

void TestWorkflowDesignerPlugin::clearNodes() {
  plugin_->addNode("Test", "A");
  plugin_->addNode("Test", "B");
  QCOMPARE(plugin_->nodeCount(), 2);

  plugin_->clearNodes();
  QCOMPARE(plugin_->nodeCount(), 0);
  QCOMPARE(plugin_->connectionCount(), 0);
  QCOMPARE(plugin_->executionMonitor()->rowCount(), 0);
}

void TestWorkflowDesignerPlugin::connections() {
  plugin_->clearNodes();
  plugin_->addNode("A", "NodeA");
  plugin_->addNode("B", "NodeB");

  plugin_->addConnection("node_1", "node_2", "data");
  QCOMPARE(plugin_->connectionCount(), 1);

  plugin_->addConnection("node_1", "node_2", "control");
  QCOMPARE(plugin_->connectionCount(), 2);

  plugin_->removeConnection(0);
  QCOMPARE(plugin_->connectionCount(), 1);

  plugin_->removeConnection(-1);
  QCOMPARE(plugin_->connectionCount(), 1);

  plugin_->clearNodes();
}

void TestWorkflowDesignerPlugin::nodeStatus() {
  plugin_->clearNodes();
  plugin_->addNode("Test", "StatusNode");

  plugin_->updateNodeStatus("node_1", "Running");
  QCOMPARE(plugin_->executionMonitor()->item(0, 1)->text(), QString("Running"));

  plugin_->updateNodeStatus("node_1", "Completed");
  QCOMPARE(plugin_->executionMonitor()->item(0, 1)->text(), QString("Completed"));

  plugin_->clearNodes();
}

void TestWorkflowDesignerPlugin::executionStatus() {
  QCOMPARE(plugin_->executionStatus(), QString());

  plugin_->setExecutionStatus("Running");
  QCOMPARE(plugin_->executionStatus(), QString("Running"));

  plugin_->setExecutionStatus("Idle");
  QCOMPARE(plugin_->executionStatus(), QString("Idle"));
}

void TestWorkflowDesignerPlugin::exportImport() {
  plugin_->clearNodes();
  plugin_->addNode("Test", "ExportNode");
  plugin_->addConnection("node_1", "node_1", "self");
  plugin_->setExecutionStatus("TestStatus");

  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  const QString tmpPath = dir.filePath(QStringLiteral("workflow_test_export.json"));
  QVERIFY(plugin_->exportWorkflow(tmpPath));

  plugin_->clearNodes();
  plugin_->setExecutionStatus("Idle");

  QVERIFY(plugin_->importWorkflow(tmpPath));
  QCOMPARE(plugin_->nodeCount(), 1);
  QCOMPARE(plugin_->connectionCount(), 1);
  QCOMPARE(plugin_->executionStatus(), QString("TestStatus"));

  plugin_->clearNodes();
}

void TestWorkflowDesignerPlugin::rejectInvalidImport() {
  plugin_->clearNodes();
  plugin_->addNode("Test", "ExistingNode");
  plugin_->addConnection("node_1", "node_1", "self");
  plugin_->setExecutionStatus("KeepStatus");

  QVERIFY(!plugin_->exportWorkflow(QString()));

  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  QVERIFY(!plugin_->exportWorkflow(dir.path()));

  const QString invalidPath = dir.filePath(QStringLiteral("invalid.json"));
  QFile invalidFile(invalidPath);
  QVERIFY(invalidFile.open(QIODevice::WriteOnly));
  QCOMPARE(invalidFile.write(QByteArrayLiteral("[]")), 2);
  invalidFile.close();

  QVERIFY(!plugin_->importWorkflow(QString()));
  QVERIFY(!plugin_->importWorkflow(invalidPath));
  QCOMPARE(plugin_->nodeCount(), 1);
  QCOMPARE(plugin_->connectionCount(), 1);
  QCOMPARE(plugin_->executionStatus(), QString("KeepStatus"));

  plugin_->clearNodes();
}

void TestWorkflowDesignerPlugin::signalEmissions() {
  QSignalSpy addSpy(plugin_, &WorkflowDesignerPlugin::nodeAdded);
  QSignalSpy removeSpy(plugin_, &WorkflowDesignerPlugin::nodeRemoved);
  QSignalSpy connSpy(plugin_, &WorkflowDesignerPlugin::connectionAdded);
  QSignalSpy statusSpy(plugin_, &WorkflowDesignerPlugin::executionStatusChanged);

  plugin_->addNode("Test", "SignalNode");
  QCOMPARE(addSpy.count(), 1);
  QCOMPARE(addSpy.at(0).at(1).toString(), QString("SignalNode"));

  plugin_->addConnection("node_1", "node_1");
  QCOMPARE(connSpy.count(), 1);

  plugin_->setExecutionStatus("Test");
  QCOMPARE(statusSpy.count(), 1);

  plugin_->removeNode("node_1");
  QCOMPARE(removeSpy.count(), 1);

  plugin_->clearNodes();
}

QTEST_MAIN(TestWorkflowDesignerPlugin)
#include "workflowdesigner_plugin_test.moc"
