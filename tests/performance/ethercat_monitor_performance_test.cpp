#include <QTest>
#include <QElapsedTimer>
#include "services/EtherCATMonitorService.h"

class EtherCATMonitorPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testStartStopPerformance() {
    EtherCATMonitorService svc(nullptr, nullptr);
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.startMonitoring(10000);
      svc.stopMonitoring();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 500);
    qDebug() << "Start/stop 1000 cycles:" << elapsed << "ms";
  }

  void testPollPerformance() {
    EtherCATMonitorService svc(nullptr, nullptr);
    svc.startMonitoring(1);
    QVERIFY(!svc.isMonitoring());
    QElapsedTimer timer;
    timer.start();
    QTest::qWait(100);
    svc.stopMonitoring();
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 500);
    qDebug() << "Polling for 100ms:" << elapsed << "ms";
  }

  void testQueryPerformance() {
    EtherCATMonitorService svc(nullptr, nullptr);
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      svc.busTraffic();
      svc.errorRate();
      svc.performance();
      svc.health();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 100);
    qDebug() << "10000 query cycles:" << elapsed << "ms";
  }

  void testMonitoringOverhead() {
    EtherCATMonitorService svc(nullptr, nullptr);
    QElapsedTimer timer;
    timer.start();
    svc.startMonitoring(1);
    QVERIFY(!svc.isMonitoring());
    for (int i = 0; i < 100; i++) {
      svc.busTraffic();
      svc.errorRate();
      svc.performance();
      svc.health();
    }
    svc.stopMonitoring();
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "Monitoring + 100 queries:" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATMonitorPerformanceTest)
#include "ethercat_monitor_performance_test.moc"
