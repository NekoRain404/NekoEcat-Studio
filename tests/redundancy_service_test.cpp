// RedundancyServiceTest — Tests for RedundancyService
//
// Test coverage:
//   - Initial state (SinglePath, not redundant)
//   - Primary and secondary path configuration
//   - Enable/disable redundancy with signals
//   - Enable without paths fails
//   - Enable when already enabled
//   - Failover with state transitions and signal
//   - Failover without redundancy fails
//   - Failback to dual-path state
//   - Failback without prior failover fails
//   - Redundancy history tracking

// RedundancyServiceTest — Tests for RedundancyService
//
// Test coverage:
//   - Initial state (SinglePath, not redundant)
//   - Primary and secondary path configuration
//   - Enable/disable redundancy with signal verification
//   - Failover and failback state transitions
//   - Redundancy history tracking
//   - Error cases (enable without paths, failover without redundancy)
#include <QTest>
#include <QSignalSpy>
#include "services/RedundancyService.h"

class RedundancyServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Verify initial state is SinglePath and not redundant
  void testInitialState() {
    RedundancyService svc;
    QCOMPARE(svc.currentState(), RedundancyState::SinglePath);
    QVERIFY(!svc.isRedundant());
  }
  // Test setting primary path configuration
  // Set primary path and verify slaveCount, state, isHealthy
  void testSetPrimaryPath() {
    RedundancyService svc;
    svc.setPrimaryPath(4);
    auto p = svc.primaryPath();
    QCOMPARE(p.slaveCount, 4);
    QCOMPARE(p.state, PathState::Active);
    QVERIFY(p.isHealthy);
  }
  // Test setting secondary path configuration
  // Set secondary path and verify slaveCount, state, isHealthy
  void testSetSecondaryPath() {
    RedundancyService svc;
    svc.setSecondaryPath(4);
    auto s = svc.secondaryPath();
    QCOMPARE(s.slaveCount, 4);
    QCOMPARE(s.state, PathState::Standby);
    QVERIFY(s.isHealthy);
  }
  // Test enabling redundancy with both paths
  // Enable redundancy and verify signal, state, and isRedundant
  void testEnableRedundancy() {
    RedundancyService svc;
    QSignalSpy spy(&svc, &RedundancyService::redundancyStateChanged);
    svc.setPrimaryPath(4);
    svc.setSecondaryPath(4);
    QVERIFY(svc.enableRedundancy());
    QCOMPARE(spy.count(), 1);
    QCOMPARE(svc.currentState(), RedundancyState::DualPath);
    QVERIFY(svc.isRedundant());
  }
  // Verify enable fails without paths
  // Verify enabling redundancy without paths fails
  void testEnableRedundancyFailsWithoutPaths() {
    RedundancyService svc;
    QVERIFY(!svc.enableRedundancy());
  }
  // Verify enable is idempotent when already enabled
  // Verify enabling redundancy when already enabled succeeds
  void testEnableRedundancyAlreadyEnabled() {
    RedundancyService svc;
    svc.setPrimaryPath(2);
    svc.setSecondaryPath(2);
    svc.enableRedundancy();
    QVERIFY(svc.enableRedundancy());
  }
  // Test disabling redundancy
  // Disable redundancy and verify signal and state reverts to SinglePath
  void testDisableRedundancy() {
    RedundancyService svc;
    QSignalSpy spy(&svc, &RedundancyService::redundancyStateChanged);
    svc.setPrimaryPath(2);
    svc.setSecondaryPath(2);
    svc.enableRedundancy();
    QVERIFY(svc.disableRedundancy());
    QCOMPARE(spy.count(), 2);
    QCOMPARE(svc.currentState(), RedundancyState::SinglePath);
  }
  // Test failover with state transitions and signal
  // Trigger failover and verify signal, Failover state, path states
  void testFailover() {
    RedundancyService svc;
    QSignalSpy spy(&svc, &RedundancyService::failoverOccurred);
    svc.setPrimaryPath(3);
    svc.setSecondaryPath(3);
    svc.enableRedundancy();
    QVERIFY(svc.failover());
    QCOMPARE(spy.count(), 1);
    QCOMPARE(svc.currentState(), RedundancyState::Failover);
    QCOMPARE(svc.primaryPath().state, PathState::Failed);
    QCOMPARE(svc.secondaryPath().state, PathState::Active);
  }
  // Verify failover fails without redundancy
  // Verify failover without redundancy fails
  void testFailoverWithoutRedundancy() {
    RedundancyService svc;
    QVERIFY(!svc.failover());
  }
  // Test failback restores dual-path state
  // Failback after failover and verify DualPath state restored
  void testFailback() {
    RedundancyService svc;
    svc.setPrimaryPath(2);
    svc.setSecondaryPath(2);
    svc.enableRedundancy();
    svc.failover();
    QVERIFY(svc.failback());
    QCOMPARE(svc.currentState(), RedundancyState::DualPath);
    QCOMPARE(svc.primaryPath().state, PathState::Active);
    QCOMPARE(svc.secondaryPath().state, PathState::Standby);
  }
  // Verify failback fails without prior failover
  // Verify failback without prior failover fails
  void testFailbackWithoutFailover() {
    RedundancyService svc;
    svc.setPrimaryPath(2);
    svc.setSecondaryPath(2);
    svc.enableRedundancy();
    QVERIFY(!svc.failback());
  }
  // Test redundancy history records
  // Verify redundancy history records enable, failover, failback
  void testHistory() {
    RedundancyService svc;
    svc.setPrimaryPath(2);
    svc.setSecondaryPath(2);
    svc.enableRedundancy();
    svc.failover();
    svc.failback();
    auto history = svc.redundancyHistory();
    QCOMPARE(history.size(), 3);
  }
};

QTEST_MAIN(RedundancyServiceTest)
#include "redundancy_service_test.moc"
