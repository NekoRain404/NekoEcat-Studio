#include <QTest>
#include <QElapsedTimer>
#include "services/RedundancyService.h"
#include "MockEcatClient.h"

class RedundancyPerformanceTest : public QObject {
  Q_OBJECT
private:
  MockEcatClient *client = nullptr;

private slots:
  void initTestCase() {
    client = new MockEcatClient(this);
  }

  void testEnableDisablePerformance() {
    RedundancyService svc(client);
    svc.setPrimaryPath(10);
    svc.setSecondaryPath(10);
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.enableRedundancy();
      svc.disableRedundancy();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 500);
  }

  void testFailoverPerformance() {
    RedundancyService svc(client);
    svc.setPrimaryPath(10);
    svc.setSecondaryPath(10);
    svc.enableRedundancy();
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.failover();
      svc.failback();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 500);
  }

  void testHistoryPerformance() {
    RedundancyService svc(client);
    svc.setPrimaryPath(10);
    svc.setSecondaryPath(10);
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.enableRedundancy();
      svc.failover();
      svc.failback();
      svc.disableRedundancy();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
  }

  void testQueryPerformance() {
    RedundancyService svc(client);
    svc.setPrimaryPath(10);
    svc.setSecondaryPath(10);
    svc.enableRedundancy();
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      svc.currentState();
      svc.isRedundant();
      svc.primaryPath();
      svc.secondaryPath();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 100);
  }
};

QTEST_MAIN(RedundancyPerformanceTest)
#include "redundancy_performance_test.moc"
