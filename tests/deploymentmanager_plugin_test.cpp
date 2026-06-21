// DeploymentManagerPluginTest — Tests for DeploymentManagerPlugin
//
// Test coverage:
//   - Plugin identity and ordering
//   - Target/package/history table structure
//   - Add/remove targets and packages
//   - Deploy and rollback operations
//   - Status log and label widgets
//   - Export deployment log

#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTextEdit>
#include <QLabel>
#include "plugins/deploymentmanager/DeploymentManagerPlugin.h"

class DeploymentManagerPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, display names, order, and visibility
  void testPluginIdentity() {
    DeploymentManagerPlugin plugin;
    QCOMPARE(plugin.id(), QString("deploymentmanager"));
    QCOMPARE(plugin.displayName(), QString("Deployment Manager"));
    QCOMPARE(plugin.displayNameZh(), QString("部署管理器"));
    QCOMPARE(plugin.defaultOrder(), 310);
    QCOMPARE(plugin.visible(), true);
  }

  // Verify main widget is created
  void testWidgetCreation() {
    DeploymentManagerPlugin plugin;
    QVERIFY(plugin.widget() != nullptr);
  }

  // Verify initial counts are zero
  void testInitialState() {
    DeploymentManagerPlugin plugin;
    QCOMPARE(plugin.targetCount(), 0);
    QCOMPARE(plugin.packageCount(), 0);
    QCOMPARE(plugin.historyCount(), 0);
  }

  // Verify target table has correct column count
  void testTargetTable() {
    DeploymentManagerPlugin plugin;
    QTableWidget *table = plugin.targetTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->columnCount(), 4);
  }

  // Verify package table has correct column count
  void testPackageTable() {
    DeploymentManagerPlugin plugin;
    QTableWidget *table = plugin.packageTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->columnCount(), 4);
  }

  // Verify history table has correct column count
  void testHistoryTable() {
    DeploymentManagerPlugin plugin;
    QTableWidget *table = plugin.historyTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->columnCount(), 5);
  }

  // Verify status log is read-only
  void testStatusLog() {
    DeploymentManagerPlugin plugin;
    QTextEdit *log = plugin.statusLog();
    QVERIFY(log != nullptr);
    QVERIFY(log->isReadOnly());
  }

  // Verify status label widget exists
  void testStatusLabel() {
    DeploymentManagerPlugin plugin;
    QLabel *label = plugin.statusLabel();
    QVERIFY(label != nullptr);
  }

  // Verify adding a target increments count and emits signal
  void testAddTarget() {
    DeploymentManagerPlugin plugin;
    QSignalSpy spy(&plugin, &DeploymentManagerPlugin::targetAdded);

    DeploymentMgrTarget t;
    t.name = "TestTarget";
    t.address = "192.168.1.100";
    t.config = "default";
    t.status = "Online";
    plugin.addTarget(t);

    QCOMPARE(plugin.targetCount(), 1);
    QCOMPARE(plugin.targetTable()->rowCount(), 1);
    QCOMPARE(spy.count(), 1);
  }

  // Verify removing a target decrements count and emits signal
  void testRemoveTarget() {
    DeploymentManagerPlugin plugin;
    QSignalSpy spy(&plugin, &DeploymentManagerPlugin::targetRemoved);

    DeploymentMgrTarget t;
    t.name = "RemoveTarget";
    t.address = "10.0.0.1";
    t.config = "test";
    t.status = "Online";
    plugin.addTarget(t);
    QCOMPARE(plugin.targetCount(), 1);

    plugin.removeTarget(0);
    QCOMPARE(plugin.targetCount(), 0);
    QCOMPARE(spy.count(), 1);
  }

  // Verify target status update emits signal with new status
  void testUpdateTargetStatus() {
    DeploymentManagerPlugin plugin;
    QSignalSpy spy(&plugin, &DeploymentManagerPlugin::targetStatusChanged);

    DeploymentMgrTarget t;
    t.name = "StatusTarget";
    t.address = "10.0.0.2";
    t.config = "test";
    t.status = "Online";
    plugin.addTarget(t);

    plugin.updateTargetStatus(0, "Offline");
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(1).toString(), QString("Offline"));
  }

  // Verify adding a package increments count and emits signal
  void testAddPackage() {
    DeploymentManagerPlugin plugin;
    QSignalSpy spy(&plugin, &DeploymentManagerPlugin::packageAdded);

    DeploymentMgrPackage p;
    p.name = "firmware";
    p.version = "1.0.0";
    p.description = "Initial release";
    plugin.addPackage(p);

    QCOMPARE(plugin.packageCount(), 1);
    QCOMPARE(plugin.packageTable()->rowCount(), 1);
    QCOMPARE(spy.count(), 1);
  }

  // Verify removing a package decrements count and emits signal
  void testRemovePackage() {
    DeploymentManagerPlugin plugin;
    QSignalSpy spy(&plugin, &DeploymentManagerPlugin::packageRemoved);

    DeploymentMgrPackage p;
    p.name = "pkg";
    p.version = "1.0.0";
    plugin.addPackage(p);
    QCOMPARE(plugin.packageCount(), 1);

    plugin.removePackage(0);
    QCOMPARE(plugin.packageCount(), 0);
    QCOMPARE(spy.count(), 1);
  }

  // Verify deploy emits start/finish signals and adds history record
  void testDeploy() {
    DeploymentManagerPlugin plugin;
    DeploymentMgrTarget t;
    t.name = "Target1";
    t.address = "192.168.1.50";
    t.config = "prod";
    t.status = "Online";
    plugin.addTarget(t);

    DeploymentMgrPackage p;
    p.name = "ecat-fw";
    p.version = "2.0.0";
    plugin.addPackage(p);

    QSignalSpy startSpy(&plugin, &DeploymentManagerPlugin::deploymentStarted);
    QSignalSpy finishSpy(&plugin, &DeploymentManagerPlugin::deploymentFinished);
    plugin.deploy(0, 0);

    QCOMPARE(startSpy.count(), 1);
    QCOMPARE(finishSpy.count(), 1);
    QCOMPARE(finishSpy.at(0).at(1).toString(), QString("Success"));
    QCOMPARE(plugin.historyCount(), 1);
    QCOMPARE(plugin.historyTable()->rowCount(), 1);
  }

  // Verify rollback emits signal and adds "Rolled Back" history entry
  void testRollback() {
    DeploymentManagerPlugin plugin;
    DeploymentMgrTarget t;
    t.name = "RbTarget";
    t.address = "10.0.0.5";
    t.config = "test";
    t.status = "Online";
    plugin.addTarget(t);

    DeploymentMgrPackage p;
    p.name = "rb-pkg";
    p.version = "1.0.0";
    plugin.addPackage(p);
    plugin.deploy(0, 0);

    QSignalSpy spy(&plugin, &DeploymentManagerPlugin::rollbackRequested);
    plugin.rollback(0);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(plugin.historyCount(), 2);
    QCOMPARE(plugin.historyTable()->item(1, 3)->text(), QString("Rolled Back"));
  }

  // Verify clear history removes all records and table rows
  void testClearHistory() {
    DeploymentManagerPlugin plugin;
    DeploymentMgrTarget t;
    t.name = "ClearTarget";
    t.address = "10.0.0.6";
    t.config = "test";
    t.status = "Online";
    plugin.addTarget(t);

    DeploymentMgrPackage p;
    p.name = "clear-pkg";
    p.version = "1.0.0";
    plugin.addPackage(p);
    plugin.deploy(0, 0);
    QCOMPARE(plugin.historyCount(), 1);

    plugin.clearHistory();
    QCOMPARE(plugin.historyCount(), 0);
    QCOMPARE(plugin.historyTable()->rowCount(), 0);
  }

  // Verify export creates a log file
  void testExportLog() {
    DeploymentManagerPlugin plugin;
    DeploymentMgrTarget t;
    t.name = "ExpTarget";
    t.address = "10.0.0.7";
    t.config = "test";
    t.status = "Online";
    plugin.addTarget(t);

    DeploymentMgrPackage p;
    p.name = "exp-pkg";
    p.version = "1.0.0";
    plugin.addPackage(p);
    plugin.deploy(0, 0);

    QString path = QDir::temp().absoluteFilePath("deploymentmanager_log_test.json");
    QVERIFY(plugin.exportLog(path));
    QVERIFY(QFile::exists(path));
    QFile::remove(path);
  }
};

QTEST_MAIN(DeploymentManagerPluginTest)
#include "deploymentmanager_plugin_test.moc"
