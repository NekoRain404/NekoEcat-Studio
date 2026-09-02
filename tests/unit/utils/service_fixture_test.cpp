// ServiceTestFixtureTest — Tests for ServiceTestFixture
//
// Test coverage:
//   - Fixture setup creates valid container, client, and event bus
//   - Connection state simulation
//   - Slave change simulation
//   - SDO value simulation

#include "fixtures/ServiceTestFixture.h"
#include "infra/EcatClient.h"
#include "services/EventBus.h"
#include "services/ServiceContainer.h"
#include <QSignalSpy>
#include <QTest>

class ServiceTestFixtureTest : public QObject {
    Q_OBJECT
private slots:
    // Verify fixture creates non-null container, client, and event bus
    void testSetup() {
        ServiceTestFixture fixture;
        QVERIFY(fixture.container() != nullptr);
        QVERIFY(fixture.container()->client() != nullptr);
        QVERIFY(fixture.container()->eventBus() != nullptr);
    }

    // Test connection state simulation emits correct signal
    void testSimulateConnection() {
        ServiceTestFixture fixture;
        QSignalSpy spy(fixture.container()->eventBus(), &EventBus::connectionStateChanged);
        fixture.simulateConnection(true);
        QCOMPARE(spy.count(), 1);
        QVERIFY(spy.at(0).at(0).toBool());
    }

    // Test slave change simulation emits correct slave info
    void testSimulateSlaveChange() {
        ServiceTestFixture fixture;
        QSignalSpy spy(fixture.container()->eventBus(), &EventBus::slaveChanged);
        fixture.simulateSlaveChange(1, "TestSlave", "OP");
        QCOMPARE(spy.count(), 1);
        auto slaves = spy.at(0).at(0).value<QVector<SlaveInfo>>();
        QCOMPARE(slaves.size(), 1);
        QCOMPARE(slaves[0].position, 1);
        QCOMPARE(slaves[0].name, QString("TestSlave"));
    }

    // Test SDO value simulation emits correct data
    void testSimulateSdoValue() {
        ServiceTestFixture fixture;
        QSignalSpy spy(fixture.container()->eventBus(), &EventBus::sdoValueReceived);
        fixture.simulateSdoValue(1, "0x6040", "0x00", "0x000F");
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toInt(), 1);
        QCOMPARE(spy.at(0).at(1).toString(), QString("0x6040"));
    }
};

QTEST_MAIN(ServiceTestFixtureTest)
#include "service_fixture_test.moc"
