// CableDiagnosticsServiceTest — Tests for CableDiagnosticsService
//
// Test coverage:
//   - Single port failure without a physical diagnostics backend
//   - All ports failure report without synthetic pass data
//   - Test history remains empty while offline
//   - Last result retrieval remains empty while offline
//   - History clearing

#include <QTest>
#include <QSignalSpy>
#include "services/CableDiagnosticsService.h"

class CableDiagnosticsServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Verify single port diagnostics cannot be simulated without a backend.
  void testTestPortFailsWithoutBackend() {
    CableDiagnosticsService svc;
    QSignalSpy startedSpy(&svc, &CableDiagnosticsService::testStarted);
    QSignalSpy completedSpy(&svc, &CableDiagnosticsService::testCompleted);
    auto result = svc.testPort(0);
    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(completedSpy.count(), 0);
    QCOMPARE(result.portId, 0);
    QCOMPARE(result.status, CableTestStatus::Error);
    QCOMPARE(result.faultType, CableFaultType::Unknown);
    QCOMPARE(result.cableLengthM, 0.0);
    QCOMPARE(result.impedanceOhms, 0.0);
    QCOMPARE(result.signalQuality, 0.0);
    QVERIFY(result.details.contains(QStringLiteral("backend")));
  }
  // Verify all ports diagnostics report failure instead of synthetic pass data.
  void testTestAllPortsFailsWithoutBackend() {
    CableDiagnosticsService svc;
    QSignalSpy spy(&svc, &CableDiagnosticsService::diagnosticsCompleted);
    auto report = svc.testAllPorts(4);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(report.results.size(), 4);
    QCOMPARE(report.passedCount, 0);
    QCOMPARE(report.failedCount, 4);
    QVERIFY(!report.allPassed);
  }
  // Verify offline diagnostics do not create synthetic history.
  void testTestHistoryRemainsEmptyWithoutBackend() {
    CableDiagnosticsService svc;
    svc.testPort(0);
    svc.testPort(0);
    auto history = svc.testHistory(0);
    QCOMPARE(history.size(), 0);
  }
  // Verify offline diagnostics do not create synthetic last results.
  void testLastResultRemainsEmptyWithoutBackend() {
    CableDiagnosticsService svc;
    svc.testPort(1);
    auto last = svc.lastResult(1);
    QCOMPARE(last.status, CableTestStatus::NotRun);
  }
  // Verify clear history reports false when offline tests created no history.
  void testClearHistoryWithoutBackendHistory() {
    CableDiagnosticsService svc;
    svc.testPort(0);
    QVERIFY(!svc.clearHistory(0));
    QCOMPARE(svc.testHistory(0).size(), 0);
  }
  // Verify clear returns false for nonexistent history
  void testClearNonexistentHistory() {
    CableDiagnosticsService svc;
    QVERIFY(!svc.clearHistory(99));
  }
};

QTEST_MAIN(CableDiagnosticsServiceTest)
#include "cable_diagnostics_service_test.moc"
