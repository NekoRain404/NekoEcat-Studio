// ErrorRecoveryTest — Tests for error recovery and edge cases via EventBus
//
// Test coverage:
//   - Invalid slave position handling
//   - Empty and malformed SDO values
//   - Empty AL event forwarding
//   - Rapid connection state changes
//   - Large SDO value passthrough
//   - Special characters in slave names

#include <QTest>
#include <QSignalSpy>
#include <QJsonObject>
#include "services/EventBus.h"
#include "EthercatTypes.h"

class ErrorRecoveryTest : public QObject {
  Q_OBJECT
private slots:
  // Verify invalid slave position propagates through EventBus
  void testInvalidSlavePosition() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);

    SlaveInfo info;
    info.position = -1;  // Invalid position
    info.name = "InvalidSlave";

    QVector<SlaveInfo> slaves{info};
    bus.emitSlaveChanged(slaves);

    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 1);
    QCOMPARE(received.at(0).position, -1);
  }

  // Verify empty SDO index propagates through EventBus
  void testEmptySdoIndex() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::sdoValueReceived);

    bus.emitSdoValue(1, "", "0x00", "0x0000");

    QCOMPARE(spy.count(), 1);
    auto args = spy.at(0);
    QCOMPARE(args.at(1).toString(), QString(""));
  }

  // Verify malformed SDO value propagates unchanged
  void testMalformedSdoValue() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::sdoValueReceived);

    bus.emitSdoValue(1, "0x6040", "0x00", "invalid_value");

    QCOMPARE(spy.count(), 1);
    auto args = spy.at(0);
    QCOMPARE(args.at(3).toString(), QString("invalid_value"));
  }

  // Verify empty AL event JSON propagates through EventBus
  void testEmptyAlEvent() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::alEvent);

    QJsonObject emptyEvent;
    bus.emitAlEvent(emptyEvent);

    QCOMPARE(spy.count(), 1);
    QJsonObject received = spy.at(0).at(0).toJsonObject();
    QVERIFY(received.isEmpty());
  }

  // Verify rapid connection state changes all propagate
  void testInvalidConnectionState() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::connectionStateChanged);

    // Emit multiple state changes rapidly
    bus.emitConnectionStateChanged(true);
    bus.emitConnectionStateChanged(false);
    bus.emitConnectionStateChanged(true);

    QCOMPARE(spy.count(), 3);
    QVERIFY(spy.at(0).at(0).toBool());
    QVERIFY(!spy.at(1).at(0).toBool());
    QVERIFY(spy.at(2).at(0).toBool());
  }

  // Verify empty signal data vectors propagate through EventBus
  void testEmptySignalData() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::signalData);

    QVector<double> emptyValues;
    QVector<qint64> emptyTimestamps;

    bus.emitSignalData(0, emptyValues, emptyTimestamps);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 0);
  }

  // Verify large SDO value (10k chars) propagates unchanged
  void testLargeSdoValue() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::sdoValueReceived);

    QString largeValue(10000, 'A');
    bus.emitSdoValue(1, "0x6040", "0x00", largeValue);

    QCOMPARE(spy.count(), 1);
    auto args = spy.at(0);
    QCOMPARE(args.at(3).toString(), largeValue);
  }

  // Verify special characters in slave names survive round-trip
  void testSpecialCharactersInName() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);

    SlaveInfo info;
    info.position = 1;
    info.name = "Slave with spaces & special chars: @#$%";

    QVector<SlaveInfo> slaves{info};
    bus.emitSlaveChanged(slaves);

    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.at(0).name, info.name);
  }
};

QTEST_MAIN(ErrorRecoveryTest)
#include "error_recovery_test.moc"
