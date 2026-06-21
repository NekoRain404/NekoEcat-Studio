#include <QTest>
#include <QSignalSpy>
#include <QElapsedTimer>
#include <QJsonObject>
#include "services/EventBus.h"
#include "EthercatTypes.h"

class EventBusPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testThroughput() {
    EventBus bus;
    QElapsedTimer timer;
    timer.start();

    const int count = 10000;
    for (int i = 0; i < count; i++) {
      bus.emitConnectionStateChanged(i % 2 == 0);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "EventBus throughput:" << count << "events in" << elapsed << "ms";
  }

  void testLatency() {
    EventBus bus;
    QElapsedTimer timer;
    const int iterations = 1000;
    qint64 totalNs = 0;

    for (int i = 0; i < iterations; i++) {
      timer.start();
      bus.emitConnectionStateChanged(true);
      totalNs += timer.nsecsElapsed();
    }

    double avgUs = (totalNs / iterations) / 1000.0;
    QVERIFY(avgUs < 1000.0);
    qDebug() << "EventBus avg latency:" << avgUs << "us per emit";
  }

  void testSlaveChangedThroughput() {
    EventBus bus;
    QElapsedTimer timer;

    QVector<SlaveInfo> slaves;
    for (int i = 0; i < 100; i++) {
      SlaveInfo s;
      s.position = i;
      s.name = QString("Slave_%1").arg(i);
      s.state = "OP";
      slaves.append(s);
    }

    timer.start();
    const int count = 1000;
    for (int i = 0; i < count; i++) {
      bus.emitSlaveChanged(slaves);
    }
    qint64 elapsed = timer.elapsed();

    QVERIFY(elapsed < 2000);
    qDebug() << "SlaveChanged throughput:" << count << "events with 100 slaves in" << elapsed << "ms";
  }

  void testSdoValueThroughput() {
    EventBus bus;
    QElapsedTimer timer;
    timer.start();

    const int count = 10000;
    for (int i = 0; i < count; i++) {
      bus.emitSdoValue(i % 10, "0x6040", "0x00", "0x000F");
    }
    qint64 elapsed = timer.elapsed();

    QVERIFY(elapsed < 1000);
    qDebug() << "SdoValue throughput:" << count << "events in" << elapsed << "ms";
  }

  void testSignalDataThroughput() {
    EventBus bus;
    QElapsedTimer timer;

    QVector<double> values(100, 1.0);
    QVector<qint64> timestamps(100, 1000);

    timer.start();
    const int count = 1000;
    for (int i = 0; i < count; i++) {
      bus.emitSignalData(0, values, timestamps);
    }
    qint64 elapsed = timer.elapsed();

    QVERIFY(elapsed < 2000);
    qDebug() << "SignalData throughput:" << count << "events with 100 samples in" << elapsed << "ms";
  }

  void testMemoryStability() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::connectionStateChanged);

    for (int round = 0; round < 100; round++) {
      for (int i = 0; i < 1000; i++) {
        bus.emitConnectionStateChanged(i % 2 == 0);
      }
    }

    QCOMPARE(spy.count(), 100000);
    qDebug() << "Memory stability: 100k events processed, spy count:" << spy.count();
  }

  void testJsonEventThroughput() {
    EventBus bus;
    QElapsedTimer timer;

    QJsonObject data{{"refClock", 0}, {"sync0", 1000}, {"sync1", 2000}};
    timer.start();

    const int count = 10000;
    for (int i = 0; i < count; i++) {
      bus.emitDcSyncUpdate(data);
    }
    qint64 elapsed = timer.elapsed();

    QVERIFY(elapsed < 2000);
    qDebug() << "JsonEvent throughput:" << count << "dcSync events in" << elapsed << "ms";
  }
};

QTEST_MAIN(EventBusPerformanceTest)
#include "eventbus_performance_test.moc"
