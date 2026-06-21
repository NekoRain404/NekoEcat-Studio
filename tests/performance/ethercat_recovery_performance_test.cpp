#include <QTest>
#include <QElapsedTimer>
#include "services/EtherCATRecoveryService.h"

class EtherCATRecoveryPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testExecuteRecoveryPerformance() {
    EtherCATRecoveryService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.executeAutoRecovery();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "1000 executeAutoRecovery() calls:" << elapsed << "ms";
  }

  void testDiagnoseErrorsPerformance() {
    EtherCATRecoveryService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      svc.diagnoseErrors();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "10000 diagnoseErrors() calls:" << elapsed << "ms";
  }

  void testStatusQueryPerformance() {
    EtherCATRecoveryService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      svc.status();
      svc.availableActions();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 100);
    qDebug() << "10000 status+actions queries:" << elapsed << "ms";
  }

  void testResetStatusPerformance() {
    EtherCATRecoveryService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      svc.resetStatus();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 100);
    qDebug() << "10000 resetStatus() calls:" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATRecoveryPerformanceTest)
#include "ethercat_recovery_performance_test.moc"
