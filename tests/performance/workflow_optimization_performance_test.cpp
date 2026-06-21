#include <QTest>
#include <QSignalSpy>
#include <QElapsedTimer>
#include "services/WorkflowOptimizationService.h"

class WorkflowOptimizationPerformanceTest : public QObject {
  Q_OBJECT
private:
  QVector<WfTask> makeLargeTaskSet(int count) {
    QVector<WfTask> tasks;
    for (int i = 0; i < count; i++) {
      WfTask t;
      t.id = QStringLiteral("t%1").arg(i);
      t.name = QStringLiteral("Task %1").arg(i);
      t.priority = i % 10;
      t.estimatedDurationMs = 50.0 + (i % 200);
      if (i > 0) {
        t.dependencies << QStringLiteral("t%1").arg(i - 1);
      }
      if (i % 3 == 0) {
        t.requiredResources << QStringLiteral("cpu");
      } else {
        t.requiredResources << QStringLiteral("network");
      }
      tasks.append(t);
    }
    return tasks;
  }

  QVector<WfResource> makeLargeResourceSet(int count) {
    QVector<WfResource> resources;
    for (int i = 0; i < count; i++) {
      WfResource r;
      r.id = QStringLiteral("r%1").arg(i);
      r.name = QStringLiteral("Resource %1").arg(i);
      r.capacity = 100.0;
      r.currentLoad = 10.0 + (i % 50);
      if (i % 2 == 0) {
        r.capabilities << QStringLiteral("cpu");
      } else {
        r.capabilities << QStringLiteral("network");
      }
      resources.append(r);
    }
    return resources;
  }

private slots:
  void testScheduleOptimization100Tasks() {
    WorkflowOptimizationService svc;
    auto tasks = makeLargeTaskSet(100);

    QElapsedTimer timer;
    timer.start();
    auto schedule = svc.optimizeTaskSchedule(tasks);
    qint64 elapsed = timer.elapsed();

    QCOMPARE(schedule.tasks.size(), 100);
    QVERIFY(elapsed < 2000);
    qDebug() << "Schedule optimization (100 tasks):" << elapsed << "ms";
  }

  void testScheduleOptimization500Tasks() {
    WorkflowOptimizationService svc;
    auto tasks = makeLargeTaskSet(500);

    QElapsedTimer timer;
    timer.start();
    auto schedule = svc.optimizeTaskSchedule(tasks);
    qint64 elapsed = timer.elapsed();

    QCOMPARE(schedule.tasks.size(), 500);
    QVERIFY(elapsed < 5000);
    qDebug() << "Schedule optimization (500 tasks):" << elapsed << "ms";
  }

  void testResourceAllocationPerformance() {
    WorkflowOptimizationService svc;
    auto tasks = makeLargeTaskSet(200);
    auto resources = makeLargeResourceSet(50);

    QElapsedTimer timer;
    timer.start();
    auto plan = svc.optimizeResourceAllocation(resources, tasks);
    qint64 elapsed = timer.elapsed();

    QVERIFY(plan.utilization > 0.0);
    QVERIFY(elapsed < 3000);
    qDebug() << "Resource allocation (200 tasks, 50 resources):" << elapsed << "ms";
  }

  void testParallelExecutionOptimization() {
    WorkflowOptimizationService svc;
    auto tasks = makeLargeTaskSet(300);

    QElapsedTimer timer;
    timer.start();
    auto plan = svc.optimizeParallelExecution(tasks);
    qint64 elapsed = timer.elapsed();

    QVERIFY(plan.stages.size() >= 1);
    QVERIFY(plan.maxConcurrency >= 1);
    QVERIFY(elapsed < 3000);
    qDebug() << "Parallel execution optimization (300 tasks):" << elapsed << "ms";
  }

  void testDependencyResolutionPerformance() {
    WorkflowOptimizationService svc;
    auto tasks = makeLargeTaskSet(400);

    QElapsedTimer timer;
    timer.start();
    auto graph = svc.resolveDependencies(tasks);
    qint64 elapsed = timer.elapsed();

    QVERIFY(!graph.hasCycles);
    QCOMPARE(graph.topologicalOrder.size(), 400);
    QVERIFY(elapsed < 3000);
    qDebug() << "Dependency resolution (400 tasks):" << elapsed << "ms";
  }

  void testCyclicDependencyDetectionPerformance() {
    WorkflowOptimizationService svc;
    QVector<WfTask> tasks;

    for (int i = 0; i < 100; i++) {
      WfTask t;
      t.id = QStringLiteral("c%1").arg(i);
      t.name = QStringLiteral("Cyclic %1").arg(i);
      t.dependencies << QStringLiteral("c%1").arg((i + 1) % 100);
      tasks.append(t);
    }

    QElapsedTimer timer;
    timer.start();
    auto graph = svc.resolveDependencies(tasks);
    qint64 elapsed = timer.elapsed();

    QVERIFY(graph.hasCycles);
    QVERIFY(elapsed < 2000);
    qDebug() << "Cyclic detection (100 tasks):" << elapsed << "ms";
  }

  void testRepeatedOptimizationStability() {
    WorkflowOptimizationService svc;
    auto tasks = makeLargeTaskSet(50);

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < 100; i++) {
      svc.optimizeTaskSchedule(tasks);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 10000);
    qDebug() << "Repeated optimization (100 rounds, 50 tasks):" << elapsed << "ms";
  }

  void testSignalThroughput() {
    WorkflowOptimizationService svc;
    QSignalSpy spy(&svc, &WorkflowOptimizationService::optimizationCompleted);

    auto tasks = makeLargeTaskSet(20);

    QElapsedTimer timer;
    timer.start();

    const int count = 500;
    for (int i = 0; i < count; i++) {
      svc.optimizeTaskSchedule(tasks);
    }

    qint64 elapsed = timer.elapsed();
    QCOMPARE(spy.count(), count);
    QVERIFY(elapsed < 10000);
    qDebug() << "Signal throughput:" << count << "optimizations in" << elapsed << "ms";
  }
};

QTEST_MAIN(WorkflowOptimizationPerformanceTest)
#include "workflow_optimization_performance_test.moc"
