// AnalyticsDashboardPluginTest — Tests for AnalyticsDashboardPlugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Metrics CRUD and min/max tracking
//   - Trends management
//   - Reports management
//   - Filters management
//   - Signal emissions

#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTextEdit>
#include <QLabel>
#include "plugins/analyticsdashboard/AnalyticsDashboardPlugin.h"

class AnalyticsDashboardPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, display names, and default order
  void testPluginIdentity() {
    AnalyticsDashboardPlugin plugin;
    QCOMPARE(plugin.id(), QString("analytics_dashboard"));
    QCOMPARE(plugin.displayName(), QString("Analytics Dashboard"));
    QCOMPARE(plugin.displayNameZh(), QString("分析仪表盘"));
    QCOMPARE(plugin.defaultOrder(), 92);
    QCOMPARE(plugin.visible(), true);
  }

  // Verify widget is created
  void testWidgetCreation() {
    AnalyticsDashboardPlugin plugin;
    QVERIFY(plugin.widget() != nullptr);
  }

  // Verify initial counts are zero
  void testInitialState() {
    AnalyticsDashboardPlugin plugin;
    QCOMPARE(plugin.metricCount(), 0);
    QCOMPARE(plugin.trendCount(), 0);
    QCOMPARE(plugin.reportCount(), 0);
    QCOMPARE(plugin.filterCount(), 0);
  }

  // Verify adding a metric with signal
  void testAddMetric() {
    AnalyticsDashboardPlugin plugin;
    QSignalSpy spy(&plugin, &AnalyticsDashboardPlugin::metricUpdated);

    AnalyticsDashboardPlugin::AnalyticsMetric m;
    m.name = "Throughput";
    m.value = 100.0;
    m.min = 100.0;
    m.max = 100.0;
    m.avg = 100.0;
    m.samples = 1;

    plugin.addMetric(m);
    QCOMPARE(plugin.metricCount(), 1);
    QCOMPARE(plugin.metrics()[0].name, QString("Throughput"));
  }

  // Verify updating a metric with signal
  void testUpdateMetric() {
    AnalyticsDashboardPlugin plugin;
    QSignalSpy spy(&plugin, &AnalyticsDashboardPlugin::metricUpdated);

    AnalyticsDashboardPlugin::AnalyticsMetric m;
    m.name = "Throughput";
    m.value = 100.0;
    m.min = 100.0;
    m.max = 100.0;
    m.avg = 100.0;
    m.samples = 1;
    plugin.addMetric(m);

    plugin.updateMetric(0, 120.0);
    QCOMPARE(plugin.metrics()[0].value, 120.0);
    QCOMPARE(plugin.metrics()[0].max, 120.0);
    QCOMPARE(plugin.metrics()[0].samples, 2);
    QCOMPARE(spy.count(), 1);
  }

  // Verify metric min/max tracking on update
  void testUpdateMetricMinMax() {
    AnalyticsDashboardPlugin plugin;

    AnalyticsDashboardPlugin::AnalyticsMetric m;
    m.name = "Test";
    m.value = 50.0;
    m.min = 50.0;
    m.max = 50.0;
    m.avg = 50.0;
    m.samples = 1;
    plugin.addMetric(m);

    plugin.updateMetric(0, 30.0);
    QCOMPARE(plugin.metrics()[0].min, 30.0);

    plugin.updateMetric(0, 80.0);
    QCOMPARE(plugin.metrics()[0].max, 80.0);
  }

  // Verify adding a trend with signal
  void testAddTrend() {
    AnalyticsDashboardPlugin plugin;
    QSignalSpy spy(&plugin, &AnalyticsDashboardPlugin::trendAdded);

    AnalyticsDashboardPlugin::AnalyticsTrend t;
    t.metric = "CPU";
    t.values = {10.0, 20.0, 30.0};
    t.timestamps = {QDateTime::currentDateTime(), QDateTime::currentDateTime(), QDateTime::currentDateTime()};
    t.slope = 10.0;
    t.direction = "up";

    plugin.addTrend(t);
    QCOMPARE(plugin.trendCount(), 1);
    QCOMPARE(spy.count(), 1);
  }

  // Verify adding a report with signal
  void testAddReport() {
    AnalyticsDashboardPlugin plugin;
    QSignalSpy spy(&plugin, &AnalyticsDashboardPlugin::reportGenerated);

    AnalyticsDashboardPlugin::AnalyticsReport r;
    r.title = "Weekly Report";
    r.generated = QDateTime::currentDateTime();
    r.summary = "All metrics nominal";

    plugin.addReport(r);
    QCOMPARE(plugin.reportCount(), 1);
    QCOMPARE(spy.count(), 1);
  }

  // Verify adding a filter increments count
  void testAddFilter() {
    AnalyticsDashboardPlugin plugin;
    AnalyticsDashboardPlugin::AnalyticsFilter f;
    f.field = "status";
    f.operator_ = "==";
    f.value = "OK";
    f.active = true;

    plugin.addFilter(f);
    QCOMPARE(plugin.filterCount(), 1);
  }

  // Verify removing a filter decrements count
  void testRemoveFilter() {
    AnalyticsDashboardPlugin plugin;

    AnalyticsDashboardPlugin::AnalyticsFilter f;
    f.field = "test";
    f.operator_ = "==";
    f.value = "val";
    f.active = true;
    plugin.addFilter(f);

    QCOMPARE(plugin.filterCount(), 1);
    plugin.removeFilter(0);
    QCOMPARE(plugin.filterCount(), 0);
  }

  // Verify toggling a filter changes active state
  void testToggleFilter() {
    AnalyticsDashboardPlugin plugin;
    QSignalSpy spy(&plugin, &AnalyticsDashboardPlugin::filterToggled);

    AnalyticsDashboardPlugin::AnalyticsFilter f;
    f.field = "test";
    f.operator_ = "==";
    f.value = "val";
    f.active = true;
    plugin.addFilter(f);

    plugin.toggleFilter(0);
    QCOMPARE(plugin.filters()[0].active, false);
    QCOMPARE(spy.count(), 1);

    plugin.toggleFilter(0);
    QCOMPARE(plugin.filters()[0].active, true);
  }

  // Verify metrics table structure
  void testMetricsTable() {
    AnalyticsDashboardPlugin plugin;
    plugin.widget();
    QTableWidget *table = plugin.metricsTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->columnCount(), 6);
  }

  // Verify trends table structure
  void testTrendsTable() {
    AnalyticsDashboardPlugin plugin;
    plugin.widget();
    QTableWidget *table = plugin.trendsTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->columnCount(), 4);
  }

  // Verify reports table structure
  void testReportsTable() {
    AnalyticsDashboardPlugin plugin;
    plugin.widget();
    QTableWidget *table = plugin.reportsTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->columnCount(), 3);
  }

  // Verify filters table structure
  void testFiltersTable() {
    AnalyticsDashboardPlugin plugin;
    plugin.widget();
    QTableWidget *table = plugin.filtersTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->columnCount(), 4);
  }

  // Verify report view is read-only
  void testReportView() {
    AnalyticsDashboardPlugin plugin;
    plugin.widget();
    QTextEdit *view = plugin.reportView();
    QVERIFY(view != nullptr);
    QVERIFY(view->isReadOnly());
  }

  // Verify status label exists
  void testStatusLabel() {
    AnalyticsDashboardPlugin plugin;
    plugin.widget();
    QLabel *label = plugin.statusLabel();
    QVERIFY(label != nullptr);
  }

  // Verify export report contains metric data
  void testExportReport() {
    AnalyticsDashboardPlugin plugin;

    AnalyticsDashboardPlugin::AnalyticsMetric m;
    m.name = "Throughput";
    m.value = 100.0;
    m.min = 80.0;
    m.max = 120.0;
    m.avg = 100.0;
    m.samples = 10;
    plugin.addMetric(m);

    QString report = plugin.exportReport();
    QVERIFY(report.contains("Analytics Report"));
    QVERIFY(report.contains("Throughput"));
  }

  // Verify refresh updates status label with metric count
  void testRefresh() {
    AnalyticsDashboardPlugin plugin;
    plugin.widget();

    AnalyticsDashboardPlugin::AnalyticsMetric m;
    m.name = "Test";
    m.value = 100.0;
    m.min = 100.0;
    m.max = 100.0;
    m.avg = 100.0;
    m.samples = 1;
    plugin.addMetric(m);

    plugin.refresh();
    QLabel *label = plugin.statusLabel();
    QVERIFY(label->text().contains("Last refreshed"));
    QVERIFY(label->text().contains("Metrics: 1"));
  }

  // Verify metrics table is populated after add
  void testMetricsTablePopulation() {
    AnalyticsDashboardPlugin plugin;
    plugin.widget();

    AnalyticsDashboardPlugin::AnalyticsMetric m;
    m.name = "Bandwidth";
    m.value = 75.0;
    m.min = 50.0;
    m.max = 100.0;
    m.avg = 75.0;
    m.samples = 20;
    plugin.addMetric(m);

    QTableWidget *table = plugin.metricsTable();
    QCOMPARE(table->rowCount(), 1);
    QCOMPARE(table->item(0, 0)->text(), QString("Bandwidth"));
    QCOMPARE(table->item(0, 5)->text(), QString("20"));
  }

  // Verify filters table reflects toggle state
  void testFiltersTablePopulation() {
    AnalyticsDashboardPlugin plugin;
    plugin.widget();

    AnalyticsDashboardPlugin::AnalyticsFilter f;
    f.field = "status";
    f.operator_ = "==";
    f.value = "OK";
    f.active = true;
    plugin.addFilter(f);

    plugin.toggleFilter(0);
    QTableWidget *table = plugin.filtersTable();
    QCOMPARE(table->item(0, 3)->text(), QString("No"));
  }
};

QTEST_MAIN(AnalyticsDashboardPluginTest)
#include "analyticsdashboard_plugin_test.moc"
