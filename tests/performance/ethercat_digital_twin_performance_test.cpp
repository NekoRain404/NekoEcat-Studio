#include <QTest>
#include <QElapsedTimer>
#include "services/EtherCATDigitalTwinService.h"

class EtherCATDigitalTwinPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testCreateTwinThroughput() {
    EtherCATDigitalTwinService svc;
    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++)
      svc.createDigitalTwin(i);

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    QCOMPARE(svc.allTwins().size(), count);
    qDebug() << "Twin creation throughput:" << count << "twins in" << elapsed << "ms";
  }

  void testSyncThroughput() {
    EtherCATDigitalTwinService svc;
    for (int i = 0; i < 100; i++)
      svc.createDigitalTwin(i);

    QElapsedTimer timer;
    timer.start();

    const int count = 100;
    for (int i = 0; i < count; i++)
      svc.syncWithPhysical(i);

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Twin sync throughput:" << count << "syncs in" << elapsed << "ms";
  }

  void testSimulationThroughput() {
    EtherCATDigitalTwinService svc;
    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      TwinScenario scenario;
      scenario.name = QString("scenario_%1").arg(i);
      svc.simulateScenario(scenario);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Twin simulation throughput:" << count << "simulations in" << elapsed << "ms";
  }

  void testPredictThroughput() {
    EtherCATDigitalTwinService svc;
    QVector<TwinDataPoint> data;
    for (int i = 0; i < 100; i++) {
      TwinDataPoint dp;
      dp.value = i * 1.5;
      dp.timestamp = QDateTime::currentDateTime().addSecs(i * 60);
      data.append(dp);
    }

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++)
      svc.predictBehavior(data);

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Twin prediction throughput:" << count << "predictions in" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATDigitalTwinPerformanceTest)
#include "ethercat_digital_twin_performance_test.moc"
