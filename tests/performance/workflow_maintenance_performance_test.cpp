#include <QTest>
#include <QSignalSpy>
#include <QElapsedTimer>
#include "services/WorkflowMaintenanceService.h"

class WorkflowMaintenancePerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testScheduleMaintenanceThroughput() {
    WorkflowMaintenanceService svc;
    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      WfMaintenanceTask task;
      task.type = static_cast<WfMaintenanceType>(i % 4);
      task.description = QStringLiteral("Task %1").arg(i);
      task.schedule = QStringLiteral("*/%1 * * * *").arg((i % 59) + 1);
      task.priority = i % 10;
      task.estimatedDurationMin = (i % 120) + 1;
      svc.scheduleMaintenance(task);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Schedule maintenance throughput:" << count << "in" << elapsed << "ms";
  }

  void testExecuteMaintenanceThroughput() {
    WorkflowMaintenanceService svc;
    for (int i = 0; i < 500; i++) {
      WfMaintenanceTask task;
      task.description = QStringLiteral("Task %1").arg(i);
      task.type = static_cast<WfMaintenanceType>(i % 4);
      svc.scheduleMaintenance(task);
    }

    QElapsedTimer timer;
    timer.start();

    const int count = 500;
    for (int i = 1; i <= count; i++) {
      svc.executeMaintenance(i);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Execute maintenance throughput:" << count << "in" << elapsed << "ms";
  }

  void testHistoryQueryThroughput() {
    WorkflowMaintenanceService svc;
    for (int i = 0; i < 500; i++) {
      WfMaintenanceTask task;
      task.description = QStringLiteral("Task %1").arg(i);
      task.type = static_cast<WfMaintenanceType>(i % 4);
      svc.scheduleMaintenance(task);
      svc.executeMaintenance(i + 1);
    }

    QElapsedTimer timer;
    timer.start();

    const int iterations = 10000;
    for (int i = 0; i < iterations; i++) {
      svc.maintenanceHistory();
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "History query throughput:" << iterations << "calls in" << elapsed << "ms";
  }

  void testScheduleQueryThroughput() {
    WorkflowMaintenanceService svc;
    for (int i = 0; i < 500; i++) {
      WfMaintenanceTask task;
      task.description = QStringLiteral("Task %1").arg(i);
      task.type = static_cast<WfMaintenanceType>(i % 4);
      svc.scheduleMaintenance(task);
    }

    QElapsedTimer timer;
    timer.start();

    const int iterations = 10000;
    for (int i = 0; i < iterations; i++) {
      svc.maintenanceSchedule();
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "Schedule query throughput:" << iterations << "calls in" << elapsed << "ms";
  }

  void testSignalThroughput() {
    WorkflowMaintenanceService svc;
    QSignalSpy scheduledSpy(&svc, &WorkflowMaintenanceService::maintenanceScheduled);
    QSignalSpy completedSpy(&svc, &WorkflowMaintenanceService::maintenanceCompleted);

    QElapsedTimer timer;
    timer.start();

    const int count = 500;
    for (int i = 0; i < count; i++) {
      WfMaintenanceTask task;
      task.description = QStringLiteral("Signal task %1").arg(i);
      task.type = static_cast<WfMaintenanceType>(i % 4);
      svc.scheduleMaintenance(task);
      svc.executeMaintenance(i + 1);
    }

    qint64 elapsed = timer.elapsed();
    QCOMPARE(scheduledSpy.count(), count);
    QCOMPARE(completedSpy.count(), count);
    QVERIFY(elapsed < 5000);
    qDebug() << "Signal throughput:" << count * 2 << "signals in" << elapsed << "ms";
  }

  void testMemoryStability() {
    WorkflowMaintenanceService svc;

    for (int round = 0; round < 10; round++) {
      for (int i = 0; i < 50; i++) {
        WfMaintenanceTask task;
        task.description = QStringLiteral("Stability %1_%2").arg(round).arg(i);
        task.type = static_cast<WfMaintenanceType>(i % 4);
        task.priority = i % 10;
        svc.scheduleMaintenance(task);
        svc.executeMaintenance(round * 50 + i + 1);
      }
    }

    QCOMPARE(svc.maintenanceHistory().size(), 500);
    qDebug() << "Memory stability: 500 schedule+execute across 10 rounds";
  }
};

QTEST_MAIN(WorkflowMaintenancePerformanceTest)
#include "workflow_maintenance_performance_test.moc"
