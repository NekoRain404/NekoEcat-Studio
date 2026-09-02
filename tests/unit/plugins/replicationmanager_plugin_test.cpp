// ReplicationManagerPluginTest — Tests for Replication Manager Plugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Empty fail-closed initial state
//   - Target/status/history/settings table structure
//   - Add/remove/update operations for targets, status, history, settings
//   - Status label and report export
#include "plugins/replicationmanager/ReplicationManagerPlugin.h"
#include <QFile>
#include <QLabel>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QTest>

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
        QCOMPARE(plugin.visible(), false);
    }

    // Widget should be created successfully
    void testWidgetCreation() {
        ReplicationManagerPlugin plugin;
        QVERIFY(plugin.widget() != nullptr);
    }

    // Default counts for targets, statuses, histories, and settings
    void testInitialState() {
        ReplicationManagerPlugin plugin;

        QCOMPARE(plugin.targetCount(), 0);
        QCOMPARE(plugin.statusCount(), 0);
        QCOMPARE(plugin.historyCount(), 0);
        QCOMPARE(plugin.settingCount(), 0);
    }

    // Target table has correct dimensions
    void testTargetTable() {
        ReplicationManagerPlugin plugin;

        QTableWidget* table = plugin.targetTable();
        QVERIFY(table != nullptr);
        QCOMPARE(table->rowCount(), 0);
        QCOMPARE(table->columnCount(), 6);
    }

    // Status table has correct dimensions
    void testStatusTable() {
        ReplicationManagerPlugin plugin;

        QTableWidget* table = plugin.statusTable();
        QVERIFY(table != nullptr);
        QCOMPARE(table->rowCount(), 0);
        QCOMPARE(table->columnCount(), 6);
    }

    // History table has correct dimensions
    void testHistoryTable() {
        ReplicationManagerPlugin plugin;

        QTableWidget* table = plugin.historyTable();
        QVERIFY(table != nullptr);
        QCOMPARE(table->rowCount(), 0);
        QCOMPARE(table->columnCount(), 6);
    }

    // Settings table has correct dimensions
    void testSettingsTable() {
        ReplicationManagerPlugin plugin;

        QTableWidget* table = plugin.settingsTable();
        QVERIFY(table != nullptr);
        QCOMPARE(table->rowCount(), 0);
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
        ReplicationManagerPlugin::ReplicationTarget t;
        t.id = "t_remove";
        t.name = "Remove Target";
        t.endpoint = "192.168.1.250:5877";
        t.type = "Full";
        t.enabled = false;
        plugin.addTarget(t);
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
        s.state = "Pending";
        s.progress = 0;
        s.lastUpdate = QDateTime::currentDateTime();
        s.message = "Waiting for replication backend";

        plugin.updateStatus(s);
        QCOMPARE(plugin.statusCount(), 1);
        QCOMPARE(plugin.statusTable()->item(0, 2)->text(), QString("Pending"));
    }

    // Add a history entry to the log
    void testAddHistoryEntry() {
        ReplicationManagerPlugin plugin;
        int initial = plugin.historyCount();

        ReplicationManagerPlugin::ReplicationHistoryEntry h;
        h.timestamp = QDateTime::currentDateTime();
        h.targetId = "t1";
        h.targetName = "Test";
        h.result = "Failed";
        h.objectsReplicated = 0;
        h.details = "No backend acknowledgement";

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
        ReplicationManagerPlugin::ReplicationSetting s;
        s.id = "rs1";
        s.name = "Replication Mode";
        s.description = "Full or incremental";
        s.value = "incremental";
        s.defaultValue = "incremental";
        plugin.addSetting(s);

        plugin.updateSetting(0, "full");
        QCOMPARE(plugin.settingsTable()->item(0, 3)->text(), QString("full"));
    }

    // Status label is created and accessible
    void testStatusLabel() {
        ReplicationManagerPlugin plugin;

        QLabel* label = plugin.statusLabel();
        QVERIFY(label != nullptr);
    }

    // Export report to file and verify file creation
    void testExportReport() {
        ReplicationManagerPlugin plugin;
        QTemporaryDir dir;
        QVERIFY(dir.isValid());

        plugin.addTarget(
            {"t_export", "Export Target", "192.168.1.250:5877", "Full", true, QDateTime::currentDateTime()});
        plugin.updateStatus({"t_export", "Export Target", "Pending", 15, QDateTime::currentDateTime(), "Waiting"});
        plugin.addHistoryEntry(
            {QDateTime::currentDateTime(), "t_export", "Export Target", "Failed", 0, "No backend acknowledgement"});

        const QString path = dir.filePath("replication_report_test.txt");
        QVERIFY(plugin.exportReport(path));
        QVERIFY(QFile::exists(path));

        QFile file(path);
        QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
        const QString text = QString::fromUtf8(file.readAll());
        QVERIFY(text.contains(QStringLiteral("Replication Manager Report\n")));
        QVERIFY(text.contains(QStringLiteral("Targets: 1\n")));
        QVERIFY(text.contains(QStringLiteral("Export Target [Full] 192.168.1.250:5877 enabled\n")));
        QVERIFY(text.contains(QStringLiteral("Export Target: Pending 15%\n")));
        QVERIFY(text.contains(QStringLiteral("Export Target Failed 0 objects\n")));

        QTest::failOnWarning(QRegularExpression(QStringLiteral("QFSFileEngine::open: No file name specified")));
        QVERIFY(!plugin.exportReport(QString()));
        QVERIFY(!plugin.exportReport(dir.path()));
    }

    void testSourceDoesNotMintSyntheticReplicationResults() {
        QFile file(
            QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/plugins/replicationmanager/ReplicationManagerPlugin.cpp"));
        QVERIFY2(file.open(QIODevice::ReadOnly | QIODevice::Text), qPrintable(file.errorString()));
        const QString source = QString::fromUtf8(file.readAll());

        QVERIFY2(!source.contains(QStringLiteral("\"Backup Server\", \"192.168.1.200:5877\"")),
                 "Replication manager UI must not seed backup targets");
        QVERIFY2(!source.contains(QStringLiteral("\"Standby Master\", \"192.168.1.201:5877\"")),
                 "Replication manager UI must not seed standby targets");
        QVERIFY2(!source.contains(QStringLiteral("\"Success\", 256, \"Full replication completed\"")),
                 "Replication manager UI must not seed successful full replication history");
        QVERIFY2(!source.contains(QStringLiteral("\"Auto Replicate\", \"Auto replicate on config change\", \"true\"")),
                 "Replication manager UI must not seed enabled automation settings");
    }
};

QTEST_MAIN(ReplicationManagerPluginTest)
#include "replicationmanager_plugin_test.moc"
