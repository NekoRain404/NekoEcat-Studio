// WorkflowReplicationServicePerformanceTest — Performance tests
//
// Test coverage:
//   - Bulk target management performance
//   - Bulk replication job performance
//   - Query performance with many jobs

#include <QTest>
#include <QElapsedTimer>
#include "services/WorkflowReplicationService.h"

class WorkflowReplicationServicePerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testBulkTargetManagement() {
    WorkflowReplicationService svc;
    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < 500; ++i) {
      ReplicationTarget target;
      target.targetId = QStringLiteral("target_%1").arg(i);
      target.name = QStringLiteral("Target %1").arg(i);
      target.endpoint = QStringLiteral("https://target%1.example.com").arg(i);
      svc.addTarget(target);
    }

    qint64 elapsed = timer.elapsed();
    QCOMPARE(svc.targetCount(), 500);
    QVERIFY(elapsed < 100);
  }

  void testBulkReplicationJobs() {
    WorkflowReplicationService svc;

    ReplicationTarget target;
    target.targetId = "target-1";
    target.name = "Production";
    target.endpoint = "https://prod.example.com";
    svc.addTarget(target);

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < 200; ++i) {
      svc.startReplication(QStringLiteral("source_%1").arg(i), "target-1",
                           ReplicationMode::Full);
    }

    qint64 elapsed = timer.elapsed();
    QCOMPARE(svc.jobCount(), 200);
    QVERIFY(elapsed < 200);
  }

  void testQueryPerformance() {
    WorkflowReplicationService svc;

    ReplicationTarget target;
    target.targetId = "target-1";
    target.name = "Production";
    target.endpoint = "https://prod.example.com";
    svc.addTarget(target);

    for (int i = 0; i < 100; ++i) {
      svc.startReplication(QStringLiteral("source_%1").arg(i), "target-1",
                           ReplicationMode::Full);
    }

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < 1000; ++i) {
      auto jobs = svc.jobHistory();
      Q_UNUSED(jobs);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 100);
  }
};

QTEST_MAIN(WorkflowReplicationServicePerformanceTest)
#include "workflow_replication_service_performance_test.moc"
