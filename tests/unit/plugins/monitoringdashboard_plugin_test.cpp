// MonitoringDashboardPluginTest — Tests for MonitoringDashboardPlugin (signal-based)
//
// Test coverage:
//   - Plugin identity (id, display names, order, visibility)
//   - Widget creation
//   - Initial state (metrics, alerts, events, dashboards)
//   - Metric add/update with threshold-based status
//   - Alert add/acknowledge with signals
//   - Event and dashboard management
//   - Table structure (metrics, alerts, events, dashboards)
//   - Report view and status label
//   - Export report content
//   - Refresh with table population

#include "plugins/monitoringdashboard/MonitoringDashboardPlugin.h"
#include <QLabel>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTest>
#include <QTextEdit>

class MonitoringDashboardPluginTest : public QObject {
    Q_OBJECT
private slots:
    // Verify plugin id, display names, order, visibility
    void testPluginIdentity() {
        MonitoringDashboardPlugin plugin;
        QCOMPARE(plugin.id(), QString("monitoring_dashboard"));
        QCOMPARE(plugin.displayName(), QString("Monitoring Dashboard"));
        QCOMPARE(plugin.displayNameZh(), QString("监控仪表盘"));
        QCOMPARE(plugin.defaultOrder(), 91);
        QCOMPARE(plugin.visible(), false);
    }

    // Check widget is created
    void testWidgetCreation() {
        MonitoringDashboardPlugin plugin;
        QVERIFY(plugin.widget() != nullptr);
    }

    // Verify all initial counts are zero
    void testInitialState() {
        MonitoringDashboardPlugin plugin;
        QCOMPARE(plugin.metricCount(), 0);
        QCOMPARE(plugin.alertCount(), 0);
        QCOMPARE(plugin.eventCount(), 0);
        QCOMPARE(plugin.dashboardCount(), 0);
        QCOMPARE(plugin.activeAlertCount(), 0);
    }

    // Test adding a metric
    void testAddMetric() {
        MonitoringDashboardPlugin plugin;
        QSignalSpy spy(&plugin, &MonitoringDashboardPlugin::metricUpdated);

        MonitoringDashboardPlugin::MonitoringMetric m;
        m.name = "CPU Load";
        m.value = 45.0;
        m.threshold = 90.0;
        m.status = "OK";

        plugin.addMetric(m);
        QCOMPARE(plugin.metricCount(), 1);
        QCOMPARE(plugin.metrics()[0].name, QString("CPU Load"));
    }

    // Test metric update triggers ALERT status above threshold
    void testUpdateMetric() {
        MonitoringDashboardPlugin plugin;
        QSignalSpy spy(&plugin, &MonitoringDashboardPlugin::metricUpdated);

        MonitoringDashboardPlugin::MonitoringMetric m;
        m.name = "CPU Load";
        m.value = 45.0;
        m.threshold = 90.0;
        m.status = "OK";
        plugin.addMetric(m);

        plugin.updateMetric(0, 95.0);
        QCOMPARE(plugin.metrics()[0].value, 95.0);
        QCOMPARE(plugin.metrics()[0].status, QString("ALERT"));
        QCOMPARE(spy.count(), 1);
    }

    // Test metric stays OK below threshold
    void testUpdateMetricBelowThreshold() {
        MonitoringDashboardPlugin plugin;

        MonitoringDashboardPlugin::MonitoringMetric m;
        m.name = "CPU";
        m.value = 45.0;
        m.threshold = 90.0;
        m.status = "OK";
        plugin.addMetric(m);

        plugin.updateMetric(0, 50.0);
        QCOMPARE(plugin.metrics()[0].status, QString("OK"));
    }

    // Test adding an alert with signal
    void testAddAlert() {
        MonitoringDashboardPlugin plugin;
        QSignalSpy spy(&plugin, &MonitoringDashboardPlugin::alertAdded);

        MonitoringDashboardPlugin::MonitoringAlert a;
        a.timestamp = QDateTime::currentDateTime();
        a.severity = "Critical";
        a.message = "Temperature high";
        a.acknowledged = false;

        plugin.addAlert(a);
        QCOMPARE(plugin.alertCount(), 1);
        QCOMPARE(plugin.activeAlertCount(), 1);
        QCOMPARE(spy.count(), 1);
    }

    // Test acknowledging an alert with signal
    void testAcknowledgeAlert() {
        MonitoringDashboardPlugin plugin;
        QSignalSpy spy(&plugin, &MonitoringDashboardPlugin::alertAcknowledged);

        MonitoringDashboardPlugin::MonitoringAlert a;
        a.timestamp = QDateTime::currentDateTime();
        a.severity = "Warning";
        a.message = "Test alert";
        a.acknowledged = false;
        plugin.addAlert(a);

        QCOMPARE(plugin.activeAlertCount(), 1);
        plugin.acknowledgeAlert(0);
        QCOMPARE(plugin.activeAlertCount(), 0);
        QCOMPARE(spy.count(), 1);
    }

    // Test adding an event
    void testAddEvent() {
        MonitoringDashboardPlugin plugin;
        MonitoringDashboardPlugin::MonitoringEvent e;
        e.timestamp = QDateTime::currentDateTime();
        e.category = "System";
        e.description = "Started";
        e.level = "Info";

        plugin.addEvent(e);
        QCOMPARE(plugin.eventCount(), 1);
    }

    // Test adding a dashboard
    void testAddDashboard() {
        MonitoringDashboardPlugin plugin;
        MonitoringDashboardPlugin::MonitoringDashboard d;
        d.name = "Main";
        d.lastUpdate = QDateTime::currentDateTime();

        plugin.addDashboard(d);
        QCOMPARE(plugin.dashboardCount(), 1);
    }

    // Check metrics table column count
    void testMetricsTable() {
        MonitoringDashboardPlugin plugin;
        plugin.widget();
        QTableWidget* table = plugin.metricsTable();
        QVERIFY(table != nullptr);
        QCOMPARE(table->columnCount(), 4);
    }

    // Check alerts table column count
    void testAlertsTable() {
        MonitoringDashboardPlugin plugin;
        plugin.widget();
        QTableWidget* table = plugin.alertsTable();
        QVERIFY(table != nullptr);
        QCOMPARE(table->columnCount(), 4);
    }

    // Check events table column count
    void testEventsTable() {
        MonitoringDashboardPlugin plugin;
        plugin.widget();
        QTableWidget* table = plugin.eventsTable();
        QVERIFY(table != nullptr);
        QCOMPARE(table->columnCount(), 4);
    }

    // Check dashboards table column count
    void testDashboardsTable() {
        MonitoringDashboardPlugin plugin;
        plugin.widget();
        QTableWidget* table = plugin.dashboardsTable();
        QVERIFY(table != nullptr);
        QCOMPARE(table->columnCount(), 3);
    }

    // Check report view is read-only
    void testReportView() {
        MonitoringDashboardPlugin plugin;
        plugin.widget();
        QTextEdit* view = plugin.reportView();
        QVERIFY(view != nullptr);
        QVERIFY(view->isReadOnly());
    }

    // Check status label exists
    void testStatusLabel() {
        MonitoringDashboardPlugin plugin;
        plugin.widget();
        QLabel* label = plugin.statusLabel();
        QVERIFY(label != nullptr);
    }

    // Test export report content
    void testExportReport() {
        MonitoringDashboardPlugin plugin;

        MonitoringDashboardPlugin::MonitoringMetric m;
        m.name = "CPU";
        m.value = 50.0;
        m.threshold = 90.0;
        m.status = "OK";
        plugin.addMetric(m);

        QString report = plugin.exportReport();
        QVERIFY(report.contains("Monitoring Report"));
        QVERIFY(report.contains("CPU"));
    }

    // Test refresh updates status with metric count
    void testRefresh() {
        MonitoringDashboardPlugin plugin;
        plugin.widget();

        MonitoringDashboardPlugin::MonitoringMetric m;
        m.name = "Test";
        m.value = 100.0;
        m.threshold = 90.0;
        m.status = "OK";
        plugin.addMetric(m);

        plugin.refresh();
        QLabel* label = plugin.statusLabel();
        QVERIFY(label->text().contains("Last refreshed"));
        QVERIFY(label->text().contains("Metrics: 1"));
    }

    // Test metrics table row population
    void testMetricsTablePopulation() {
        MonitoringDashboardPlugin plugin;
        plugin.widget();

        MonitoringDashboardPlugin::MonitoringMetric m;
        m.name = "Bandwidth";
        m.value = 75.0;
        m.threshold = 80.0;
        m.status = "OK";
        plugin.addMetric(m);

        QTableWidget* table = plugin.metricsTable();
        QCOMPARE(table->rowCount(), 1);
        QCOMPARE(table->item(0, 0)->text(), QString("Bandwidth"));
        QCOMPARE(table->item(0, 3)->text(), QString("OK"));
    }

    // Test alerts table acknowledge column population
    void testAlertsTablePopulation() {
        MonitoringDashboardPlugin plugin;
        plugin.widget();

        MonitoringDashboardPlugin::MonitoringAlert a;
        a.timestamp = QDateTime::currentDateTime();
        a.severity = "Critical";
        a.message = "Overheating";
        a.acknowledged = false;
        plugin.addAlert(a);

        plugin.acknowledgeAlert(0);
        QTableWidget* table = plugin.alertsTable();
        QCOMPARE(table->item(0, 3)->text(), QString("Yes"));
    }
};

QTEST_MAIN(MonitoringDashboardPluginTest)
#include "monitoringdashboard_plugin_test.moc"
