// EventBusIntegrationTest — Tests for EventBus
//
// Test coverage:
//   - Slave changed, SDO value, and connection state signals
//   - Topology changed and DC sync update signals
//   - AL event and signal data signals
//   - Free-run telemetry signal

#include <QTest>
#include <QSignalSpy>
#include <QJsonObject>
#include "services/EventBus.h"
#include "EthercatTypes.h"

class EventBusIntegrationTest : public QObject {
  Q_OBJECT
private slots:
  // Emit slaveChanged and verify received data
  void testSlaveChangedIntegration() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);
    
    SlaveInfo info1;
    info1.position = 1;
    info1.name = "Slave1";
    SlaveInfo info2;
    info2.position = 2;
    info2.name = "Slave2";
    
    QVector<SlaveInfo> slaves{info1, info2};
    bus.emitSlaveChanged(slaves);
    
    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 2);
    QCOMPARE(received.at(0).position, 1);
    QCOMPARE(received.at(1).position, 2);
  }

  // Emit multiple SDO values and verify arguments
  void testSdoValueIntegration() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::sdoValueReceived);
    
    bus.emitSdoValue(1, "0x6040", "0x00", "0x000F");
    bus.emitSdoValue(2, "0x6041", "0x00", "0x0000");
    
    QCOMPARE(spy.count(), 2);
    
    auto args1 = spy.at(0);
    QCOMPARE(args1.at(0).toInt(), 1);
    QCOMPARE(args1.at(1).toString(), QString("0x6040"));
    
    auto args2 = spy.at(1);
    QCOMPARE(args2.at(0).toInt(), 2);
    QCOMPARE(args2.at(1).toString(), QString("0x6041"));
  }

  // Emit connection state changes and verify signals
  void testConnectionStateIntegration() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::connectionStateChanged);
    
    bus.emitConnectionStateChanged(true);
    bus.emitConnectionStateChanged(false);
    
    QCOMPARE(spy.count(), 2);
    QVERIFY(spy.at(0).at(0).toBool());
    QVERIFY(!spy.at(1).at(0).toBool());
  }

  // Emit topologyChanged and verify slave data
  void testTopologyChangedIntegration() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::topologyChanged);
    
    SlaveInfo info;
    info.position = 1;
    info.name = "TopologySlave";
    QVector<SlaveInfo> slaves{info};
    
    bus.emitTopologyChanged(slaves);
    
    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 1);
    QCOMPARE(received.at(0).name, QString("TopologySlave"));
  }

  // Emit DC sync update and verify JSON fields
  void testDcSyncUpdateIntegration() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::dcSyncUpdate);
    
    QJsonObject data{{"refClock", 0}, {"sync0", 1000}};
    bus.emitDcSyncUpdate(data);
    
    QCOMPARE(spy.count(), 1);
    QJsonObject received = spy.at(0).at(0).toJsonObject();
    QVERIFY(received.contains("refClock"));
    QVERIFY(received.contains("sync0"));
  }

  // Emit AL event and verify slave and code fields
  void testAlEventIntegration() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::alEvent);
    
    QJsonObject event{{"slave", 1}, {"code", 0x001A}, {"text", "Sync error"}};
    bus.emitAlEvent(event);
    
    QCOMPARE(spy.count(), 1);
    QJsonObject received = spy.at(0).at(0).toJsonObject();
    QCOMPARE(received["slave"].toInt(), 1);
    QCOMPARE(received["code"].toInt(), 0x001A);
  }

  // Emit signal data and verify channel index
  void testSignalDataIntegration() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::signalData);
    
    QVector<double> values{1.0, 2.0, 3.0};
    QVector<qint64> timestamps{100, 200, 300};
    
    bus.emitSignalData(0, values, timestamps);
    
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 0);
  }

  // Emit free-run telemetry and verify JSON fields
  void testFreeRunTelemetryIntegration() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::freeRunTelemetry);
    
    QJsonObject tel{{"running", true}, {"frequency", 1000}};
    bus.emitFreeRunTelemetry(tel);
    
    QCOMPARE(spy.count(), 1);
    QJsonObject received = spy.at(0).at(0).toJsonObject();
    QVERIFY(received.contains("running"));
    QVERIFY(received.contains("frequency"));
  }
};

QTEST_MAIN(EventBusIntegrationTest)
#include "event_bus_integration_test.moc"
