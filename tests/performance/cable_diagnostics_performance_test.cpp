#include <QTest>
#include <QElapsedTimer>
#include "services/CableDiagnosticsService.h"

class CableDiagnosticsPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testSinglePortPerformance() {
    CableDiagnosticsService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.testPort(0);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 500);
  }

  void testAllPortsPerformance() {
    CableDiagnosticsService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 100; i++) {
      svc.testAllPorts(10);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
  }

  void testHistoryPerformance() {
    CableDiagnosticsService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.testPort(i % 10);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 500);
    QCOMPARE(svc.testHistory(0).size(), 0);
  }

  void testQueryPerformance() {
    CableDiagnosticsService svc;
    for (int i = 0; i < 10; i++) {
      svc.testPort(i);
    }
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      svc.lastResult(i % 10);
      svc.testHistory(i % 10);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 100);
  }
};

QTEST_MAIN(CableDiagnosticsPerformanceTest)
#include "cable_diagnostics_performance_test.moc"
