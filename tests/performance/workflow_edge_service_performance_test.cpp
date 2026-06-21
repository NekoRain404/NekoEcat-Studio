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
      svc.processAtEdge(data);
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
      svc.analyzeAtEdge(data);
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
      svc.storeAtEdge(data);
    }
    QCOMPARE(svc.storedCount(), 10000);
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
  }

  void testMemoryStability() {
    WorkflowEdgeService svc;
    WfEdgeData data;
    data.data = QByteArray("test");
    for (int i = 0; i < 10000; ++i) {
      svc.processAtEdge(data);
      svc.storeAtEdge(data);
    }
    QCOMPARE(svc.storedCount(), 10000);
    QVERIFY(svc.syncFromEdge());
  }

  void testLargeDataProcessing() {
    WorkflowEdgeService svc;
    WfEdgeData data;
    data.data = QByteArray(1024 * 1024, 'z');
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 100; ++i) {
      svc.processAtEdge(data);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 10000);
  }

  void testSyncThroughput() {
    WorkflowEdgeService svc;
    WfEdgeData data;
    data.data = QByteArray("sync-data");
    svc.storeAtEdge(data);
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 100000; ++i) {
      svc.syncFromEdge();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
  }
};

QTEST_MAIN(WorkflowEdgeServicePerformanceTest)
#include "workflow_edge_service_performance_test.moc"
