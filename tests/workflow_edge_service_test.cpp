#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowEdgeService.h"

class WorkflowEdgeServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testProcessAtEdge() {
    WorkflowEdgeService svc;
    QSignalSpy spy(&svc, &WorkflowEdgeService::edgeProcessed);
    WfEdgeData data;
    data.data = QByteArray("hello");
    data.source = "sensor-01";
    auto result = svc.processAtEdge(data);
    QVERIFY(result.success);
    QCOMPARE(result.output, QByteArray("hello"));
    QCOMPARE(spy.count(), 1);
  }

  void testProcessAtEdgeEmptyData() {
    WorkflowEdgeService svc;
    WfEdgeData data;
    data.data = QByteArray();
    auto result = svc.processAtEdge(data);
    QVERIFY(!result.success);
    QVERIFY(!result.error.isEmpty());
  }

  void testAnalyzeAtEdge() {
    WorkflowEdgeService svc;
    QSignalSpy spy(&svc, &WorkflowEdgeService::edgeAnalyzed);
    WfEdgeData data;
    data.data = QByteArray("test-data-for-analysis");
    auto analysis = svc.analyzeAtEdge(data);
    QVERIFY(analysis.success);
    QCOMPARE(analysis.sampleCount, data.data.size());
    QCOMPARE(spy.count(), 1);
  }

  void testAnalyzeAtEdgeEmptyData() {
    WorkflowEdgeService svc;
    WfEdgeData data;
    data.data = QByteArray();
    auto analysis = svc.analyzeAtEdge(data);
    QVERIFY(!analysis.success);
  }

  void testStoreAtEdge() {
    WorkflowEdgeService svc;
    WfEdgeData data;
    data.data = QByteArray("stored-data");
    data.source = "sensor-01";
    QVERIFY(svc.storeAtEdge(data));
    QCOMPARE(svc.storedCount(), 1);
    QVERIFY(svc.storeAtEdge(data));
    QCOMPARE(svc.storedCount(), 2);
  }

  void testStoreAtEdgeEmptyData() {
    WorkflowEdgeService svc;
    WfEdgeData data;
    data.data = QByteArray();
    QVERIFY(!svc.storeAtEdge(data));
    QCOMPARE(svc.storedCount(), 0);
  }

  void testSyncFromEdge() {
    WorkflowEdgeService svc;
    QVERIFY(!svc.syncFromEdge());
    WfEdgeData data;
    data.data = QByteArray("sync-data");
    svc.storeAtEdge(data);
    QVERIFY(svc.syncFromEdge());
  }

  void testProcessResultOutput() {
    WorkflowEdgeService svc;
    WfEdgeData data;
    data.data = QByteArray("roundtrip");
    auto result = svc.processAtEdge(data);
    QVERIFY(result.success);
    QCOMPARE(result.output, data.data);
  }

  void testAnalysisPattern() {
    WorkflowEdgeService svc;
    WfEdgeData data;
    data.data = QByteArray("pattern-data");
    auto analysis = svc.analyzeAtEdge(data);
    QVERIFY(analysis.success);
    QCOMPARE(analysis.pattern, QString("uniform"));
  }

  void testMultipleStores() {
    WorkflowEdgeService svc;
    for (int i = 0; i < 100; ++i) {
      WfEdgeData data;
      data.data = QByteArray("data-" + QByteArray::number(i));
      svc.storeAtEdge(data);
    }
    QCOMPARE(svc.storedCount(), 100);
    QVERIFY(svc.syncFromEdge());
  }
};

QTEST_MAIN(WorkflowEdgeServiceTest)
#include "workflow_edge_service_test.moc"
