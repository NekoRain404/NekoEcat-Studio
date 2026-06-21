#include <QTest>
#include <QElapsedTimer>
#include "services/EtherCATSyncService.h"
#include "services/EventBus.h"
#include "infra/EcatClient.h"

class EtherCATSyncPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testTimeSyncThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATSyncService svc(&bus, &client);

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      svc.syncTime();
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "Time sync throughput:" << count << "syncs in" << elapsed << "ms";
  }

  void testDataSyncThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATSyncService svc(&bus, &client);

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      svc.syncData();
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "Data sync throughput:" << count << "syncs in" << elapsed << "ms";
  }

  void testStateSyncThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATSyncService svc(&bus, &client);

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      svc.syncState();
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "State sync throughput:" << count << "syncs in" << elapsed << "ms";
  }

  void testConfigSyncThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATSyncService svc(&bus, &client);

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      svc.syncConfiguration();
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "Config sync throughput:" << count << "syncs in" << elapsed << "ms";
  }

  void testStatusQueryPerformance() {
    EventBus bus;
    EcatClient client;
    EtherCATSyncService svc(&bus, &client);

    QElapsedTimer timer;
    timer.start();

    const int count = 10000;
    for (int i = 0; i < count; i++) {
      svc.syncStatus();
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "SyncStatus query throughput:" << count << "queries in" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATSyncPerformanceTest)
#include "ethercat_sync_performance_test.moc"
