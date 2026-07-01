// RedundancyServiceTest — Tests for RedundancyService
//
// Test coverage:
//   - Initial state (SinglePath, not redundant)
//   - Primary and secondary path configuration
//   - Enable/disable redundancy fail closed without a live backend
//   - Enable without paths fails
//   - Repeated enable attempts fail closed
//   - Failover cannot be simulated without a live backend
//   - Failover without redundancy fails
//   - Failback cannot be simulated without a live backend
//   - Failback without prior failover fails
//   - Redundancy history tracking

// RedundancyServiceTest — Tests for RedundancyService
//
// Test coverage:
//   - Initial state (SinglePath, not redundant)
//   - Primary and secondary path configuration
//   - Enable/disable/failover/failback fail closed without a live backend
//   - Redundancy history is not synthesized from offline attempts
//   - Error cases (enable without paths, failover without redundancy)
#include <QTest>
#include <QSignalSpy>
#include <QScopedPointer>
#include "services/RedundancyService.h"
#include "MockEcatClient.h"

class RedundancyServiceTest : public QObject {
  Q_OBJECT
private:
  MockEcatClient *client = nullptr;

private slots:
  void initTestCase() {
    client = new MockEcatClient(this);
  }

  void cleanupTestCase() {
    // client is parented to this, Qt handles deletion
  }

  // Verify initial state is SinglePath and not redundant
  void testInitialState() {
    RedundancyService svc(client);
    QCOMPARE(svc.currentState(), RedundancyState::SinglePath);
    QVERIFY(!svc.isRedundant());
  }
  // Test setting primary path configuration
  // Set primary path and verify slaveCount, state, isHealthy
  void testSetPrimaryPath() {
    RedundancyService svc(client);
    svc.setPrimaryPath(4);
    auto p = svc.primaryPath();
    QCOMPARE(p.slaveCount, 4);
    QCOMPARE(p.state, PathState::Active);
    QVERIFY(p.isHealthy);
  }
  // Test setting secondary path configuration
  // Set secondary path and verify slaveCount, state, isHealthy
  void testSetSecondaryPath() {
    RedundancyService svc(client);
    svc.setSecondaryPath(4);
    auto s = svc.secondaryPath();
    QCOMPARE(s.slaveCount, 4);
    QCOMPARE(s.state, PathState::Standby);
    QVERIFY(s.isHealthy);
  }
  // Verify enabling redundancy cannot be simulated without a live backend.
  void testEnableRedundancyFailsClosedWithoutBackend() {
    RedundancyService svc(client);
    QSignalSpy spy(&svc, &RedundancyService::redundancyStateChanged);
    svc.setPrimaryPath(4);
    svc.setSecondaryPath(4);
    QVERIFY(!svc.enableRedundancy());
    QCOMPARE(spy.count(), 0);
    QCOMPARE(svc.currentState(), RedundancyState::SinglePath);
    QVERIFY(!svc.isRedundant());
    QCOMPARE(svc.redundancyHistory().size(), 0);
  }
  // Verify enable fails without paths
  // Verify enabling redundancy without paths fails
  void testEnableRedundancyFailsWithoutPaths() {
    RedundancyService svc(client);
    QVERIFY(!svc.enableRedundancy());
  }
  // Verify repeated enable attempts still fail closed.
  void testEnableRedundancyRepeatedAttemptFailsClosed() {
    RedundancyService svc(client);
    svc.setPrimaryPath(2);
    svc.setSecondaryPath(2);
    QVERIFY(!svc.enableRedundancy());
    QVERIFY(!svc.enableRedundancy());
  }
  // Verify disabling redundancy cannot be simulated without a live backend.
  void testDisableRedundancyFailsClosedWithoutBackend() {
    RedundancyService svc(client);
    QSignalSpy spy(&svc, &RedundancyService::redundancyStateChanged);
    svc.setPrimaryPath(2);
    svc.setSecondaryPath(2);
    svc.enableRedundancy();
    QVERIFY(!svc.disableRedundancy());
    QCOMPARE(spy.count(), 0);
    QCOMPARE(svc.currentState(), RedundancyState::SinglePath);
  }
  // Verify failover cannot be simulated without a live backend.
  void testFailoverFailsClosedWithoutBackend() {
    RedundancyService svc(client);
    QSignalSpy failoverSpy(&svc, &RedundancyService::failoverOccurred);
    QSignalSpy pathSpy(&svc, &RedundancyService::pathStateChanged);
    svc.setPrimaryPath(3);
    svc.setSecondaryPath(3);
    svc.enableRedundancy();
    QVERIFY(!svc.failover());
    QCOMPARE(failoverSpy.count(), 0);
    QCOMPARE(pathSpy.count(), 0);
    QCOMPARE(svc.currentState(), RedundancyState::SinglePath);
    QCOMPARE(svc.primaryPath().state, PathState::Active);
    QCOMPARE(svc.secondaryPath().state, PathState::Standby);
  }
  // Verify failover fails without redundancy
  // Verify failover without redundancy fails
  void testFailoverWithoutRedundancy() {
    RedundancyService svc(client);
    QVERIFY(!svc.failover());
  }
  // Verify failback cannot be simulated without a live backend.
  void testFailbackFailsClosedWithoutBackend() {
    RedundancyService svc(client);
    svc.setPrimaryPath(2);
    svc.setSecondaryPath(2);
    svc.enableRedundancy();
    svc.failover();
    QVERIFY(!svc.failback());
    QCOMPARE(svc.currentState(), RedundancyState::SinglePath);
    QCOMPARE(svc.primaryPath().state, PathState::Active);
    QCOMPARE(svc.secondaryPath().state, PathState::Standby);
  }
  // Verify failback fails without prior failover
  // Verify failback without prior failover fails
  void testFailbackWithoutFailover() {
    RedundancyService svc(client);
    svc.setPrimaryPath(2);
    svc.setSecondaryPath(2);
    svc.enableRedundancy();
    QVERIFY(!svc.failback());
  }
  // Verify offline redundancy operations do not synthesize history.
  void testHistoryNotSynthesizedOffline() {
    RedundancyService svc(client);
    svc.setPrimaryPath(2);
    svc.setSecondaryPath(2);
    svc.enableRedundancy();
    svc.failover();
    svc.failback();
    auto history = svc.redundancyHistory();
    QCOMPARE(history.size(), 0);
  }
};

QTEST_MAIN(RedundancyServiceTest)
#include "redundancy_service_test.moc"
