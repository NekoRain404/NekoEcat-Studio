// SystemMonitorPluginTest — Tests for System Monitor Plugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Initial state (CPU, memory, disk, network usage; alerts)
//   - Usage updates with history tracking
//   - Overview and alert table structure
//   - History view widget
//   - Alert add/remove and trigger checking
//   - Status label with refresh
//   - Overview status colors (NORMAL, CRITICAL, SATURATED)
#include "plugins/systemmonitor/SystemMonitorPlugin.h"
#include <QLabel>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTest>
#include <QTextEdit>

class SystemMonitorPluginTest : public QObject {
    Q_OBJECT
private slots:
    // Verify plugin ID, display name, order, and visibility
    void testPluginIdentity() {
        SystemMonitorPlugin plugin;
        QCOMPARE(plugin.id(), QString("systemmonitor"));
        QCOMPARE(plugin.displayName(), QString("System Monitor"));
        QCOMPARE(plugin.displayNameZh(), QString("系统监视器"));
        QCOMPARE(plugin.defaultOrder(), 275);
        QCOMPARE(plugin.visible(), false);
    }

    // Verify widget is created
    void testWidgetCreation() {
        SystemMonitorPlugin plugin;
        QVERIFY(plugin.widget() != nullptr);
    }

    // Verify initial resource usage is zero and alert count
    void testInitialState() {
        SystemMonitorPlugin plugin;
        QCOMPARE(plugin.cpuUsage(), 0.0);
        QCOMPARE(plugin.memoryUsage(), 0.0);
        QCOMPARE(plugin.diskUsage(), 0.0);
        QCOMPARE(plugin.networkUsage(), 0.0);
        QCOMPARE(plugin.alertCount(), 4);
        QCOMPARE(plugin.triggeredAlertCount(), 0);
    }

    // Test CPU usage update, signal, and history recording
    void testUpdateCpuUsage() {
        SystemMonitorPlugin plugin;
        QSignalSpy spy(&plugin, &SystemMonitorPlugin::usageUpdated);

        plugin.updateCpuUsage(75.5);
        QCOMPARE(plugin.cpuUsage(), 75.5);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(plugin.cpuHistory().size(), 1);
    }

    // Test memory usage update, signal, and history recording
    void testUpdateMemoryUsage() {
        SystemMonitorPlugin plugin;
        QSignalSpy spy(&plugin, &SystemMonitorPlugin::usageUpdated);

        plugin.updateMemoryUsage(60.0);
        QCOMPARE(plugin.memoryUsage(), 60.0);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(plugin.memoryHistory().size(), 1);
    }

    // Test disk usage update, signal, and history recording
    void testUpdateDiskUsage() {
        SystemMonitorPlugin plugin;
        QSignalSpy spy(&plugin, &SystemMonitorPlugin::usageUpdated);

        plugin.updateDiskUsage(45.0);
        QCOMPARE(plugin.diskUsage(), 45.0);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(plugin.diskHistory().size(), 1);
    }

    // Test network usage update, signal, and history recording
    void testUpdateNetworkUsage() {
        SystemMonitorPlugin plugin;
        QSignalSpy spy(&plugin, &SystemMonitorPlugin::usageUpdated);

        plugin.updateNetworkUsage(100.5);
        QCOMPARE(plugin.networkUsage(), 100.5);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(plugin.networkHistory().size(), 1);
    }

    // Verify overview table has correct dimensions
    void testOverviewTable() {
        SystemMonitorPlugin plugin;
        QTableWidget* table = plugin.overviewTable();
        QVERIFY(table != nullptr);
        QCOMPARE(table->columnCount(), 4);
        QCOMPARE(table->rowCount(), 4);
    }

    // Verify alert table has correct column count
    void testAlertTable() {
        SystemMonitorPlugin plugin;
        QTableWidget* table = plugin.alertTable();
        QVERIFY(table != nullptr);
        QCOMPARE(table->columnCount(), 5);
    }

    // Verify history view is read-only
    void testHistoryView() {
        SystemMonitorPlugin plugin;
        QTextEdit* view = plugin.historyView();
        QVERIFY(view != nullptr);
        QVERIFY(view->isReadOnly());
    }

    // Test adding an alert rule
    void testAddAlert() {
        SystemMonitorPlugin plugin;
        int initial = plugin.alertCount();

        SystemMonitorPlugin::AlertRule rule;
        rule.metric = "CPU";
        rule.threshold = 95.0;
        rule.condition = ">";
        rule.message = "Test alert";
        rule.triggered = false;

        plugin.addAlert(rule);
        QCOMPARE(plugin.alertCount(), initial + 1);
    }

    // Test removing an alert rule
    void testRemoveAlert() {
        SystemMonitorPlugin plugin;
        int initial = plugin.alertCount();

        plugin.removeAlert(0);
        QCOMPARE(plugin.alertCount(), initial - 1);
    }

    // Test alert triggering when threshold is exceeded
    void testCheckAlertsTriggered() {
        SystemMonitorPlugin plugin;
        QSignalSpy spy(&plugin, &SystemMonitorPlugin::alertTriggered);

        plugin.updateCpuUsage(95.0);
        plugin.checkAlerts();
        QVERIFY(plugin.triggeredAlertCount() > 0);
        QCOMPARE(spy.count(), 1);
    }

    // Verify no alerts triggered when below thresholds
    void testCheckAlertsNotTriggered() {
        SystemMonitorPlugin plugin;

        plugin.updateCpuUsage(50.0);
        plugin.updateMemoryUsage(50.0);
        plugin.updateDiskUsage(50.0);
        plugin.updateNetworkUsage(100.0);
        plugin.checkAlerts();
        QCOMPARE(plugin.triggeredAlertCount(), 0);
    }

    // Verify status label shows CPU info after refresh
    void testStatusLabel() {
        SystemMonitorPlugin plugin;
        QLabel* label = plugin.statusLabel();
        QVERIFY(label != nullptr);

        plugin.updateCpuUsage(50.0);
        plugin.refresh();
        QVERIFY(label->text().contains("CPU"));
    }

    // Test refresh populates history view
    void testRefresh() {
        SystemMonitorPlugin plugin;

        plugin.updateCpuUsage(42.0);
        plugin.updateMemoryUsage(55.0);
        plugin.refresh();

        QTextEdit* view = plugin.historyView();
        QVERIFY(!view->toPlainText().isEmpty());
    }

    // Verify multiple updates are recorded in history
    void testHistoryRecording() {
        SystemMonitorPlugin plugin;

        for (int i = 0; i < 5; ++i) {
            plugin.updateCpuUsage(50.0 + i);
        }
        QCOMPARE(plugin.cpuHistory().size(), 5);
    }

    // Test overview table shows CRITICAL/SATURATED at high usage
    void testOverviewStatusColors() {
        SystemMonitorPlugin plugin;

        plugin.updateCpuUsage(95.0);
        plugin.updateMemoryUsage(90.0);
        plugin.updateDiskUsage(98.0);
        plugin.updateNetworkUsage(950.0);
        plugin.refresh();

        QTableWidget* table = plugin.overviewTable();
        QCOMPARE(table->item(0, 3)->text(), QString("CRITICAL"));
        QCOMPARE(table->item(1, 3)->text(), QString("CRITICAL"));
        QCOMPARE(table->item(2, 3)->text(), QString("CRITICAL"));
        QCOMPARE(table->item(3, 3)->text(), QString("SATURATED"));
    }
};

QTEST_MAIN(SystemMonitorPluginTest)
#include "systemmonitor_plugin_test.moc"
