// WorkflowSyncServicePerformanceTest — Performance tests
//
// Test coverage:
//   - Bulk sync operations performance
//   - History query performance

#include <QTest>
#include <QElapsedTimer>
#include "services/WorkflowSyncService.h"

class WorkflowSyncServicePerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testBulkSyncOperations() {
    WorkflowSyncService svc;
    SyncConfig config;
    config.remoteUrl = "https://sync.example.com";
    svc.configureSync(config);

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < 1000; ++i) {
      svc.startSync(QStringLiteral("workflow_%1").arg(i));
    }

    qint64 elapsed = timer.elapsed();
    QCOMPARE(svc.recordCount(), 1000);
    QVERIFY(elapsed < 500);
  }

  void testHistoryQueryPerformance() {
    WorkflowSyncService svc;
    SyncConfig config;
    config.remoteUrl = "https://sync.example.com";
    svc.configureSync(config);

    for (int i = 0; i < 500; ++i) {
      svc.startSync(QStringLiteral("workflow_%1").arg(i));
    }

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < 100; ++i) {
      auto history = svc.syncHistory();
      Q_UNUSED(history);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 100);
  }
};

QTEST_MAIN(WorkflowSyncServicePerformanceTest)
#include "workflow_sync_service_performance_test.moc"
