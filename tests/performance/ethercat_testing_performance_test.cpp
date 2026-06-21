#include <QTest>
#include <QElapsedTimer>
#include "services/EtherCATTestingService.h"

class EtherCATTestingPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testRunUnitTestsPerformance() {
    EtherCATTestingService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.runUnitTests();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "1000 runUnitTests() calls:" << elapsed << "ms";
  }

  void testRunIntegrationTestsPerformance() {
    EtherCATTestingService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.runIntegrationTests();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "1000 runIntegrationTests() calls:" << elapsed << "ms";
  }

  void testRunPerformanceTestsPerformance() {
    EtherCATTestingService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.runPerformanceTests();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "1000 runPerformanceTests() calls:" << elapsed << "ms";
  }

  void testRunStressTestsPerformance() {
    EtherCATTestingService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.runStressTests();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "1000 runStressTests() calls:" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATTestingPerformanceTest)
#include "ethercat_testing_performance_test.moc"
