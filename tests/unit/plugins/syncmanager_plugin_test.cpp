// SyncManagerPluginTest — Tests for Sync Manager Plugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Empty fail-closed initial state
//   - Status/history/settings/log table structure
//   - Add/remove operations for status, history, settings, logs
//   - Log filtering
//   - Status label and report export
#include "plugins/syncmanager/SyncManagerPlugin.h"
#include <QFile>
#include <QLabel>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QTest>

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
        QCOMPARE(plugin.visible(), false);
    }

    // Verify widget is created
    void testWidgetCreation() {
        SyncManagerPlugin plugin;
        QVERIFY(plugin.widget() != nullptr);
    }

    // Verify initial counts for status, history, settings, and log
    void testInitialState() {
        SyncManagerPlugin plugin;

        QCOMPARE(plugin.statusCount(), 0);
        QCOMPARE(plugin.historyCount(), 0);
        QCOMPARE(plugin.settingCount(), 0);
        QCOMPARE(plugin.logCount(), 0);
    }

    // Check status table has correct dimensions
    void testStatusTable() {
        SyncManagerPlugin plugin;

        QTableWidget* table = plugin.statusTable();
        QVERIFY(table != nullptr);
        QCOMPARE(table->rowCount(), 0);
        QCOMPARE(table->columnCount(), 7);
    }

    // Check history table has correct dimensions
    void testHistoryTable() {
        SyncManagerPlugin plugin;

        QTableWidget* table = plugin.historyTable();
        QVERIFY(table != nullptr);
        QCOMPARE(table->rowCount(), 0);
        QCOMPARE(table->columnCount(), 6);
    }

    // Check settings table has correct dimensions
    void testSettingsTable() {
        SyncManagerPlugin plugin;

        QTableWidget* table = plugin.settingsTable();
        QVERIFY(table != nullptr);
        QCOMPARE(table->rowCount(), 0);
        QCOMPARE(table->columnCount(), 5);
    }

    // Check log table has correct dimensions
    void testLogTable() {
        SyncManagerPlugin plugin;

        QTableWidget* table = plugin.logTable();
        QVERIFY(table != nullptr);
        QCOMPARE(table->rowCount(), 0);
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
        s.state = "Pending";
        s.progress = 0;
        s.lastSync = QDateTime::currentDateTime();
        s.message = "Syncing";

        plugin.addStatus(s);
        QCOMPARE(plugin.statusCount(), initial + 1);
        QCOMPARE(spy.count(), 1);
    }

    // Test removing a sync status entry
    void testRemoveStatus() {
        SyncManagerPlugin plugin;
        SyncManagerPlugin::SyncStatusEntry s;
        s.id = "remove";
        s.name = "Remove Sync";
        s.type = "Custom";
        s.state = "Pending";
        s.progress = 0;
        plugin.addStatus(s);
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
        h.result = "Failed";
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
        SyncManagerPlugin::SyncSetting s;
        s.id = "set1";
        s.name = "Sync Interval";
        s.description = "Sync check interval in ms";
        s.value = "1000";
        s.defaultValue = "1000";
        plugin.addSetting(s);

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
        SyncManagerPlugin::SyncLog log;
        log.timestamp = QDateTime::currentDateTime();
        log.source = "test";
        log.level = "error";
        log.message = "test error";
        plugin.addLog(log);

        plugin.filterLogs("error", "");
        QVERIFY(plugin.logTable()->rowCount() >= 1);
    }

    // Verify status label widget exists
    void testStatusLabel() {
        SyncManagerPlugin plugin;

        QLabel* label = plugin.statusLabel();
        QVERIFY(label != nullptr);
    }

    // Test exporting sync report to file
    void testExportReport() {
        SyncManagerPlugin plugin;
        QTemporaryDir dir;
        QVERIFY(dir.isValid());

        plugin.addStatus({"s_export", "Export Sync", "Custom", "Pending", 25, QDateTime::currentDateTime(), "Waiting"});
        plugin.addHistoryEntry({QDateTime::currentDateTime(), "s_export", "Export Sync", "Failed", 42, "Timeout"});
        plugin.addSetting({"set_export", "Export Interval", "Interval", "1000", "500"});

        const QString path = dir.filePath("sync_report_test.txt");
        QVERIFY(plugin.exportReport(path));
        QVERIFY(QFile::exists(path));

        QFile file(path);
        QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
        const QString text = QString::fromUtf8(file.readAll());
        QVERIFY(text.contains(QStringLiteral("Sync Manager Report\n")));
        QVERIFY(text.contains(QStringLiteral("Active Syncs: 1\n")));
        QVERIFY(text.contains(QStringLiteral("Export Sync [Custom] Pending 25%\n")));
        QVERIFY(text.contains(QStringLiteral("Export Sync Failed 42ms\n")));
        QVERIFY(text.contains(QStringLiteral("Export Interval: 1000 (default: 500)\n")));

        QTest::failOnWarning(QRegularExpression(QStringLiteral("QFSFileEngine::open: No file name specified")));
        QVERIFY(!plugin.exportReport(QString()));
        QVERIFY(!plugin.exportReport(dir.path()));
    }

    void testSourceDoesNotMintSyntheticSyncResults() {
        QFile file(QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/plugins/syncmanager/SyncManagerPlugin.cpp"));
        QVERIFY2(file.open(QIODevice::ReadOnly | QIODevice::Text), qPrintable(file.errorString()));
        const QString source = QString::fromUtf8(file.readAll());

        QVERIFY2(!source.contains(QStringLiteral("\"DC Sync\", \"Distributed Clock\", \"Active\", 100")),
                 "Sync manager UI must not seed active DC sync status");
        QVERIFY2(!source.contains(QStringLiteral("\"PDO Sync\", \"Process Data\", \"Active\", 100")),
                 "Sync manager UI must not seed active PDO sync status");
        QVERIFY2(!source.contains(QStringLiteral("\"Success\", 12, \"Sync completed in 12ms\"")),
                 "Sync manager UI must not seed successful sync history");
        QVERIFY2(!source.contains(QStringLiteral("\"Clock synchronization established\"")),
                 "Sync manager UI must not seed successful sync logs");
    }
};

QTEST_MAIN(SyncManagerPluginTest)
#include "syncmanager_plugin_test.moc"
