// OptimizationDashboardPluginTest — Tests for OptimizationDashboardPlugin (signal-based)
//
// Test coverage:
//   - Plugin identity (id, display names, order, visibility)
//   - Widget creation
//   - Initial state (metrics, history, recommendations, actions)
//   - Metric add/update with signals
//   - History entry management
//   - Recommendation add/remove with signals
//   - Action add/execute with signals
//   - Table structure (metrics, history, recommendations, actions)
//   - Report view and status label
//   - Export report content
//   - Refresh and table population

#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTextEdit>
#include <QLabel>
#include "plugins/optimizationdashboard/OptimizationDashboardPlugin.h"

class OptimizationDashboardPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, display names, order, visibility
  void testPluginIdentity() {
    OptimizationDashboardPlugin plugin;
    QCOMPARE(plugin.id(), QString("optimization_dashboard"));
    QCOMPARE(plugin.displayName(), QString("Optimization Dashboard"));
    QCOMPARE(plugin.displayNameZh(), QString("优化仪表盘"));
    QCOMPARE(plugin.defaultOrder(), 90);
    QCOMPARE(plugin.visible(), false);
  }

  // Check widget is created
  void testWidgetCreation() {
    OptimizationDashboardPlugin plugin;
    QVERIFY(plugin.widget() != nullptr);
  }

  // Verify all initial counts are zero
  void testInitialState() {
    OptimizationDashboardPlugin plugin;
    QCOMPARE(plugin.metricCount(), 0);
    QCOMPARE(plugin.historyCount(), 0);
    QCOMPARE(plugin.recommendationCount(), 0);
    QCOMPARE(plugin.actionCount(), 0);
  }

  // Test adding a metric with signal
  void testAddMetric() {
    OptimizationDashboardPlugin plugin;
    QSignalSpy spy(&plugin, &OptimizationDashboardPlugin::metricUpdated);

    OptimizationDashboardPlugin::OptimizationMetric m;
    m.name = "Cycle Time";
    m.value = 1000.0;
    m.target = 900.0;
    m.improvement = 0.0;

    plugin.addMetric(m);
    QCOMPARE(plugin.metricCount(), 1);
    QCOMPARE(plugin.metrics()[0].name, QString("Cycle Time"));
  }

  // Test metric update with signal
  void testUpdateMetric() {
    OptimizationDashboardPlugin plugin;
    QSignalSpy spy(&plugin, &OptimizationDashboardPlugin::metricUpdated);

    OptimizationDashboardPlugin::OptimizationMetric m;
    m.name = "Cycle Time";
    m.value = 1000.0;
    m.target = 900.0;
    m.improvement = 0.0;
    plugin.addMetric(m);

    plugin.updateMetric(0, 850.0);
    QCOMPARE(plugin.metrics()[0].value, 850.0);
    QCOMPARE(spy.count(), 1);
  }

  // Test adding a history entry
  void testAddHistoryEntry() {
    OptimizationDashboardPlugin plugin;
    OptimizationDashboardPlugin::OptimizationHistoryEntry e;
    e.timestamp = QDateTime::currentDateTime();
    e.action = "Optimize cycle";
    e.result = "Improved";
    e.improvement = 15.0;

    plugin.addHistoryEntry(e);
    QCOMPARE(plugin.historyCount(), 1);
  }

  // Test adding a recommendation with signal
  void testAddRecommendation() {
    OptimizationDashboardPlugin plugin;
    QSignalSpy spy(&plugin, &OptimizationDashboardPlugin::recommendationAdded);

    OptimizationDashboardPlugin::OptimizationRecommendation r;
    r.title = "Reduce cycle time";
    r.description = "Lower DC sync interval";
    r.priority = "High";
    r.category = "Performance";

    plugin.addRecommendation(r);
    QCOMPARE(plugin.recommendationCount(), 1);
    QCOMPARE(spy.count(), 1);
  }

  // Test removing a recommendation
  void testRemoveRecommendation() {
    OptimizationDashboardPlugin plugin;

    OptimizationDashboardPlugin::OptimizationRecommendation r;
    r.title = "Test";
    r.description = "Test desc";
    r.priority = "Low";
    r.category = "Test";
    plugin.addRecommendation(r);

    QCOMPARE(plugin.recommendationCount(), 1);
    plugin.removeRecommendation(0);
    QCOMPARE(plugin.recommendationCount(), 0);
  }

  // Test adding an action
  void testAddAction() {
    OptimizationDashboardPlugin plugin;
    OptimizationDashboardPlugin::OptimizationAction a;
    a.name = "Tune DC";
    a.description = "Adjust DC sync";
    a.executed = false;
    a.result = "";

    plugin.addAction(a);
    QCOMPARE(plugin.actionCount(), 1);
    QCOMPARE(plugin.actions()[0].executed, false);
  }

  // Test executing an action with signal
  void testExecuteAction() {
    OptimizationDashboardPlugin plugin;
    QSignalSpy spy(&plugin, &OptimizationDashboardPlugin::actionExecuted);

    OptimizationDashboardPlugin::OptimizationAction a;
    a.name = "Tune DC";
    a.description = "Adjust DC sync";
    a.executed = false;
    a.result = "";
    plugin.addAction(a);

    plugin.executeAction(0);
    QCOMPARE(plugin.actions()[0].executed, true);
    QCOMPARE(plugin.actions()[0].result, QString("Executed successfully"));
    QCOMPARE(spy.count(), 1);
  }

  // Check metrics table column count
  void testMetricsTable() {
    OptimizationDashboardPlugin plugin;
    plugin.widget();
    QTableWidget *table = plugin.metricsTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->columnCount(), 4);
  }

  // Check history table column count
  void testHistoryTable() {
    OptimizationDashboardPlugin plugin;
    plugin.widget();
    QTableWidget *table = plugin.historyTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->columnCount(), 4);
  }

  // Check recommendations table column count
  void testRecommendationsTable() {
    OptimizationDashboardPlugin plugin;
    plugin.widget();
    QTableWidget *table = plugin.recommendationsTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->columnCount(), 4);
  }

  // Check actions table column count
  void testActionsTable() {
    OptimizationDashboardPlugin plugin;
    plugin.widget();
    QTableWidget *table = plugin.actionsTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->columnCount(), 4);
  }

  // Check report view is read-only
  void testReportView() {
    OptimizationDashboardPlugin plugin;
    plugin.widget();
    QTextEdit *view = plugin.reportView();
    QVERIFY(view != nullptr);
    QVERIFY(view->isReadOnly());
  }

  // Check status label exists
  void testStatusLabel() {
    OptimizationDashboardPlugin plugin;
    plugin.widget();
    QLabel *label = plugin.statusLabel();
    QVERIFY(label != nullptr);
  }

  // Test export report content
  void testExportReport() {
    OptimizationDashboardPlugin plugin;

    OptimizationDashboardPlugin::OptimizationMetric m;
    m.name = "Cycle Time";
    m.value = 850.0;
    m.target = 900.0;
    m.improvement = -5.6;
    plugin.addMetric(m);

    QString report = plugin.exportReport();
    QVERIFY(report.contains("Optimization Report"));
    QVERIFY(report.contains("Cycle Time"));
  }

  // Test refresh updates status label
  void testRefresh() {
    OptimizationDashboardPlugin plugin;
    plugin.widget();

    OptimizationDashboardPlugin::OptimizationMetric m;
    m.name = "Test";
    m.value = 100.0;
    m.target = 90.0;
    m.improvement = 0.0;
    plugin.addMetric(m);

    plugin.refresh();
    QLabel *label = plugin.statusLabel();
    QVERIFY(label->text().contains("Last refreshed"));
  }

  // Test metrics table row population
  void testMetricsTablePopulation() {
    OptimizationDashboardPlugin plugin;
    plugin.widget();

    OptimizationDashboardPlugin::OptimizationMetric m;
    m.name = "Bandwidth";
    m.value = 75.0;
    m.target = 80.0;
    m.improvement = -6.25;
    plugin.addMetric(m);

    QTableWidget *table = plugin.metricsTable();
    QCOMPARE(table->rowCount(), 1);
    QCOMPARE(table->item(0, 0)->text(), QString("Bandwidth"));
  }

  // Test actions table executed column population
  void testActionsTablePopulation() {
    OptimizationDashboardPlugin plugin;
    plugin.widget();

    OptimizationDashboardPlugin::OptimizationAction a;
    a.name = "Test Action";
    a.description = "Test";
    a.executed = false;
    a.result = "";
    plugin.addAction(a);

    plugin.executeAction(0);
    QTableWidget *table = plugin.actionsTable();
    QCOMPARE(table->item(0, 2)->text(), QString("Yes"));
  }
};

QTEST_MAIN(OptimizationDashboardPluginTest)
#include "optimizationdashboard_plugin_test.moc"
