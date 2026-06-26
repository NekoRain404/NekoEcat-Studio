#include <QTest>
#include <QElapsedTimer>
#include <QStringList>
#include "services/EtherCATReplicationService.h"
#include "services/EventBus.h"
#include "infra/EcatClient.h"

class EtherCATReplicationPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testConfigReplicationOfflineRejectionThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATReplicationService svc(&bus, &client);

    QStringList targets;
    for (int i = 0; i < 10; i++) {
      targets << QString("target_%1").arg(i);
    }

    QElapsedTimer timer;
    timer.start();

    const int count = 100;
    for (int i = 0; i < count; i++) {
      QVERIFY(!svc.replicateConfiguration(targets));
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "Config replication offline rejection throughput:" << count << "runs with 10 targets in" << elapsed << "ms";
  }

  void testDataReplicationOfflineRejectionThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATReplicationService svc(&bus, &client);

    QStringList targets;
    for (int i = 0; i < 10; i++) {
      targets << QString("target_%1").arg(i);
    }

    QElapsedTimer timer;
    timer.start();

    const int count = 100;
    for (int i = 0; i < count; i++) {
      QVERIFY(!svc.replicateData(targets));
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "Data replication offline rejection throughput:" << count << "runs with 10 targets in" << elapsed << "ms";
  }

  void testStateReplicationOfflineRejectionThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATReplicationService svc(&bus, &client);

    QStringList targets;
    for (int i = 0; i < 10; i++) {
      targets << QString("target_%1").arg(i);
    }

    QElapsedTimer timer;
    timer.start();

    const int count = 100;
    for (int i = 0; i < count; i++) {
      QVERIFY(!svc.replicateState(targets));
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "State replication offline rejection throughput:" << count << "runs with 10 targets in" << elapsed << "ms";
  }

  void testBackupReplicationOfflineRejectionThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATReplicationService svc(&bus, &client);

    QStringList targets;
    for (int i = 0; i < 10; i++) {
      targets << QString("target_%1").arg(i);
    }

    QElapsedTimer timer;
    timer.start();

    const int count = 100;
    for (int i = 0; i < count; i++) {
      QVERIFY(!svc.replicateBackup(targets));
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "Backup replication offline rejection throughput:" << count << "runs with 10 targets in" << elapsed << "ms";
  }

  void testHistoryQueryPerformance() {
    EventBus bus;
    EcatClient client;
    EtherCATReplicationService svc(&bus, &client);

    QElapsedTimer timer;
    timer.start();

    const int count = 10000;
    for (int i = 0; i < count; i++) {
      svc.replicationHistory();
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "ReplicationHistory query throughput:" << count << "queries in" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATReplicationPerformanceTest)
#include "ethercat_replication_performance_test.moc"
