// EmptyDataBoundaryTest — Tests for empty and boundary data conditions
//
// Test coverage:
//   - Empty slave and topology vectors
//   - Empty SDO value forwarding
//   - Empty JSON object forwarding
//   - Empty AL event forwarding
//   - Empty signal data forwarding
//   - Default SlaveInfo field state

// EmptyDataBoundaryTest — Tests for EventBus and SlaveInfo with empty data
//
// Test coverage:
//   - Empty slave/topology vector emission
//   - Empty SDO value emission
//   - Empty JSON object emission (DC sync, AL event)
//   - Empty signal data emission
//   - Default SlaveInfo field values

#include "EthercatTypes.h"
#include "services/EventBus.h"
#include <QJsonObject>
#include <QSignalSpy>
#include <QString>
#include <QTest>
#include <QVector>

class EmptyDataBoundaryTest : public QObject {
    Q_OBJECT
private slots:
    // Verify empty slave vector emits signal with zero-size result
    void testEmptySlaveVector() {
        EventBus bus;
        QSignalSpy spy(&bus, &EventBus::slaveChanged);

        QVector<SlaveInfo> emptySlaves;
        bus.emitSlaveChanged(emptySlaves);

        QCOMPARE(spy.count(), 1);
        QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
        QCOMPARE(received.size(), 0);
    }

    // Verify empty topology vector propagates through EventBus
    // Verify empty topology vector emits signal with zero-size result
    void testEmptyTopologyVector() {
        EventBus bus;
        QSignalSpy spy(&bus, &EventBus::topologyChanged);

        QVector<SlaveInfo> emptyTopology;
        bus.emitTopologyChanged(emptyTopology);

        QCOMPARE(spy.count(), 1);
        QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
        QCOMPARE(received.size(), 0);
    }

    // Verify empty SDO value fields propagate through EventBus
    // Verify empty SDO value strings are forwarded correctly
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

    // Verify empty JSON object propagates through DC sync update
    // Verify empty JSON object is forwarded for DC sync update
    void testEmptyJsonObject() {
        EventBus bus;
        QSignalSpy spy(&bus, &EventBus::dcSyncUpdate);

        QJsonObject emptyObj;
        bus.emitDcSyncUpdate(emptyObj);

        QCOMPARE(spy.count(), 1);
        QJsonObject received = spy.at(0).at(0).toJsonObject();
        QVERIFY(received.isEmpty());
    }

    // Verify empty AL event JSON propagates through EventBus
    // Verify empty JSON object is forwarded for AL event
    void testEmptyAlEvent() {
        EventBus bus;
        QSignalSpy spy(&bus, &EventBus::alEvent);

        QJsonObject emptyEvent;
        bus.emitAlEvent(emptyEvent);

        QCOMPARE(spy.count(), 1);
        QJsonObject received = spy.at(0).at(0).toJsonObject();
        QVERIFY(received.isEmpty());
    }

    // Verify empty signal data vectors propagate through EventBus
    // Verify empty signal data vectors are forwarded correctly
    void testEmptySignalData() {
        EventBus bus;
        QSignalSpy spy(&bus, &EventBus::signalData);

        QVector<double> emptyValues;
        QVector<qint64> emptyTimestamps;

        bus.emitSignalData(0, emptyValues, emptyTimestamps);

        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toInt(), 0);
    }

    // Verify default SlaveInfo has empty fields and position -1
    // Verify default SlaveInfo has empty fields and position -1
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
