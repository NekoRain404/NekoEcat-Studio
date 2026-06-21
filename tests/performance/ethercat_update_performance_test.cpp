#include <QTest>
#include <QElapsedTimer>
#include "services/EtherCATUpdateService.h"

class EtherCATUpdatePerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testCheckPerformance() {
    EtherCATUpdateService svc(nullptr, nullptr);
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.checkForUpdates(i % 10);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 500);
    qDebug() << "1000 check calls:" << elapsed << "ms";
  }

  void testStartUpdatePerformance() {
    EtherCATUpdateService svc(nullptr, nullptr);
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.startUpdate(i % 10, "2.0.0");
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 500);
    qDebug() << "1000 update calls:" << elapsed << "ms";
  }

  void testHistoryPerformance() {
    EtherCATUpdateService svc(nullptr, nullptr);
    for (int i = 0; i < 100; i++) {
      svc.checkForUpdates(i);
    }
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.getUpdateHistory();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 100);
    qDebug() << "1000 history calls:" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATUpdatePerformanceTest)
#include "ethercat_update_performance_test.moc"
