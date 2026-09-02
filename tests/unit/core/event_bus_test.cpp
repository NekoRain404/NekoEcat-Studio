// EventBus Test Suite
//
// This test suite verifies the EventBus central inter-plugin signal hub.
//
// Test Coverage:
//   - slaveChanged: emit/receive QVector<SlaveInfo> payload
//   - sdoValueReceived: emit/receive SDO value (position, index, subindex, value)
//   - connectionStateChanged: emit/receive connection state boolean
//   - freeRunTelemetry: emit/receive JSON telemetry object
//   - topologyChanged: emit/receive topology change notification
//   - dcSyncUpdate: emit/receive DC sync status JSON
//   - alEvent: emit/receive AL event JSON
//   - signalData: emit/receive multi-channel signal data
//
// Test Dependencies:
//   - Qt6::Test (QTest framework)
//   - Qt6::Core (for QSignalSpy, QJsonObject)
//   - EventBus (central signal hub)
//   - EthercatTypes (SlaveInfo structure)
//
// Test Environment:
//   - No QApplication required (pure signal/slot tests)
//   - Uses QSignalSpy to verify signal emission and payload

#include "EthercatTypes.h"
#include "services/EventBus.h"
#include <QJsonObject>
#include <QSignalSpy>
#include <QTest>

/// Test suite verifying EventBus signal emission and payload delivery.
class EventBusTest : public QObject {
    Q_OBJECT
private slots:
    // Test that slaveChanged signal emits correct QVector<SlaveInfo> payload.
    // Setup: Create EventBus, emit slave list with one slave.
    // Assert: Signal count is 1, received payload matches position and name.
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
    // Test that sdoValueReceived signal emits correct 4-arg payload.
    // Setup: Emit SDO value with position=1, index=0x6040, sub=0x00, value=0x000F.
    // Assert: All 4 arguments match expected values.
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
    // Test that connectionStateChanged signal emits boolean payload.
    // Setup: Emit connection state = true.
    // Assert: Signal emitted once, payload is true.
    void testConnectionStateChanged() {
        EventBus bus;
        QSignalSpy spy(&bus, &EventBus::connectionStateChanged);
        bus.emitConnectionStateChanged(true);
        QCOMPARE(spy.count(), 1);
        QVERIFY(spy.at(0).at(0).toBool());
    }
    // Test that freeRunTelemetry signal emits JSON object payload.
    // Setup: Emit telemetry JSON with "running" key.
    // Assert: Signal emitted once, payload contains "running" key.
    void testFreeRunTelemetry() {
        EventBus bus;
        QSignalSpy spy(&bus, &EventBus::freeRunTelemetry);
        QJsonObject tel{{"running", true}};
        bus.emitFreeRunTelemetry(tel);
        QCOMPARE(spy.count(), 1);
        QVERIFY(spy.at(0).at(0).toJsonObject().contains("running"));
    }
    // Test that topologyChanged signal emits QVector<SlaveInfo> payload.
    // Setup: Emit topology change with one slave.
    // Assert: Signal emitted once, payload matches slave data.
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
    // Test that dcSyncUpdate signal emits JSON object payload.
    // Setup: Emit DC sync data with "refClock" key.
    // Assert: Signal emitted once, payload contains "refClock" key.
    void testDcSyncUpdate() {
        EventBus bus;
        QSignalSpy spy(&bus, &EventBus::dcSyncUpdate);
        QJsonObject data{{"refClock", 0}};
        bus.emitDcSyncUpdate(data);
        QCOMPARE(spy.count(), 1);
        QVERIFY(spy.at(0).at(0).toJsonObject().contains("refClock"));
    }
    // Test that alEvent signal emits JSON object payload.
    // Setup: Emit AL event with "slave" and "code" keys.
    // Assert: Signal emitted once, payload contains both keys.
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
    // Test that signalData signal emits channel ID, values, and timestamps.
    // Setup: Emit signal data for channel 0 with 2 samples.
    // Assert: Signal emitted once, channel ID matches.
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
