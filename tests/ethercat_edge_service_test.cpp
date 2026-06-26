// EtherCATEdgeServiceTest — Tests for EtherCATEdgeService
//
// Test coverage:
//   - Edge processing, analysis, storage, and sync reject without backend
//   - Signal emission for processing and analysis
//   - Source guard against synthetic edge success

#include <QFile>
#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATEdgeService.h"

class EtherCATEdgeServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Verify valid edge data fails closed without a real edge backend
  void testProcessAtEdgeRequiresBackend() {
    EtherCATEdgeService svc(nullptr);
    EdgeData data;
    data.data = QByteArray("test payload");
    data.source = QStringLiteral("sensor");
    EdgeResult result = svc.processAtEdge(data);
    QVERIFY(!result.success);
    QVERIFY(result.output.isEmpty());
    QVERIFY(result.error.contains(QStringLiteral("backend"), Qt::CaseInsensitive));
  }

  // Verify empty edge data fails processing
  void testProcessAtEdgeEmpty() {
    EtherCATEdgeService svc(nullptr);
    EdgeData data;
    EdgeResult result = svc.processAtEdge(data);
    QVERIFY(!result.success);
  }

  // Verify valid edge data analysis fails closed without a real edge backend
  void testAnalyzeAtEdgeRequiresBackend() {
    EtherCATEdgeService svc(nullptr);
    EdgeData data;
    data.data = QByteArray("1.0 2.0 3.0 4.0 5.0");
    EdgeAnalysis analysis = svc.analyzeAtEdge(data);
    QVERIFY(!analysis.success);
    QCOMPARE(analysis.sampleCount, 0);
  }

  // Verify empty edge data fails analysis
  void testAnalyzeAtEdgeEmpty() {
    EtherCATEdgeService svc(nullptr);
    EdgeData data;
    EdgeAnalysis analysis = svc.analyzeAtEdge(data);
    QVERIFY(!analysis.success);
  }

  // Verify edge analysis does not synthesize statistics without a backend
  void testAnalyzeAtEdgeStatisticsRequireBackend() {
    EtherCATEdgeService svc(nullptr);
    EdgeData data;
    data.data = QByteArray("\x0a\x14\x1e\x28\x32", 5);
    EdgeAnalysis analysis = svc.analyzeAtEdge(data);
    QVERIFY(!analysis.success);
    QCOMPARE(analysis.sampleCount, 0);
    QCOMPARE(analysis.min, 0.0);
    QCOMPARE(analysis.max, 0.0);
    QCOMPARE(analysis.mean, 0.0);
    QCOMPARE(analysis.variance, 0.0);
  }

  // Verify storing valid edge data fails closed without a real edge backend
  void testStoreAtEdgeRequiresBackend() {
    EtherCATEdgeService svc(nullptr);
    QCOMPARE(svc.storedCount(), 0);
    EdgeData data;
    data.data = QByteArray("stored payload");
    bool ok = svc.storeAtEdge(data);
    QVERIFY(!ok);
    QCOMPARE(svc.storedCount(), 0);
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
    QVERIFY(!svc.storeAtEdge(data));
    QCOMPARE(svc.storedCount(), 0);
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

  void noSyntheticSuccessInSource() {
    QFile source(QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/services/EtherCATEdgeService.cpp"));
    QVERIFY2(source.open(QIODevice::ReadOnly | QIODevice::Text),
             qPrintable(QStringLiteral("Unable to open %1").arg(source.fileName())));
    const QString text = QString::fromUtf8(source.readAll());
    QVERIFY2(!text.contains(QStringLiteral("result.success = true")),
             "Edge processing must not synthesize success without a backend.");
    QVERIFY2(!text.contains(QStringLiteral("analysis.success = true")),
             "Edge analysis must not synthesize success without a backend.");
  }
};

QTEST_MAIN(EtherCATEdgeServiceTest)
#include "ethercat_edge_service_test.moc"
