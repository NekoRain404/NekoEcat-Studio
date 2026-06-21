#include <QTest>
#include <QElapsedTimer>
#include "services/EtherCATValidationService.h"

class EtherCATValidationPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testValidateConfigurationPerformance() {
    EtherCATValidationService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      svc.validateConfiguration();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "10000 validateConfiguration() calls:" << elapsed << "ms";
  }

  void testValidateNetworkPerformance() {
    EtherCATValidationService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      svc.validateNetwork();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "10000 validateNetwork() calls:" << elapsed << "ms";
  }

  void testValidateTimingPerformance() {
    EtherCATValidationService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      svc.validateTiming();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "10000 validateTiming() calls:" << elapsed << "ms";
  }

  void testValidateSafetyPerformance() {
    EtherCATValidationService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      svc.validateSafety();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "10000 validateSafety() calls:" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATValidationPerformanceTest)
#include "ethercat_validation_performance_test.moc"
