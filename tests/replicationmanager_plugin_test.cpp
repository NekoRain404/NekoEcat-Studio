// ReplicationManagerPluginTest — Tests for Replication Manager Plugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Initial state validation
//   - Target/status/history/settings table structure
//   - Add/remove/update operations for targets, status, history, settings
//   - Status label and report export
#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QLabel>
#include "plugins/replicationmanager/ReplicationManagerPlugin.h"

class ReplicationManagerPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, display names, order, and visibility
  void testPluginIdentity() {
    ReplicationManagerPlugin plugin;

    QCOMPARE(plugin.id(), QString("replicationmanager"));
    QCOMPARE(plugin.displayName(), QString("Replication Manager"));
    QCOMPARE(plugin.displayNameZh(), QString("复制管理器"));
    QCOMPARE(plugin.defaultOrder(), 335);
    QCOMPARE(plugin.visible(), true);
  }

  // Widget should be created successfully
  void testWidgetCreation() {
    ReplicationManagerPlugin plugin;
    QVERIFY(plugin.widget() != nullptr);
  }

  // Default counts for targets, statuses, histories, and settings
  void testInitialState() {
    ReplicationManagerPlugin plugin;

    QCOMPARE(plugin.targetCount(), 3);
    QCOMPARE(plugin.statusCount(), 3);
    QCOMPARE(plugin.historyCount(), 3);
    QCOMPARE(plugin.settingCount(), 4);
  }

  // Target table has correct dimensions
  void testTargetTable() {
    ReplicationManagerPlugin plugin;

    QTableWidget *table = plugin.targetTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 3);
    QCOMPARE(table->columnCount(), 6);
  }

  // Status table has correct dimensions
  void testStatusTable() {
    ReplicationManagerPlugin plugin;

    QTableWidget *table = plugin.statusTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 3);
    QCOMPARE(table->columnCount(), 6);
  }

  // History table has correct dimensions
  void testHistoryTable() {
    ReplicationManagerPlugin plugin;

    QTableWidget *table = plugin.historyTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 3);
    QCOMPARE(table->columnCount(), 6);
  }

  // Settings table has correct dimensions
  void testSettingsTable() {
    ReplicationManagerPlugin plugin;

    QTableWidget *table = plugin.settingsTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 4);
    QCOMPARE(table->columnCount(), 5);
  }

  // Add a new replication target and verify signal emission
  void testAddTarget() {
    ReplicationManagerPlugin plugin;
    QSignalSpy spy(&plugin, &ReplicationManagerPlugin::targetAdded);
    int initial = plugin.targetCount();

    ReplicationManagerPlugin::ReplicationTarget t;
    t.id = "t_new";
    t.name = "New Target";
    t.endpoint = "192.168.1.250:5877";
    t.type = "Full";
    t.enabled = true;
    t.lastReplicated = QDateTime::currentDateTime();

    plugin.addTarget(t);
    QCOMPARE(plugin.targetCount(), initial + 1);
    QCOMPARE(spy.count(), 1);
  }

  // Remove a target and verify signal emission
  void testRemoveTarget() {
    ReplicationManagerPlugin plugin;
    QSignalSpy spy(&plugin, &ReplicationManagerPlugin::targetRemoved);
    int initial = plugin.targetCount();

    plugin.removeTarget(0);
    QCOMPARE(plugin.targetCount(), initial - 1);
    QCOMPARE(spy.count(), 1);
  }

  // Update replication status entry
  void testUpdateStatus() {
    ReplicationManagerPlugin plugin;

    ReplicationManagerPlugin::ReplicationStatus s;
    s.targetId = "t1";
    s.targetName = "Updated";
    s.state = "Complete";
    s.progress = 100;
    s.lastUpdate = QDateTime::currentDateTime();
    s.message = "Done";

    plugin.updateStatus(s);
    QCOMPARE(plugin.statusCount(), 3);
    QCOMPARE(plugin.statusTable()->item(0, 2)->text(), QString("Complete"));
  }

  // Add a history entry to the log
  void testAddHistoryEntry() {
    ReplicationManagerPlugin plugin;
    int initial = plugin.historyCount();

    ReplicationManagerPlugin::ReplicationHistoryEntry h;
    h.timestamp = QDateTime::currentDateTime();
    h.targetId = "t1";
    h.targetName = "Test";
    h.result = "Success";
    h.objectsReplicated = 100;
    h.details = "OK";

    plugin.addHistoryEntry(h);
    QCOMPARE(plugin.historyCount(), initial + 1);
  }

  // Add a new replication setting
  void testAddSetting() {
    ReplicationManagerPlugin plugin;
    int initial = plugin.settingCount();

    ReplicationManagerPlugin::ReplicationSetting s;
    s.id = "rs_new";
    s.name = "New Setting";
    s.description = "Test";
    s.value = "default";
    s.defaultValue = "default";

    plugin.addSetting(s);
    QCOMPARE(plugin.settingCount(), initial + 1);
  }

  // Update an existing setting value
  void testUpdateSetting() {
    ReplicationManagerPlugin plugin;

    plugin.updateSetting(0, "full");
    QCOMPARE(plugin.settingsTable()->item(0, 3)->text(), QString("full"));
  }

  // Status label is created and accessible
  void testStatusLabel() {
    ReplicationManagerPlugin plugin;

    QLabel *label = plugin.statusLabel();
    QVERIFY(label != nullptr);
  }

  // Export report to file and verify file creation
  void testExportReport() {
    ReplicationManagerPlugin plugin;

    QString path = QDir::temp().absoluteFilePath("replication_report_test.txt");
    plugin.exportReport(path);
    QVERIFY(QFile::exists(path));
    QFile::remove(path);
  }
};

QTEST_MAIN(ReplicationManagerPluginTest)
#include "replicationmanager_plugin_test.moc"
