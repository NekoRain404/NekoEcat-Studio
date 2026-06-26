// EtherCATEdgeServiceTest — Tests for EtherCATEdgeService
//
// Test coverage:
//   - Edge data processing (valid + empty)
//   - Edge data analysis (statistics, empty)
//   - Local edge data storage and sync rejection without backend
//   - Signal emission for processing and analysis

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATEdgeService.h"

class EtherCATEdgeServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Verify valid edge data is processed successfully
  void testProcessAtEdge() {
    EtherCATEdgeService svc(nullptr);
    EdgeData data;
    data.data = QByteArray("test payload");
    data.source = QStringLiteral("sensor");
    EdgeResult result = svc.processAtEdge(data);
    QVERIFY(result.success);
    QVERIFY(result.output.size() > 0);
  }

  // Verify empty edge data fails processing
  void testProcessAtEdgeEmpty() {
    EtherCATEdgeService svc(nullptr);
    EdgeData data;
    EdgeResult result = svc.processAtEdge(data);
    QVERIFY(!result.success);
  }

  // Verify valid edge data is analyzed with sample count
  void testAnalyzeAtEdge() {
    EtherCATEdgeService svc(nullptr);
    EdgeData data;
    data.data = QByteArray("1.0 2.0 3.0 4.0 5.0");
    EdgeAnalysis analysis = svc.analyzeAtEdge(data);
    QVERIFY(analysis.success);
    QVERIFY(analysis.sampleCount > 0);
  }

  // Verify empty edge data fails analysis
  void testAnalyzeAtEdgeEmpty() {
    EtherCATEdgeService svc(nullptr);
    EdgeData data;
    EdgeAnalysis analysis = svc.analyzeAtEdge(data);
    QVERIFY(!analysis.success);
  }

  // Verify edge analysis computes min, max, mean, and variance
  void testAnalyzeAtEdgeStatistics() {
    EtherCATEdgeService svc(nullptr);
    EdgeData data;
    data.data = QByteArray("\x0a\x14\x1e\x28\x32", 5);
    EdgeAnalysis analysis = svc.analyzeAtEdge(data);
    QVERIFY(analysis.success);
    QCOMPARE(analysis.min, 10.0);
    QCOMPARE(analysis.max, 50.0);
    QCOMPARE(analysis.mean, 30.0);
    QVERIFY(analysis.variance > 0.0);
  }

  // Verify storing valid edge data increments count
  void testStoreAtEdge() {
    EtherCATEdgeService svc(nullptr);
    QCOMPARE(svc.storedCount(), 0);
    EdgeData data;
    data.data = QByteArray("stored payload");
    bool ok = svc.storeAtEdge(data);
    QVERIFY(ok);
    QCOMPARE(svc.storedCount(), 1);
  }

  // Verify storing empty edge data fails
  void testStoreAtEdgeEmpty() {
    EtherCATEdgeService svc(nullptr);
    EdgeData data;
    bool ok = svc.storeAtEdge(data);
    QVERIFY(!ok);
    QCOMPARE(svc.storedCount(), 0);
  }

  // Verify sync fails closed without a real edge backend
  void testSyncFromEdge() {
    EtherCATEdgeService svc(nullptr);
    EdgeData data;
    data.data = QByteArray("sync payload");
    svc.storeAtEdge(data);
    QCOMPARE(svc.storedCount(), 1);
    QVERIFY(!svc.syncFromEdge());
  }

  // Verify sync fails when nothing is stored
  void testSyncFromEdgeNothingStored() {
    EtherCATEdgeService svc(nullptr);
    QVERIFY(!svc.syncFromEdge());
  }

  // Verify edgeProcessed signal is emitted on processing
  void testEdgeProcessedSignal() {
    EtherCATEdgeService svc(nullptr);
    QSignalSpy spy(&svc, &EtherCATEdgeService::edgeProcessed);
    QVERIFY(spy.isValid());
    EdgeData data;
    data.data = QByteArray("signal test");
    svc.processAtEdge(data);
    QCOMPARE(spy.count(), 1);
  }

  // Verify edgeAnalyzed signal is emitted on analysis
  void testEdgeAnalyzedSignal() {
    EtherCATEdgeService svc(nullptr);
    QSignalSpy spy(&svc, &EtherCATEdgeService::edgeAnalyzed);
    QVERIFY(spy.isValid());
    EdgeData data;
    data.data = QByteArray("10 20 30");
    svc.analyzeAtEdge(data);
    QCOMPARE(spy.count(), 1);
  }
};

QTEST_MAIN(EtherCATEdgeServiceTest)
#include "ethercat_edge_service_test.moc"
