#include <QTest>
#include <QSignalSpy>
#include <QElapsedTimer>
#include "services/MasterApiService.h"
#include "infra/EcatClient.h"

class MasterApiPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testErrorPathThroughput() {
    EcatClient client;
    MasterApiService svc(&client);
    QSignalSpy spy(&svc, &MasterApiService::error);

    QElapsedTimer timer;
    timer.start();

    const int count = 10000;
    for (int i = 0; i < count; i++) {
      MasterApiService s(nullptr);
      s.createMaster();
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "Error path throughput:" << count << "createMaster(nullptr) in" << elapsed << "ms";
  }

  void testStateQueryLatency() {
    EcatClient client;
    MasterApiService svc(&client);

    QElapsedTimer timer;
    timer.start();

    const int count = 100000;
    volatile int sink = 0;
    for (int i = 0; i < count; i++) {
      MasterApiState st = svc.masterState();
      sink = st.slavesResponding;
    }

    qint64 elapsed = timer.elapsed();
    Q_UNUSED(sink);
    QVERIFY(elapsed < 500);
    qDebug() << "State query latency:" << count << "masterState() in" << elapsed << "ms";
  }

  void testSlaveConfigThroughput() {
    EcatClient client;
    MasterApiService svc(&client);
    svc.createMaster();

    QElapsedTimer timer;
    timer.start();

    const int count = 100000;
    volatile int sink = 0;
    for (int i = 0; i < count; i++) {
      SlaveApiConfig cfg = svc.slaveConfig(i % 100);
      sink = cfg.position;
    }

    qint64 elapsed = timer.elapsed();
    Q_UNUSED(sink);
    QVERIFY(elapsed < 500);
    qDebug() << "SlaveConfig throughput:" << count << "slaveConfig() in" << elapsed << "ms";
  }

  void testMemoryStability() {
    for (int round = 0; round < 100; round++) {
      for (int i = 0; i < 1000; i++) {
        MasterApiService s(nullptr);
        QSignalSpy spy(&s, &MasterApiService::error);
        s.createMaster();
        QCOMPARE(spy.count(), 1);
      }
    }

    qDebug() << "Memory stability: 100k create/error cycles completed";
  }
};

QTEST_MAIN(MasterApiPerformanceTest)
#include "master_api_performance_test.moc"
