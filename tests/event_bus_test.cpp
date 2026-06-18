#include <QTest>
#include <QSignalSpy>
#include <QJsonObject>
#include "services/EventBus.h"
#include "EthercatTypes.h"

class EventBusTest : public QObject {
  Q_OBJECT
private slots:
  void testSlaveChanged() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);
    SlaveInfo info;
    info.position = 1;
    info.name = "TestSlave";
    QVector<SlaveInfo> slaves{info};
    bus.emitSlaveChanged(slaves);
    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 1);
    QCOMPARE(received.at(0).position, 1);
    QCOMPARE(received.at(0).name, QString("TestSlave"));
  }
  void testSdoValueReceived() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::sdoValueReceived);
    bus.emitSdoValue(1, "0x6040", "0x00", "0x000F");
    QCOMPARE(spy.count(), 1);
    auto args = spy.at(0);
    QCOMPARE(args.at(0).toInt(), 1);
    QCOMPARE(args.at(1).toString(), QString("0x6040"));
    QCOMPARE(args.at(2).toString(), QString("0x00"));
    QCOMPARE(args.at(3).toString(), QString("0x000F"));
  }
  void testConnectionStateChanged() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::connectionStateChanged);
    bus.emitConnectionStateChanged(true);
    QCOMPARE(spy.count(), 1);
    QVERIFY(spy.at(0).at(0).toBool());
  }
  void testFreeRunTelemetry() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::freeRunTelemetry);
    QJsonObject tel{{"running", true}};
    bus.emitFreeRunTelemetry(tel);
    QCOMPARE(spy.count(), 1);
    QVERIFY(spy.at(0).at(0).toJsonObject().contains("running"));
  }
  void testTopologyChanged() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::topologyChanged);
    SlaveInfo info;
    info.position = 1;
    info.name = "TestSlave";
    QVector<SlaveInfo> slaves{info};
    bus.emitTopologyChanged(slaves);
    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 1);
    QCOMPARE(received.at(0).position, 1);
    QCOMPARE(received.at(0).name, QString("TestSlave"));
  }
  void testDcSyncUpdate() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::dcSyncUpdate);
    QJsonObject data{{"refClock", 0}};
    bus.emitDcSyncUpdate(data);
    QCOMPARE(spy.count(), 1);
    QVERIFY(spy.at(0).at(0).toJsonObject().contains("refClock"));
  }
  void testAlEvent() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::alEvent);
    QJsonObject evt{{"slave", 1}, {"code", 0x001A}};
    bus.emitAlEvent(evt);
    QCOMPARE(spy.count(), 1);
    QJsonObject received = spy.at(0).at(0).toJsonObject();
    QVERIFY(received.contains("slave"));
    QVERIFY(received.contains("code"));
  }
  void testSignalData() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::signalData);
    bus.emitSignalData(0, {1.0, 2.0}, {100, 200});
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 0);
  }
};

QTEST_MAIN(EventBusTest)
#include "event_bus_test.moc"
