// NetworkDiagnosticsServiceTest — Tests for NetworkDiagnosticsService
//
// Test coverage:
//   - Default unknown health state (ports, errors, bandwidth, latency)
//   - Error counter defaults and reset
//   - Bandwidth utilization default
//   - Port status for out-of-range and negative indices
//   - All port status on empty network
//   - Monitoring signal validity
//   - Offline start/stop monitoring with idempotency

#include "infra/EcatClient.h"
#include "services/NetworkDiagnosticsService.h"
#include <QSignalSpy>
#include <QTest>

class NetworkDiagnosticsServiceTest : public QObject {
    Q_OBJECT
private slots:
    // No port/link evidence must not be reported as a good network.
    void testDefaultState() {
        EcatClient client;
        NetworkDiagnosticsService svc(&client);
        QVERIFY(!svc.isMonitoring());
        auto health = svc.currentHealth();
        QCOMPARE(health.portCount, 0);
        QCOMPARE(health.activePorts, 0);
        QCOMPARE(health.errorCount, 0);
        QCOMPARE(health.bandwidth, 0.0);
        QCOMPARE(health.latencyMs, 0.0);
        QCOMPARE(health.jitterMs, 0.0);
        QCOMPARE(health.overall, NetworkHealth::Status::Unknown);
    }

    // Verify error counters default to zero
    void testErrorCountersDefault() {
        EcatClient client;
        NetworkDiagnosticsService svc(&client);
        auto errors = svc.errorCounters();
        QCOMPARE(errors.crc, quint64(0));
        QCOMPARE(errors.frame, quint64(0));
        QCOMPARE(errors.lost, quint64(0));
        QCOMPARE(errors.overrun, quint64(0));
    }

    // Verify bandwidth utilization defaults to zero
    void testBandwidthUtilizationDefault() {
        EcatClient client;
        NetworkDiagnosticsService svc(&client);
        QCOMPARE(svc.bandwidthUtilization(), 0.0);
    }

    // Test reset error counters
    void testResetErrorCounters() {
        EcatClient client;
        NetworkDiagnosticsService svc(&client);
        svc.resetErrorCounters();
        auto errors = svc.errorCounters();
        QCOMPARE(errors.crc, quint64(0));
        QCOMPARE(errors.frame, quint64(0));
        QCOMPARE(errors.lost, quint64(0));
        QCOMPARE(errors.overrun, quint64(0));
    }

    // Verify port status for out-of-range index
    void testPortStatusOutOfRange() {
        EcatClient client;
        NetworkDiagnosticsService svc(&client);
        auto ps = svc.portStatus(99);
        QCOMPARE(ps.port, -1);
        QVERIFY(!ps.linkUp);
        QCOMPARE(ps.speedMbps, 0);
        QVERIFY(!ps.fullDuplex);
        QCOMPARE(ps.errorCount, 0);
    }

    // Verify port status for negative index
    void testPortStatusNegative() {
        EcatClient client;
        NetworkDiagnosticsService svc(&client);
        auto ps = svc.portStatus(-1);
        QCOMPARE(ps.port, -1);
    }

    // Verify all port status is empty initially
    void testAllPortStatusEmpty() {
        EcatClient client;
        NetworkDiagnosticsService svc(&client);
        auto all = svc.allPortStatus();
        QVERIFY(all.isEmpty());
    }

    // Verify monitoring signals are valid
    void testMonitoringSignals() {
        EcatClient client;
        NetworkDiagnosticsService svc(&client);
        QSignalSpy healthSpy(&svc, &NetworkDiagnosticsService::healthUpdated);
        QSignalSpy portSpy(&svc, &NetworkDiagnosticsService::portStatusChanged);
        QSignalSpy errorSpy(&svc, &NetworkDiagnosticsService::errorDetected);
        QVERIFY(healthSpy.isValid());
        QVERIFY(portSpy.isValid());
        QVERIFY(errorSpy.isValid());
    }

    // Offline start must not synthesize active monitoring.
    void testStartStopMonitoringOffline() {
        EcatClient client;
        NetworkDiagnosticsService svc(&client);
        svc.startMonitoring(100);
        QVERIFY(!svc.isMonitoring());
        svc.stopMonitoring();
        QVERIFY(!svc.isMonitoring());
    }

    // Verify repeated offline start remains inactive.
    void testStartMonitoringIdempotentOffline() {
        EcatClient client;
        NetworkDiagnosticsService svc(&client);
        svc.startMonitoring(100);
        svc.startMonitoring(100);
        QVERIFY(!svc.isMonitoring());
        svc.stopMonitoring();
    }

    // Verify stop monitoring is idempotent
    void testStopMonitoringIdempotent() {
        EcatClient client;
        NetworkDiagnosticsService svc(&client);
        svc.stopMonitoring();
        svc.stopMonitoring();
        QVERIFY(!svc.isMonitoring());
    }
};

QTEST_MAIN(NetworkDiagnosticsServiceTest)
#include "network_diagnostics_service_test.moc"
