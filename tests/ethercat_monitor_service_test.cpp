// EtherCATMonitorServiceTest — Tests for EtherCATMonitorService
//
// Test coverage:
//   - Default state (monitoring off, zero traffic, unknown health)
//   - Offline start/stop monitoring rejection (including double start/stop)
//   - Traffic, error rate, performance, and health updates
//   - Signal emission and health grade defaults

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATMonitorService.h"

class EtherCATMonitorServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Default state: not monitoring, zero traffic, unknown health
  void testDefaultState() {
    EtherCATMonitorService svc(nullptr, nullptr);
    QVERIFY(!svc.isMonitoring());
    QCOMPARE(svc.busTraffic().txFrames, static_cast<quint64>(0));
    QCOMPARE(svc.errorRate().rate, 0.0);
    QCOMPARE(svc.health().score, 0);
    QCOMPARE(svc.health().grade, QStringLiteral("Unknown"));
  }

  // Offline start must not synthesize active monitoring.
  void testStartStopOffline() {
    EtherCATMonitorService svc(nullptr, nullptr);
    svc.startMonitoring(10000);
    QVERIFY(!svc.isMonitoring());
    svc.stopMonitoring();
    QVERIFY(!svc.isMonitoring());
  }

  // Double offline start remains inactive.
  void testDoubleStartOffline() {
    EtherCATMonitorService svc(nullptr, nullptr);
    svc.startMonitoring(10000);
    svc.startMonitoring(10000);
    QVERIFY(!svc.isMonitoring());
    svc.stopMonitoring();
  }

  // Double stop is safe when not monitoring
  // Double stop when already stopped is safe
  void testDoubleStop() {
    EtherCATMonitorService svc(nullptr, nullptr);
    svc.stopMonitoring();
    QVERIFY(!svc.isMonitoring());
  }

  // Update and read back bus traffic metrics
  // Update bus traffic and verify values
  void testUpdateTraffic() {
    EtherCATMonitorService svc(nullptr, nullptr);
    BusTraffic t;
    t.txFrames = 100;
    t.rxFrames = 99;
    t.bandwidth = 10.5;
    svc.updateTraffic(t);
    QCOMPARE(svc.busTraffic().txFrames, static_cast<quint64>(100));
    QCOMPARE(svc.busTraffic().bandwidth, 10.5);
  }

  // Update and read back error rate
  // Update error rate and verify values
  void testUpdateErrorRate() {
    EtherCATMonitorService svc(nullptr, nullptr);
    ErrorRate r;
    r.rate = 0.01;
    r.totalErrors = 5;
    svc.updateErrorRate(r);
    QCOMPARE(svc.errorRate().rate, 0.01);
    QCOMPARE(svc.errorRate().totalErrors, static_cast<quint64>(5));
  }

  // Update and read back performance metrics
  // Update performance metrics and verify values
  void testUpdatePerformance() {
    EtherCATMonitorService svc(nullptr, nullptr);
    PerformanceMetrics m;
    m.cycleTimeUs = 1000.0;
    m.jitterUs = 5.0;
    svc.updatePerformance(m);
    QCOMPARE(svc.performance().cycleTimeUs, 1000.0);
    QCOMPARE(svc.performance().jitterUs, 5.0);
  }

  // Update and read back health status
  // Update health status and verify score and slave counts
  void testUpdateHealth() {
    EtherCATMonitorService svc(nullptr, nullptr);
    HealthStatus h;
    h.score = 85;
    h.grade = QStringLiteral("B");
    h.totalSlaves = 4;
    h.opSlaves = 3;
    svc.updateHealth(h);
    QCOMPARE(svc.health().score, 85);
    QCOMPARE(svc.health().totalSlaves, 4);
  }

  // Offline monitoring must not emit runtime traffic updates.
  void testOfflineUpdateDoesNotEmitSignal() {
    EtherCATMonitorService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATMonitorService::trafficUpdated);
    BusTraffic t;
    t.txFrames = 50;
    svc.startMonitoring(10000);
    svc.updateTraffic(t);
    QCOMPARE(spy.count(), 0);
    svc.stopMonitoring();
  }

  // Health defaults must not claim watchdog/DC health before sampling evidence.
  void testHealthGradeDefaults() {
    EtherCATMonitorService svc(nullptr, nullptr);
    QCOMPARE(svc.health().score, 0);
    QCOMPARE(svc.health().grade, QStringLiteral("Unknown"));
    QVERIFY(!svc.health().watchdogOk);
    QVERIFY(!svc.health().dcInSync);
  }
};

QTEST_MAIN(EtherCATMonitorServiceTest)
#include "ethercat_monitor_service_test.moc"
