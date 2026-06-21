// SyncManagerPluginTest — Tests for Sync Manager Plugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Initial state (status, history, settings, log counts)
//   - Status/history/settings/log table structure
//   - Add/remove operations for status, history, settings, logs
//   - Log filtering
//   - Status label and report export
#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QLabel>
#include "plugins/syncmanager/SyncManagerPlugin.h"

class SyncManagerPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin ID, display name, order, and visibility
  void testPluginIdentity() {
    SyncManagerPlugin plugin;

    QCOMPARE(plugin.id(), QString("syncmanager"));
    QCOMPARE(plugin.displayName(), QString("Sync Manager"));
    QCOMPARE(plugin.displayNameZh(), QString("同步管理器"));
    QCOMPARE(plugin.defaultOrder(), 330);
    QCOMPARE(plugin.visible(), true);
  }

  // Verify widget is created
  void testWidgetCreation() {
    SyncManagerPlugin plugin;
    QVERIFY(plugin.widget() != nullptr);
  }

  // Verify initial counts for status, history, settings, and log
  void testInitialState() {
    SyncManagerPlugin plugin;

    QCOMPARE(plugin.statusCount(), 3);
    QCOMPARE(plugin.historyCount(), 3);
    QCOMPARE(plugin.settingCount(), 4);
    QCOMPARE(plugin.logCount(), 3);
  }

  // Check status table has correct dimensions
  void testStatusTable() {
    SyncManagerPlugin plugin;

    QTableWidget *table = plugin.statusTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 3);
    QCOMPARE(table->columnCount(), 7);
  }

  // Check history table has correct dimensions
  void testHistoryTable() {
    SyncManagerPlugin plugin;

    QTableWidget *table = plugin.historyTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 3);
    QCOMPARE(table->columnCount(), 6);
  }

  // Check settings table has correct dimensions
  void testSettingsTable() {
    SyncManagerPlugin plugin;

    QTableWidget *table = plugin.settingsTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 4);
    QCOMPARE(table->columnCount(), 5);
  }

  // Check log table has correct dimensions
  void testLogTable() {
    SyncManagerPlugin plugin;

    QTableWidget *table = plugin.logTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 3);
    QCOMPARE(table->columnCount(), 4);
  }

  // Test adding a sync status entry
  void testAddStatus() {
    SyncManagerPlugin plugin;
    QSignalSpy spy(&plugin, &SyncManagerPlugin::syncStatusChanged);
    int initial = plugin.statusCount();

    SyncManagerPlugin::SyncStatusEntry s;
    s.id = "s_new";
    s.name = "Test Sync";
    s.type = "Custom";
    s.state = "Active";
    s.progress = 50;
    s.lastSync = QDateTime::currentDateTime();
    s.message = "Syncing";

    plugin.addStatus(s);
    QCOMPARE(plugin.statusCount(), initial + 1);
    QCOMPARE(spy.count(), 1);
  }

  // Test removing a sync status entry
  void testRemoveStatus() {
    SyncManagerPlugin plugin;
    int initial = plugin.statusCount();

    plugin.removeStatus(0);
    QCOMPARE(plugin.statusCount(), initial - 1);
  }

  // Test adding a history entry
  void testAddHistoryEntry() {
    SyncManagerPlugin plugin;
    int initial = plugin.historyCount();

    SyncManagerPlugin::SyncHistoryEntry h;
    h.timestamp = QDateTime::currentDateTime();
    h.syncId = "s1";
    h.name = "Test";
    h.result = "Success";
    h.duration = 10;
    h.details = "OK";

    plugin.addHistoryEntry(h);
    QCOMPARE(plugin.historyCount(), initial + 1);
  }

  // Test adding a setting
  void testAddSetting() {
    SyncManagerPlugin plugin;
    int initial = plugin.settingCount();

    SyncManagerPlugin::SyncSetting s;
    s.id = "set_new";
    s.name = "New Setting";
    s.description = "A new setting";
    s.value = "default";
    s.defaultValue = "default";

    plugin.addSetting(s);
    QCOMPARE(plugin.settingCount(), initial + 1);
  }

  // Test updating a setting value
  void testUpdateSetting() {
    SyncManagerPlugin plugin;

    plugin.updateSetting(0, "2000");
    QCOMPARE(plugin.settingsTable()->item(0, 3)->text(), QString("2000"));
  }

  // Test adding a log entry
  void testAddLog() {
    SyncManagerPlugin plugin;
    int initial = plugin.logCount();

    SyncManagerPlugin::SyncLog log;
    log.timestamp = QDateTime::currentDateTime();
    log.source = "test";
    log.level = "info";
    log.message = "test log";

    plugin.addLog(log);
    QCOMPARE(plugin.logCount(), initial + 1);
  }

  // Test filtering log entries by level
  void testFilterLogs() {
    SyncManagerPlugin plugin;

    plugin.filterLogs("error", "");
    QVERIFY(plugin.logTable()->rowCount() >= 1);
  }

  // Verify status label widget exists
  void testStatusLabel() {
    SyncManagerPlugin plugin;

    QLabel *label = plugin.statusLabel();
    QVERIFY(label != nullptr);
  }

  // Test exporting sync report to file
  void testExportReport() {
    SyncManagerPlugin plugin;

    QString path = QDir::temp().absoluteFilePath("sync_report_test.txt");
    plugin.exportReport(path);
    QVERIFY(QFile::exists(path));
    QFile::remove(path);
  }
};

QTEST_MAIN(SyncManagerPluginTest)
#include "syncmanager_plugin_test.moc"
