#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTextEdit>
#include <QLabel>
#include "plugins/optimizationdashboard/OptimizationDashboardPlugin.h"
#include "services/WorkflowOptimizationService.h"
#include "services/WorkflowMonitoringService.h"

class WorkflowDashboardPluginIntegrationTest : public QObject {
  Q_OBJECT
private slots:
  void testDashboardWithOptimizationService() {
    OptimizationDashboardPlugin dashboard;
    WorkflowOptimizationService optimizer;

    dashboard.widget();

    QVector<WfTask> tasks;
    WfTask t1;
    t1.id = QStringLiteral("t1");
    t1.name = QStringLiteral("Init");
    t1.estimatedDurationMs = 100.0;
    tasks.append(t1);

    WfTask t2;
    t2.id = QStringLiteral("t2");
    t2.name = QStringLiteral("Process");
    t2.estimatedDurationMs = 200.0;
    t2.dependencies << QStringLiteral("t1");
    tasks.append(t2);

    auto schedule = optimizer.optimizeTaskSchedule(tasks);

    OptimizationDashboardPlugin::OptimizationMetric m;
    m.name = "Estimated Duration";
    m.value = schedule.estimatedDurationMs;
    m.target = 250.0;
    m.improvement = 0.0;
    dashboard.addMetric(m);

    QCOMPARE(dashboard.metricCount(), 1);
    QCOMPARE(dashboard.metrics()[0].name, QString("Estimated Duration"));

    QTableWidget *table = dashboard.metricsTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 1);
  }

  void testDashboardWithMonitoringService() {
    OptimizationDashboardPlugin dashboard;
    WorkflowMonitoringService monitor;

    dashboard.widget();

    ExecutionStatus exec;
    exec.workflowId = QStringLiteral("wf1");
    exec.state = ExecutionState::Running;
    exec.startTime = QDateTime::currentDateTime().addSecs(-10);
    exec.endTime = QDateTime::currentDateTime();
    exec.currentStep = 3;
    exec.totalSteps = 5;
    monitor.recordExecution(exec);

    auto perf = monitor.monitorPerformance(QStringLiteral("wf1"));

    OptimizationDashboardPlugin::OptimizationMetric m;
    m.name = "Throughput";
    m.value = perf.throughput;
    m.target = 1.0;
    m.improvement = 0.0;
    dashboard.addMetric(m);

    QCOMPARE(dashboard.metricCount(), 1);

    dashboard.updateMetric(0, 2.5);
    QCOMPARE(dashboard.metrics()[0].value, 2.5);
  }

  void testDashboardRecommendationsFromOptimization() {
    OptimizationDashboardPlugin dashboard;
    WorkflowOptimizationService optimizer;

    dashboard.widget();

    QVector<WfResource> resources;
    WfResource r;
    r.id = QStringLiteral("cpu");
    r.name = QStringLiteral("CPU");
    r.capacity = 100.0;
    r.currentLoad = 80.0;
    r.capabilities << QStringLiteral("cpu");
    resources.append(r);

    QVector<WfTask> tasks;
    WfTask t;
    t.id = QStringLiteral("t1");
    t.name = QStringLiteral("Heavy Task");
    t.requiredResources << QStringLiteral("cpu");
    tasks.append(t);

    auto plan = optimizer.optimizeResourceAllocation(resources, tasks);

    for (const auto &rec : plan.recommendations) {
      OptimizationDashboardPlugin::OptimizationRecommendation r;
      r.title = rec;
      r.description = QStringLiteral("Auto-generated recommendation");
      r.priority = QStringLiteral("Medium");
      r.category = QStringLiteral("Resource");
      dashboard.addRecommendation(r);
    }

    QVERIFY(dashboard.recommendationCount() >= 0);
  }

  void testDashboardActionsExecution() {
    OptimizationDashboardPlugin dashboard;
    dashboard.widget();

    OptimizationDashboardPlugin::OptimizationAction a;
    a.name = "Optimize Schedule";
    a.description = "Run schedule optimizer";
    a.executed = false;
    a.result = "";
    dashboard.addAction(a);

    QSignalSpy spy(&dashboard, &OptimizationDashboardPlugin::actionExecuted);
    dashboard.executeAction(0);

    QCOMPARE(dashboard.actions()[0].executed, true);
    QCOMPARE(dashboard.actions()[0].result, QString("Executed successfully"));
    QCOMPARE(spy.count(), 1);
  }

  void testDashboardHistoryTracking() {
    OptimizationDashboardPlugin dashboard;
    dashboard.widget();

    for (int i = 0; i < 10; i++) {
      OptimizationDashboardPlugin::OptimizationHistoryEntry e;
      e.timestamp = QDateTime::currentDateTime().addSecs(-i * 60);
      e.action = QStringLiteral("Action %1").arg(i);
      e.result = QStringLiteral("Result %1").arg(i);
      e.improvement = i * 2.5;
      dashboard.addHistoryEntry(e);
    }

    QCOMPARE(dashboard.historyCount(), 10);

    QTableWidget *table = dashboard.historyTable();
    QCOMPARE(table->rowCount(), 10);
  }

  void testDashboardReportGeneration() {
    OptimizationDashboardPlugin dashboard;
    dashboard.widget();

    OptimizationDashboardPlugin::OptimizationMetric m1;
    m1.name = "Cycle Time";
    m1.value = 850.0;
    m1.target = 900.0;
    m1.improvement = -5.6;
    dashboard.addMetric(m1);

    OptimizationDashboardPlugin::OptimizationMetric m2;
    m2.name = "Bandwidth";
    m2.value = 75.0;
    m2.target = 80.0;
    m2.improvement = -6.25;
    dashboard.addMetric(m2);

    QString report = dashboard.exportReport();
    QVERIFY(report.contains("Optimization Report"));
    QVERIFY(report.contains("Cycle Time"));
    QVERIFY(report.contains("Bandwidth"));

    QTextEdit *view = dashboard.reportView();
    QVERIFY(view != nullptr);
    QVERIFY(view->isReadOnly());
  }

  void testDashboardRefresh() {
    OptimizationDashboardPlugin dashboard;
    dashboard.widget();

    QSignalSpy spy(&dashboard, &OptimizationDashboardPlugin::metricUpdated);

    OptimizationDashboardPlugin::OptimizationMetric m;
    m.name = "Test";
    m.value = 100.0;
    m.target = 90.0;
    m.improvement = 0.0;
    dashboard.addMetric(m);

    dashboard.refresh();
    QLabel *label = dashboard.statusLabel();
    QVERIFY(label->text().contains("Last refreshed"));
  }

  void testDashboardTablesWidgetCreation() {
    OptimizationDashboardPlugin dashboard;
    dashboard.widget();

    QVERIFY(dashboard.metricsTable() != nullptr);
    QVERIFY(dashboard.historyTable() != nullptr);
    QVERIFY(dashboard.recommendationsTable() != nullptr);
    QVERIFY(dashboard.actionsTable() != nullptr);
    QVERIFY(dashboard.reportView() != nullptr);
    QVERIFY(dashboard.statusLabel() != nullptr);

    QCOMPARE(dashboard.metricsTable()->columnCount(), 4);
    QCOMPARE(dashboard.historyTable()->columnCount(), 4);
    QCOMPARE(dashboard.recommendationsTable()->columnCount(), 4);
    QCOMPARE(dashboard.actionsTable()->columnCount(), 4);
  }

  void testDashboardActivationDeactivation() {
    OptimizationDashboardPlugin dashboard;
    dashboard.widget();

    dashboard.activate();
    dashboard.deactivate();
  }

  void testDashboardWithDependencyGraph() {
    OptimizationDashboardPlugin dashboard;
    WorkflowOptimizationService optimizer;

    dashboard.widget();

    QVector<WfTask> tasks;
    for (int i = 0; i < 5; i++) {
      WfTask t;
      t.id = QStringLiteral("t%1").arg(i);
      t.name = QStringLiteral("Task %1").arg(i);
      if (i > 0) {
        t.dependencies << QStringLiteral("t%1").arg(i - 1);
      }
      tasks.append(t);
    }

    auto graph = optimizer.resolveDependencies(tasks);

    OptimizationDashboardPlugin::OptimizationMetric m;
    m.name = "Dependency Depth";
    m.value = graph.topologicalOrder.size();
    m.target = 5.0;
    m.improvement = 0.0;
    dashboard.addMetric(m);

    QCOMPARE(dashboard.metricCount(), 1);
    QCOMPARE(dashboard.metrics()[0].value, 5.0);
  }
};

QTEST_MAIN(WorkflowDashboardPluginIntegrationTest)
#include "workflow_dashboard_plugin_integration_test.moc"
