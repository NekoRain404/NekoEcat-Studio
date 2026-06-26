#include <QTest>
#include <QElapsedTimer>
#include "services/WorkflowEdgeService.h"

class WorkflowEdgeServicePerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testProcessThroughput() {
    WorkflowEdgeService svc;
    WfEdgeData data;
    data.data = QByteArray(1024, 'x');
    data.source = "perf-test";
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; ++i) {
      QVERIFY(!svc.processAtEdge(data).success);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
  }

  void testAnalysisThroughput() {
    WorkflowEdgeService svc;
    WfEdgeData data;
    data.data = QByteArray(1024, 'a');
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; ++i) {
      QVERIFY(!svc.analyzeAtEdge(data).success);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 10000);
  }

  void testStoreThroughput() {
    WorkflowEdgeService svc;
    WfEdgeData data;
    data.data = QByteArray(512, 'y');
    data.source = "store-test";
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; ++i) {
      QVERIFY(!svc.storeAtEdge(data));
    }
    QCOMPARE(svc.storedCount(), 0);
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
  }

  void testMemoryStability() {
    WorkflowEdgeService svc;
    WfEdgeData data;
    data.data = QByteArray("test");
    for (int i = 0; i < 10000; ++i) {
      QVERIFY(!svc.processAtEdge(data).success);
      QVERIFY(!svc.storeAtEdge(data));
    }
    QCOMPARE(svc.storedCount(), 0);
    QVERIFY(!svc.syncFromEdge());
  }

  void testLargeDataProcessing() {
    WorkflowEdgeService svc;
    WfEdgeData data;
    data.data = QByteArray(1024 * 1024, 'z');
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 100; ++i) {
      QVERIFY(!svc.processAtEdge(data).success);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 10000);
  }

  void testSyncThroughput() {
    WorkflowEdgeService svc;
    WfEdgeData data;
    data.data = QByteArray("sync-data");
    QVERIFY(!svc.storeAtEdge(data));
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 100000; ++i) {
      QVERIFY(!svc.syncFromEdge());
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
  }
};

QTEST_MAIN(WorkflowEdgeServicePerformanceTest)
#include "workflow_edge_service_performance_test.moc"
