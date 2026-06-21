#include <QTest>
#include <QSignalSpy>
#include <QElapsedTimer>
#include "services/WorkflowMonitoringService.h"

class WorkflowMonitoringPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testExecutionRecordingThroughput() {
    WorkflowMonitoringService svc;
    QElapsedTimer timer;
    timer.start();

    const int count = 5000;
    for (int i = 0; i < count; i++) {
      ExecutionStatus s;
      s.workflowId = QStringLiteral("wf_%1").arg(i % 100);
      s.state = ExecutionState::Running;
      s.startTime = QDateTime::currentDateTime().addSecs(-10);
      s.endTime = QDateTime::currentDateTime();
      s.currentStep = i % 10;
      s.totalSteps = 10;
      svc.recordExecution(s);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Execution recording throughput:" << count << "records in" << elapsed << "ms";
  }

  void testErrorRecordingThroughput() {
    WorkflowMonitoringService svc;
    QElapsedTimer timer;
    timer.start();

    const int count = 5000;
    for (int i = 0; i < count; i++) {
      WfErrorInfo err;
      err.workflowId = QStringLiteral("wf_%1").arg(i % 100);
      err.stepId = QStringLiteral("step_%1").arg(i % 5);
      err.message = QStringLiteral("Error %1").arg(i);
      err.severity = (i % 10 == 0) ? QStringLiteral("critical") : QStringLiteral("warning");
      err.timestamp = QDateTime::currentDateTime();
      svc.recordError(err);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Error recording throughput:" << count << "records in" << elapsed << "ms";
  }

  void testMonitorQueryLatency() {
    WorkflowMonitoringService svc;
    for (int i = 0; i < 100; i++) {
      ExecutionStatus s;
      s.workflowId = QStringLiteral("wf_%1").arg(i);
      s.state = ExecutionState::Running;
      s.startTime = QDateTime::currentDateTime().addSecs(-5);
      s.endTime = QDateTime::currentDateTime();
      s.currentStep = 5;
      s.totalSteps = 10;
      svc.recordExecution(s);
    }

    QElapsedTimer timer;
    timer.start();

    const int iterations = 10000;
    for (int i = 0; i < iterations; i++) {
      svc.monitorExecution(QStringLiteral("wf_%1").arg(i % 100));
      svc.monitorPerformance(QStringLiteral("wf_%1").arg(i % 100));
      svc.monitorErrors(QStringLiteral("wf_%1").arg(i % 100));
      svc.monitorResources(QStringLiteral("wf_%1").arg(i % 100));
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Monitor query latency:" << iterations * 4 << "queries in" << elapsed << "ms";
  }

  void testSignalThroughput() {
    WorkflowMonitoringService svc;
    QSignalSpy execSpy(&svc, &WorkflowMonitoringService::executionUpdated);
    QSignalSpy perfSpy(&svc, &WorkflowMonitoringService::performanceUpdated);

    QElapsedTimer timer;
    timer.start();

    const int count = 2000;
    for (int i = 0; i < count; i++) {
      ExecutionStatus s;
      s.workflowId = QStringLiteral("wf_%1").arg(i % 10);
      s.state = ExecutionState::Running;
      s.startTime = QDateTime::currentDateTime().addSecs(-1);
      s.endTime = QDateTime::currentDateTime();
      s.currentStep = 1;
      s.totalSteps = 5;
      svc.recordExecution(s);
    }

    qint64 elapsed = timer.elapsed();
    QCOMPARE(execSpy.count(), count);
    QCOMPARE(perfSpy.count(), count);
    QVERIFY(elapsed < 5000);
    qDebug() << "Signal throughput:" << count * 2 << "signals in" << elapsed << "ms";
  }

  void testClearHistoryThroughput() {
    WorkflowMonitoringService svc;

    for (int i = 0; i < 500; i++) {
      ExecutionStatus s;
      s.workflowId = QStringLiteral("wf_%1").arg(i);
      s.state = ExecutionState::Completed;
      s.startTime = QDateTime::currentDateTime().addSecs(-1);
      s.endTime = QDateTime::currentDateTime();
      s.currentStep = 5;
      s.totalSteps = 5;
      svc.recordExecution(s);

      WfErrorInfo err;
      err.workflowId = QStringLiteral("wf_%1").arg(i);
      err.message = QStringLiteral("test error");
      svc.recordError(err);
    }

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < 500; i++) {
      svc.clearHistory(QStringLiteral("wf_%1").arg(i));
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "Clear history throughput: 500 clears in" << elapsed << "ms";
  }

  void testMemoryStability() {
    WorkflowMonitoringService svc;

    for (int round = 0; round < 10; round++) {
      for (int i = 0; i < 50; i++) {
        ExecutionStatus s;
        s.workflowId = QStringLiteral("wf_%1_%2").arg(round).arg(i);
        s.state = ExecutionState::Running;
        s.startTime = QDateTime::currentDateTime().addSecs(-1);
        s.endTime = QDateTime::currentDateTime();
        s.currentStep = 3;
        s.totalSteps = 10;
        svc.recordExecution(s);

        WfErrorInfo err;
        err.workflowId = s.workflowId;
        err.message = QStringLiteral("error");
        svc.recordError(err);
      }
    }

    for (int i = 0; i < 50; i++) {
      svc.monitorExecution(QStringLiteral("wf_0_%1").arg(i));
    }

    qDebug() << "Memory stability: 500 executions + 500 errors across 500 workflows";
  }
};

QTEST_MAIN(WorkflowMonitoringPerformanceTest)
#include "workflow_monitoring_performance_test.moc"
