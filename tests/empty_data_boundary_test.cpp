#include <QTest>
#include <QSignalSpy>
#include <QVector>
#include <QString>
#include <QJsonObject>
#include "EthercatTypes.h"
#include "services/EventBus.h"

class EmptyDataBoundaryTest : public QObject {
  Q_OBJECT
private slots:
  void testEmptySlaveVector() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);

    QVector<SlaveInfo> emptySlaves;
    bus.emitSlaveChanged(emptySlaves);

    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 0);
  }

  void testEmptyTopologyVector() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::topologyChanged);

    QVector<SlaveInfo> emptyTopology;
    bus.emitTopologyChanged(emptyTopology);

    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 0);
  }

  void testEmptySdoValue() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::sdoValueReceived);

    bus.emitSdoValue(0, "", "", "");

    QCOMPARE(spy.count(), 1);
    auto args = spy.at(0);
    QCOMPARE(args.at(0).toInt(), 0);
    QCOMPARE(args.at(1).toString(), QString(""));
    QCOMPARE(args.at(2).toString(), QString(""));
    QCOMPARE(args.at(3).toString(), QString(""));
  }

  void testEmptyJsonObject() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::dcSyncUpdate);

    QJsonObject emptyObj;
    bus.emitDcSyncUpdate(emptyObj);

    QCOMPARE(spy.count(), 1);
    QJsonObject received = spy.at(0).at(0).toJsonObject();
    QVERIFY(received.isEmpty());
  }

  void testEmptyAlEvent() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::alEvent);

    QJsonObject emptyEvent;
    bus.emitAlEvent(emptyEvent);

    QCOMPARE(spy.count(), 1);
    QJsonObject received = spy.at(0).at(0).toJsonObject();
    QVERIFY(received.isEmpty());
  }

  void testEmptySignalData() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::signalData);

    QVector<double> emptyValues;
    QVector<qint64> emptyTimestamps;

    bus.emitSignalData(0, emptyValues, emptyTimestamps);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 0);
  }

  void testEmptySlaveInfo() {
    SlaveInfo info;
    QVERIFY(info.name.isEmpty());
    QCOMPARE(info.position, -1);
    QVERIFY(info.state.isEmpty());
    QVERIFY(info.flags.isEmpty());
    QVERIFY(info.rawLine.isEmpty());
  }
};

QTEST_MAIN(EmptyDataBoundaryTest)
#include "empty_data_boundary_test.moc"
