#include <QTest>
#include <QElapsedTimer>
#include "services/EtherCATCloudService.h"

class EtherCATCloudPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testConnectThroughput() {
    EtherCATCloudService svc(nullptr);
    CloudConfig cfg;
    cfg.endpoint = QStringLiteral("https://cloud.example.com");
    cfg.apiKey = QStringLiteral("key123");

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      svc.connectToCloud(cfg);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Cloud connect throughput:" << count << "connects in" << elapsed << "ms";
  }

  void testSyncThroughput() {
    EtherCATCloudService svc(nullptr);
    CloudConfig cfg;
    cfg.endpoint = QStringLiteral("https://cloud.example.com");
    cfg.apiKey = QStringLiteral("key123");
    svc.connectToCloud(cfg);

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      svc.syncToCloud();
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Cloud sync throughput:" << count << "syncs in" << elapsed << "ms";
  }

  void testBackupThroughput() {
    EtherCATCloudService svc(nullptr);
    CloudConfig cfg;
    cfg.endpoint = QStringLiteral("https://cloud.example.com");
    cfg.apiKey = QStringLiteral("key123");
    svc.connectToCloud(cfg);

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      svc.backupToCloud();
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Cloud backup throughput:" << count << "backups in" << elapsed << "ms";
  }

  void testMonitorThroughput() {
    EtherCATCloudService svc(nullptr);

    QElapsedTimer timer;
    timer.start();

    const int count = 10000;
    for (int i = 0; i < count; i++) {
      svc.monitorCloud();
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "Cloud monitor throughput:" << count << "monitors in" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATCloudPerformanceTest)
#include "ethercat_cloud_performance_test.moc"
