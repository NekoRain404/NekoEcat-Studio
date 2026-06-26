#include <QTest>
#include <QElapsedTimer>
#include "services/EtherCATEdgeService.h"

class EtherCATEdgePerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testProcessThroughput() {
    EtherCATEdgeService svc(nullptr);
    EdgeData data;
    data.data = QByteArray(100, 'A');
    data.size = 100;
    data.source = QStringLiteral("perf-test");

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      EdgeResult result = svc.processAtEdge(data);
      QVERIFY(!result.success);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Edge process throughput:" << count << "processes in" << elapsed << "ms";
  }

  void testAnalyzeThroughput() {
    EtherCATEdgeService svc(nullptr);
    EdgeData data;
    data.data = QByteArray(100, 'A');
    data.size = 100;

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      EdgeAnalysis analysis = svc.analyzeAtEdge(data);
      QVERIFY(!analysis.success);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Edge analyze throughput:" << count << "analyzes in" << elapsed << "ms";
  }

  void testStoreThroughput() {
    EtherCATEdgeService svc(nullptr);
    EdgeData data;
    data.data = QByteArray(100, 'A');
    data.size = 100;

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      QVERIFY(!svc.storeAtEdge(data));
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Edge store throughput:" << count << "stores in" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATEdgePerformanceTest)
#include "ethercat_edge_performance_test.moc"
