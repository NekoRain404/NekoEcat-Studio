// WorkflowOptimizationServiceTest — Tests for Workflow Optimization Service
//
// Test coverage:
//   - Task schedule optimization (ordering, parallelism)
//   - Resource allocation optimization
//   - Bottleneck detection
//   - Critical path analysis
//   - Optimization suggestions generation
//   - Signal emissions

#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowOptimizationService.h"

class WorkflowOptimizationServiceTest : public QObject {
  Q_OBJECT
private:
  QVector<WfTask> makeTasks()
  {
      QVector<WfTask> tasks;
      WfTask t1;
      t1.id = QStringLiteral("t1");
      t1.name = QStringLiteral("Initialize");
      t1.priority = 10;
      t1.estimatedDurationMs = 100.0;
      tasks.append(t1);

      WfTask t2;
      t2.id = QStringLiteral("t2");
      t2.name = QStringLiteral("Configure");
      t2.priority = 8;
      t2.estimatedDurationMs = 200.0;
      t2.dependencies << QStringLiteral("t1");
      t2.requiredResources << QStringLiteral("cpu");
      tasks.append(t2);

      WfTask t3;
      t3.id = QStringLiteral("t3");
      t3.name = QStringLiteral("Validate");
      t3.priority = 6;
      t3.estimatedDurationMs = 150.0;
      t3.dependencies << QStringLiteral("t1");
      t3.requiredResources << QStringLiteral("cpu");
      tasks.append(t3);

      WfTask t4;
      t4.id = QStringLiteral("t4");
      t4.name = QStringLiteral("Deploy");
      t4.priority = 4;
      t4.estimatedDurationMs = 300.0;
      t4.dependencies << QStringLiteral("t2") << QStringLiteral("t3");
      t4.requiredResources << QStringLiteral("network");
      tasks.append(t4);

      return tasks;
  }

  QVector<WfResource> makeResources()
  {
      QVector<WfResource> resources;
      WfResource r1;
      r1.id = QStringLiteral("cpu");
      r1.name = QStringLiteral("CPU");
      r1.capacity = 100.0;
      r1.currentLoad = 20.0;
      r1.capabilities << QStringLiteral("cpu");
      resources.append(r1);

      WfResource r2;
      r2.id = QStringLiteral("network");
      r2.name = QStringLiteral("Network");
      r2.capacity = 50.0;
      r2.currentLoad = 10.0;
      r2.capabilities << QStringLiteral("network");
      resources.append(r2);

      return resources;
  }

private slots:
  // Optimize task schedule and verify ordering, parallelism, duration
  void testOptimizeTaskSchedule() {
      WorkflowOptimizationService svc;
      auto tasks = makeTasks();
      auto schedule = svc.optimizeTaskSchedule(tasks);

      QCOMPARE(schedule.tasks.size(), 4);
      QVERIFY(schedule.order.size() == 4);
      QVERIFY(schedule.estimatedDurationMs > 0.0);
      QVERIFY(schedule.parallelism >= 1.0);

      // t1 must come before t2 and t3
      int idx1 = schedule.order.indexOf(QStringLiteral("t1"));
      int idx2 = schedule.order.indexOf(QStringLiteral("t2"));
      int idx3 = schedule.order.indexOf(QStringLiteral("t3"));
      QVERIFY(idx1 < idx2);
      QVERIFY(idx1 < idx3);
  }

  // Optimize resource allocation and verify utilization
  void testOptimizeResourceAllocation() {
      WorkflowOptimizationService svc;
      auto tasks = makeTasks();
      auto resources = makeResources();
      auto plan = svc.optimizeResourceAllocation(resources, tasks);

      QVERIFY(plan.utilization > 0.0);
      QVERIFY(!plan.allocations.isEmpty());
  }

  // Optimize parallel execution and verify stages and critical path
  void testOptimizeParallelExecution() {
      WorkflowOptimizationService svc;
      auto tasks = makeTasks();
      auto plan = svc.optimizeParallelExecution(tasks);

      QVERIFY(plan.stages.size() >= 2);
      QVERIFY(plan.maxConcurrency >= 1);
      QVERIFY(plan.speedupRatio >= 1.0);
      QVERIFY(!plan.criticalPath.isEmpty());
  }

  // Resolve dependencies and verify topological order
  void testResolveDependencies() {
      WorkflowOptimizationService svc;
      auto tasks = makeTasks();
      auto graph = svc.resolveDependencies(tasks);

      QVERIFY(!graph.hasCycles);
      QCOMPARE(graph.topologicalOrder.size(), 4);
      QVERIFY(graph.cycles.isEmpty());
  }

  // Detect cyclic dependencies in task graph
  void testCyclicDependencyDetection() {
      WorkflowOptimizationService svc;
      QVector<WfTask> tasks;

      WfTask t1;
      t1.id = QStringLiteral("a");
      t1.name = QStringLiteral("A");
      t1.dependencies << QStringLiteral("b");
      tasks.append(t1);

      WfTask t2;
      t2.id = QStringLiteral("b");
      t2.name = QStringLiteral("B");
      t2.dependencies << QStringLiteral("a");
      tasks.append(t2);

      auto graph = svc.resolveDependencies(tasks);
      QVERIFY(graph.hasCycles);
      QVERIFY(graph.topologicalOrder.size() < tasks.size());
  }

  // Handle empty task list gracefully
  void testEmptyTaskList() {
      WorkflowOptimizationService svc;
      QVector<WfTask> empty;
      auto schedule = svc.optimizeTaskSchedule(empty);

      QVERIFY(schedule.tasks.isEmpty());
      QVERIFY(schedule.order.isEmpty());
      QCOMPARE(schedule.estimatedDurationMs, 0.0);
      QCOMPARE(schedule.parallelism, 1.0);
  }

  // Verify optimizationCompleted signal emission
  void testSignalEmission() {
      WorkflowOptimizationService svc;
      QSignalSpy spy(&svc, &WorkflowOptimizationService::optimizationCompleted);

      auto tasks = makeTasks();
      svc.optimizeTaskSchedule(tasks);
      QCOMPARE(spy.count(), 1);

      svc.resolveDependencies(tasks);
      QCOMPARE(spy.count(), 2);
  }

  // Detect bottleneck nodes in task graph
  void testBottleneckDetection() {
      WorkflowOptimizationService svc;
      QVector<WfTask> tasks;

      WfTask root;
      root.id = QStringLiteral("root");
      root.name = QStringLiteral("Root");
      tasks.append(root);

      for (int i = 0; i < 3; ++i) {
          WfTask t;
          t.id = QStringLiteral("child%1").arg(i);
          t.name = QStringLiteral("Child %1").arg(i);
          t.dependencies << QStringLiteral("root");
          tasks.append(t);
      }

      auto schedule = svc.optimizeTaskSchedule(tasks);
      QVERIFY(schedule.bottlenecks.contains(QStringLiteral("root")));
  }

  // Verify execution plan stage grouping by dependency level
  void testExecutionPlanStages() {
      WorkflowOptimizationService svc;
      auto tasks = makeTasks();
      auto plan = svc.optimizeParallelExecution(tasks);

      // Stage 0: t1 (no deps), Stage 1: t2+t3 (depend on t1), Stage 2: t4
      QVERIFY(plan.stages.size() == 3);
      QCOMPARE(plan.stages[0].size(), 1);
      QCOMPARE(plan.stages[1].size(), 2);
      QCOMPARE(plan.stages[2].size(), 1);
  }
};

QTEST_MAIN(WorkflowOptimizationServiceTest)
#include "workflow_optimization_service_test.moc"
