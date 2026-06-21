// CableDiagnosticsServiceTest — Tests for CableDiagnosticsService
//
// Test coverage:
//   - Single port test
//   - All ports test
//   - Test history tracking
//   - Last result retrieval
//   - History clearing
//   - Cable length variation by port

#include <QTest>
#include <QSignalSpy>
#include "services/CableDiagnosticsService.h"

class CableDiagnosticsServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Verify single port test with signals and result fields
  void testTestPort() {
    CableDiagnosticsService svc;
    QSignalSpy startedSpy(&svc, &CableDiagnosticsService::testStarted);
    QSignalSpy completedSpy(&svc, &CableDiagnosticsService::testCompleted);
    auto result = svc.testPort(0);
    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(completedSpy.count(), 1);
    QCOMPARE(result.portId, 0);
    QCOMPARE(result.status, CableTestStatus::Passed);
    QCOMPARE(result.faultType, CableFaultType::None);
    QVERIFY(result.cableLengthM > 0);
    QVERIFY(result.impedanceOhms > 0);
    QVERIFY(result.signalQuality > 0);
  }
  // Verify all ports test completes with correct counts
  void testTestAllPorts() {
    CableDiagnosticsService svc;
    QSignalSpy spy(&svc, &CableDiagnosticsService::diagnosticsCompleted);
    auto report = svc.testAllPorts(4);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(report.results.size(), 4);
    QCOMPARE(report.passedCount, 4);
    QCOMPARE(report.failedCount, 0);
    QVERIFY(report.allPassed);
  }
  // Verify test history tracks multiple runs
  void testTestHistory() {
    CableDiagnosticsService svc;
    svc.testPort(0);
    svc.testPort(0);
    auto history = svc.testHistory(0);
    QCOMPARE(history.size(), 2);
  }
  // Verify last result returns most recent test
  void testLastResult() {
    CableDiagnosticsService svc;
    svc.testPort(1);
    auto last = svc.lastResult(1);
    QCOMPARE(last.portId, 1);
    QCOMPARE(last.status, CableTestStatus::Passed);
  }
  // Verify clear history removes entries
  void testClearHistory() {
    CableDiagnosticsService svc;
    svc.testPort(0);
    QVERIFY(svc.clearHistory(0));
    QCOMPARE(svc.testHistory(0).size(), 0);
  }
  // Verify clear returns false for nonexistent history
  void testClearNonexistentHistory() {
    CableDiagnosticsService svc;
    QVERIFY(!svc.clearHistory(99));
  }
  // Verify cable length varies by port index
  void testCableLengthVariesByPort() {
    CableDiagnosticsService svc;
    auto r0 = svc.testPort(0);
    auto r5 = svc.testPort(5);
    QVERIFY(r5.cableLengthM > r0.cableLengthM);
  }
};

QTEST_MAIN(CableDiagnosticsServiceTest)
#include "cable_diagnostics_service_test.moc"
