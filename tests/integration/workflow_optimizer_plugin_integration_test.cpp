#include <QTest>
#include <QSignalSpy>
#include <QTemporaryFile>
#include "plugins/workflowdesigner/WorkflowDesignerPlugin.h"
#include "services/WorkflowOptimizationService.h"
#include "services/WorkflowSchedulingService.h"

class WorkflowOptimizerPluginIntegrationTest : public QObject {
  Q_OBJECT
private slots:
  void testDesignerToOptimizationPipeline() {
    WorkflowDesignerPlugin designer;
    WorkflowOptimizationService optimizer;

    designer.addNode(QStringLiteral("action"), QStringLiteral("Init"));
    designer.addNode(QStringLiteral("action"), QStringLiteral("Configure"));
    designer.addNode(QStringLiteral("action"), QStringLiteral("Validate"));
    QCOMPARE(designer.nodeCount(), 3);

    designer.addConnection(QStringLiteral("node_1"), QStringLiteral("node_2"));
    designer.addConnection(QStringLiteral("node_2"), QStringLiteral("node_3"));
    QCOMPARE(designer.connectionCount(), 2);

    QVector<WfTask> tasks;
    WfTask t1;
    t1.id = QStringLiteral("node_1");
    t1.name = QStringLiteral("Init");
    tasks.append(t1);

    WfTask t2;
    t2.id = QStringLiteral("node_2");
    t2.name = QStringLiteral("Configure");
    t2.dependencies << QStringLiteral("node_1");
    tasks.append(t2);

    WfTask t3;
    t3.id = QStringLiteral("node_3");
    t3.name = QStringLiteral("Validate");
    t3.dependencies << QStringLiteral("node_2");
    tasks.append(t3);

    auto schedule = optimizer.optimizeTaskSchedule(tasks);
    QCOMPARE(schedule.tasks.size(), 3);
    QVERIFY(schedule.order.indexOf(QStringLiteral("node_1")) < schedule.order.indexOf(QStringLiteral("node_2")));
    QVERIFY(schedule.order.indexOf(QStringLiteral("node_2")) < schedule.order.indexOf(QStringLiteral("node_3")));
  }

  void testDesignerToSchedulingPipeline() {
    WorkflowDesignerPlugin designer;
    WorkflowSchedulingService scheduler;

    designer.addNode(QStringLiteral("action"), QStringLiteral("Deploy"));
    QCOMPARE(designer.nodeCount(), 1);

    WorkflowConfig cfg;
    cfg.workflowId = QStringLiteral("deploy_wf");
    cfg.name = QStringLiteral("Deploy Workflow");
    cfg.scheduleType = ScheduleType::Priority;
    cfg.priority = 10;
    cfg.steps.append(QJsonObject{{"node", "node_1"}});

    QVERIFY(scheduler.scheduleWorkflow(cfg));
    QCOMPARE(scheduler.workflowCount(), 1);

    QSignalSpy spy(&scheduler, &WorkflowSchedulingService::workflowTriggered);
    scheduler.triggerWorkflow(QStringLiteral("deploy_wf"));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(scheduler.runs(QStringLiteral("deploy_wf")).last().status,
             WorkflowStatus::Running);
  }

  void testOptimizationDependencyResolution() {
    WorkflowOptimizationService optimizer;

    QVector<WfTask> tasks;
    for (int i = 0; i < 10; i++) {
      WfTask t;
      t.id = QStringLiteral("n%1").arg(i);
      t.name = QStringLiteral("Node %1").arg(i);
      if (i > 0) {
        t.dependencies << QStringLiteral("n%1").arg(i - 1);
      }
      tasks.append(t);
    }

    auto graph = optimizer.resolveDependencies(tasks);
    QVERIFY(!graph.hasCycles);
    QCOMPARE(graph.topologicalOrder.size(), 10);
  }

  void testDesignerNodeOperations() {
    WorkflowDesignerPlugin designer;
    QSignalSpy addSpy(&designer, &WorkflowDesignerPlugin::nodeAdded);
    QSignalSpy removeSpy(&designer, &WorkflowDesignerPlugin::nodeRemoved);

    designer.addNode(QStringLiteral("action"), QStringLiteral("Test"));
    QCOMPARE(addSpy.count(), 1);
    QCOMPARE(designer.nodeCount(), 1);

    designer.removeNode(QStringLiteral("node_1"));
    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(designer.nodeCount(), 0);
  }

  void testDesignerConnectionOperations() {
    WorkflowDesignerPlugin designer;
    QSignalSpy addSpy(&designer, &WorkflowDesignerPlugin::connectionAdded);
    QSignalSpy removeSpy(&designer, &WorkflowDesignerPlugin::connectionRemoved);

    designer.addNode(QStringLiteral("action"), QStringLiteral("A"));
    designer.addNode(QStringLiteral("action"), QStringLiteral("B"));

    designer.addConnection(QStringLiteral("node_1"), QStringLiteral("node_2"), QStringLiteral("flow"));
    QCOMPARE(addSpy.count(), 1);
    QCOMPARE(designer.connectionCount(), 1);

    designer.removeConnection(0);
    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(designer.connectionCount(), 0);
  }

  void testDesignerExecutionStatus() {
    WorkflowDesignerPlugin designer;
    QSignalSpy spy(&designer, &WorkflowDesignerPlugin::executionStatusChanged);

    designer.setExecutionStatus(QStringLiteral("Running"));
    QCOMPARE(designer.executionStatus(), QString("Running"));
    QCOMPARE(spy.count(), 1);
  }

  void testDesignerExportImport() {
    WorkflowDesignerPlugin designer;
    designer.addNode(QStringLiteral("action"), QStringLiteral("Test"));

    QTemporaryFile tmpFile;
    tmpFile.setAutoRemove(true);
    QVERIFY(tmpFile.open());
    QString path = tmpFile.fileName();
    tmpFile.close();

    QVERIFY(designer.exportWorkflow(path));

    WorkflowDesignerPlugin importer;
    QVERIFY(importer.importWorkflow(path));
  }

  void testOptimizationWithDesignerNodes() {
    WorkflowDesignerPlugin designer;
    WorkflowOptimizationService optimizer;

    for (int i = 0; i < 5; i++) {
      designer.addNode(QStringLiteral("action"), QStringLiteral("Step %1").arg(i));
    }
    QCOMPARE(designer.nodeCount(), 5);

    QVector<WfTask> tasks;
    for (int i = 0; i < 5; i++) {
      WfTask t;
      t.id = QStringLiteral("node_%1").arg(i + 1);
      t.name = QStringLiteral("Step %1").arg(i);
      t.priority = 5 - i;
      if (i > 0) {
        t.dependencies << QStringLiteral("node_%1").arg(i);
      }
      tasks.append(t);
    }

    auto plan = optimizer.optimizeParallelExecution(tasks);
    QVERIFY(plan.stages.size() >= 1);
    QVERIFY(plan.maxConcurrency >= 1);
  }

  void testFullWorkflowLifecycle() {
    WorkflowDesignerPlugin designer;
    WorkflowOptimizationService optimizer;
    WorkflowSchedulingService scheduler;

    designer.addNode(QStringLiteral("action"), QStringLiteral("Prepare"));
    designer.addNode(QStringLiteral("action"), QStringLiteral("Execute"));
    designer.addNode(QStringLiteral("action"), QStringLiteral("Verify"));

    QVector<WfTask> tasks;
    for (int i = 0; i < 3; i++) {
      WfTask t;
      t.id = QStringLiteral("node_%1").arg(i + 1);
      t.name = designer.nodeCount() > 0 ? QStringLiteral("Step %1").arg(i) : QString();
      tasks.append(t);
    }

    auto schedule = optimizer.optimizeTaskSchedule(tasks);
    QCOMPARE(schedule.tasks.size(), 3);

    WorkflowConfig cfg;
    cfg.workflowId = QStringLiteral("full_lifecycle");
    cfg.name = QStringLiteral("Full Lifecycle");
    cfg.steps.append(QJsonObject{{"action", "prepare"}});
    cfg.steps.append(QJsonObject{{"action", "execute"}});
    cfg.steps.append(QJsonObject{{"action", "verify"}});
    QVERIFY(scheduler.scheduleWorkflow(cfg));

    QSignalSpy spy(&scheduler, &WorkflowSchedulingService::workflowTriggered);
    scheduler.triggerWorkflow(QStringLiteral("full_lifecycle"));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(scheduler.runs(QStringLiteral("full_lifecycle")).last().status,
             WorkflowStatus::Running);

    designer.clearNodes();
    QCOMPARE(designer.nodeCount(), 0);
  }
};

QTEST_MAIN(WorkflowOptimizerPluginIntegrationTest)
#include "workflow_optimizer_plugin_integration_test.moc"
