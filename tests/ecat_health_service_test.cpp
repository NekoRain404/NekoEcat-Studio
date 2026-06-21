// EcatHealthServiceTest — Tests for EcatHealthService
//
// Test coverage:
//   - Default state and health score
//   - DC sync and AL event status
//   - Watchdog status
//   - Slave state out-of-range query
//   - Monitoring signals and start/stop lifecycle

// EcatHealthServiceTest — Tests for EcatHealthService
//
// Test coverage:
//   - Default state (not monitoring, empty master state)
//   - Default health score, DC sync, AL event, and watchdog status
//   - Slave state for out-of-range position
//   - Monitoring signals and start/stop lifecycle
//   - Idempotent start/stop monitoring

#include <QTest>
#include <QSignalSpy>
#include "infra/EcatClient.h"
#include "services/EventBus.h"
#include "services/TopologyService.h"
#include "services/DcSyncService.h"
#include "services/AlEventService.h"
#include "services/WatchdogService.h"
#include "services/EcatHealthService.h"

class EcatHealthServiceTest : public QObject {
  Q_OBJECT
private:
  EcatClient *client_ = nullptr;
  EventBus *bus_ = nullptr;
  TopologyService *topology_ = nullptr;
  DcSyncService *dcSync_ = nullptr;
  AlEventService *alEvent_ = nullptr;
  WatchdogService *watchdog_ = nullptr;
  EcatHealthService *svc_ = nullptr;

private slots:
  void init() {
    client_ = new EcatClient(this);
    bus_ = new EventBus(this);
    topology_ = new TopologyService(client_, this);
    dcSync_ = new DcSyncService(client_, this);
    alEvent_ = new AlEventService(client_, this);
    watchdog_ = new WatchdogService(bus_, client_, this);
    svc_ = new EcatHealthService(client_, bus_, topology_, dcSync_,
                                  alEvent_, watchdog_, this);
  }

  void cleanup() {
    delete svc_;
    svc_ = nullptr;
    delete watchdog_;
    delete alEvent_;
    delete dcSync_;
    delete topology_;
    delete bus_;
    delete client_;
    watchdog_ = nullptr;
    alEvent_ = nullptr;
    dcSync_ = nullptr;
    topology_ = nullptr;
    bus_ = nullptr;
    client_ = nullptr;
  }

  // Verify default state is not monitoring with empty master state
  void testDefaultState() {
    QVERIFY(!svc_->isMonitoring());
    auto master = svc_->masterState();
    QVERIFY(master.state.isEmpty());
    QVERIFY(!master.responsive);
  }

  // Verify default health score is 100 with zero slave counts
  // Verify default health score is 100 with zero slave counts
  void testDefaultHealthScore() {
    auto health = svc_->overallHealth();
    QCOMPARE(health.score, 100);
    QVERIFY(health.totalSlaves == 0);
    QCOMPARE(health.opSlaves, 0);
    QCOMPARE(health.safeOpSlaves, 0);
    QCOMPARE(health.preOpSlaves, 0);
    QCOMPARE(health.initSlaves, 0);
    QCOMPARE(health.errorSlaves, 0);
  }

  // Verify default DC sync status is out-of-sync
  // Verify default DC sync status is not in sync
  void testDefaultDcSyncStatus() {
    auto dc = svc_->dcSyncStatus();
    QVERIFY(!dc.inSync);
    QCOMPARE(dc.driftNs, 0.0);
    QCOMPARE(dc.referencePort, 0);
  }

  // Verify default AL event status has no errors
  // Verify default AL event status has no errors
  void testDefaultAlEventStatus() {
    auto al = svc_->alEventStatus();
    QCOMPARE(al.events, quint32(0));
    QVERIFY(!al.hasError);
    QVERIFY(al.lastError.isEmpty());
  }

  // Verify default watchdog status is not triggered
  // Verify default watchdog status is not triggered
  void testDefaultWatchdogStatus() {
    auto wd = svc_->watchdogStatus();
    QVERIFY(!wd.triggered);
    QCOMPARE(wd.expiredSlaves, 0);
    QVERIFY(wd.detail.isEmpty());
  }

  // Verify slave state returns defaults for out-of-range position
  // Verify slave state returns empty for out-of-range position
  void testSlaveStateOutOfRange() {
    auto ss = svc_->slaveState(99);
    QCOMPARE(ss.position, -1);
    QVERIFY(ss.state.isEmpty());
    QVERIFY(!ss.responding);
    QVERIFY(!ss.hasError);
  }

  // Verify health and state signals are valid
  // Verify healthChanged and stateChanged signals are valid
  void testMonitoringSignals() {
    QSignalSpy healthSpy(svc_, &EcatHealthService::healthChanged);
    QSignalSpy stateSpy(svc_, &EcatHealthService::stateChanged);
    QVERIFY(healthSpy.isValid());
    QVERIFY(stateSpy.isValid());
  }

  // Verify start and stop monitoring toggle isMonitoring flag
  // Verify start/stop monitoring toggles isMonitoring flag
  void testStartStopMonitoring() {
    svc_->startMonitoring(100);
    QVERIFY(svc_->isMonitoring());
    svc_->stopMonitoring();
    QVERIFY(!svc_->isMonitoring());
  }

  // Verify starting monitoring twice is idempotent
  // Verify starting monitoring twice is idempotent
  void testStartMonitoringIdempotent() {
    svc_->startMonitoring(100);
    svc_->startMonitoring(100);
    QVERIFY(svc_->isMonitoring());
    svc_->stopMonitoring();
  }

  // Verify stopping monitoring twice is idempotent
  // Verify stopping monitoring twice is idempotent
  void testStopMonitoringIdempotent() {
    svc_->stopMonitoring();
    svc_->stopMonitoring();
    QVERIFY(!svc_->isMonitoring());
  }
};

QTEST_MAIN(EcatHealthServiceTest)
#include "ecat_health_service_test.moc"
