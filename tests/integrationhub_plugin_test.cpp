// IntegrationHubPluginTest — Tests for IntegrationHubPlugin
//
// Test coverage:
//   - Plugin identity (id, display names, order, visibility)
//   - Widget creation
//   - Initial connection, mapping, sync, and log counts
//   - Connection table, mapping table, sync table, log table structure
//   - Add/remove connections with signals
//   - Add/remove mappings
//   - Sync status updates
//   - Log entry add and filter
//   - Status label and export report

#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QLabel>
#include "plugins/integrationhub/IntegrationHubPlugin.h"

class IntegrationHubPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, display names, order, visibility
  void testPluginIdentity() {
    IntegrationHubPlugin plugin;

    QCOMPARE(plugin.id(), QString("integrationhub"));
    QCOMPARE(plugin.displayName(), QString("Integration Hub"));
    QCOMPARE(plugin.displayNameZh(), QString("集成中心"));
    QCOMPARE(plugin.defaultOrder(), 325);
    QCOMPARE(plugin.visible(), true);
  }

  // Check widget is created
  void testWidgetCreation() {
    IntegrationHubPlugin plugin;
    QVERIFY(plugin.widget() != nullptr);
  }

  // Verify initial data counts
  void testInitialState() {
    IntegrationHubPlugin plugin;

    QCOMPARE(plugin.connectionCount(), 3);
    QCOMPARE(plugin.mappingCount(), 3);
    QCOMPARE(plugin.syncStatusCount(), 3);
    QCOMPARE(plugin.logCount(), 3);
  }

  // Check connection table dimensions
  void testConnectionTable() {
    IntegrationHubPlugin plugin;

    QTableWidget *table = plugin.connectionTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 3);
    QCOMPARE(table->columnCount(), 6);
  }

  // Check mapping table dimensions
  void testMappingTable() {
    IntegrationHubPlugin plugin;

    QTableWidget *table = plugin.mappingTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 3);
    QCOMPARE(table->columnCount(), 5);
  }

  // Check sync table dimensions
  void testSyncTable() {
    IntegrationHubPlugin plugin;

    QTableWidget *table = plugin.syncTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 3);
    QCOMPARE(table->columnCount(), 6);
  }

  // Check log table dimensions
  void testLogTable() {
    IntegrationHubPlugin plugin;

    QTableWidget *table = plugin.logTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 3);
    QCOMPARE(table->columnCount(), 5);
  }

  // Test adding a connection with signal verification
  void testAddConnection() {
    IntegrationHubPlugin plugin;
    QSignalSpy spy(&plugin, &IntegrationHubPlugin::connectionAdded);
    int initial = plugin.connectionCount();

    IntegrationHubPlugin::SystemConnection c;
    c.id = "c_new";
    c.name = "Test Connection";
    c.type = "REST";
    c.endpoint = "http://test";
    c.status = "Connected";

    plugin.addConnection(c);
    QCOMPARE(plugin.connectionCount(), initial + 1);
    QCOMPARE(spy.count(), 1);
  }

  // Test removing a connection with signal verification
  void testRemoveConnection() {
    IntegrationHubPlugin plugin;
    QSignalSpy spy(&plugin, &IntegrationHubPlugin::connectionRemoved);
    int initial = plugin.connectionCount();

    plugin.removeConnection(0);
    QCOMPARE(plugin.connectionCount(), initial - 1);
    QCOMPARE(spy.count(), 1);
  }

  // Test adding a data mapping
  void testAddMapping() {
    IntegrationHubPlugin plugin;
    int initial = plugin.mappingCount();

    IntegrationHubPlugin::DataMapping m;
    m.id = "m_new";
    m.source = "src";
    m.destination = "dst";
    m.transformation = "Direct";
    m.enabled = true;

    plugin.addMapping(m);
    QCOMPARE(plugin.mappingCount(), initial + 1);
  }

  // Test removing a data mapping
  void testRemoveMapping() {
    IntegrationHubPlugin plugin;
    int initial = plugin.mappingCount();

    plugin.removeMapping(0);
    QCOMPARE(plugin.mappingCount(), initial - 1);
  }

  // Test updating sync status
  void testUpdateSyncStatus() {
    IntegrationHubPlugin plugin;

    IntegrationHubPlugin::SyncStatus s;
    s.connectionId = "c1";
    s.connectionName = "Updated";
    s.state = "Synced";
    s.progress = 100;
    s.lastUpdate = QDateTime::currentDateTime();
    s.message = "Done";

    plugin.updateSyncStatus(s);
    QCOMPARE(plugin.syncStatusCount(), 3);
    QCOMPARE(plugin.syncTable()->item(0, 2)->text(), QString("Synced"));
  }

  // Test adding a log entry
  void testAddLog() {
    IntegrationHubPlugin plugin;
    int initial = plugin.logCount();

    IntegrationHubPlugin::IntegrationLog log;
    log.timestamp = QDateTime::currentDateTime();
    log.source = "test";
    log.level = "info";
    log.message = "test message";
    log.details = "test details";

    plugin.addLog(log);
    QCOMPARE(plugin.logCount(), initial + 1);
  }

  // Test log filtering by level
  void testFilterLogs() {
    IntegrationHubPlugin plugin;

    plugin.filterLogs("error", "");
    QVERIFY(plugin.logTable()->rowCount() >= 1);
  }

  // Check status label exists
  void testStatusLabel() {
    IntegrationHubPlugin plugin;

    QLabel *label = plugin.statusLabel();
    QVERIFY(label != nullptr);
  }

  // Test exporting report to file
  void testExportReport() {
    IntegrationHubPlugin plugin;

    QString path = QDir::temp().absoluteFilePath("integration_report_test.txt");
    plugin.exportReport(path);
    QVERIFY(QFile::exists(path));
    QFile::remove(path);
  }
};

QTEST_MAIN(IntegrationHubPluginTest)
#include "integrationhub_plugin_test.moc"
