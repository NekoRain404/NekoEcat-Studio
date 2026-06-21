#include "CableDiagnosticsService.h"
#include <QtMath>

// CableDiagnosticsService.cpp — Cable quality testing and fault detection
//
// Implementation notes:
//   - Supports per-port and all-port test runs with aggregated reports
//   - Maintains per-port test history capped at kMaxHistory
//   - simulateTest() generates synthetic results for offline/demo use

CableDiagnosticsService::CableDiagnosticsService(QObject *parent)
    : QObject(parent) {}

CableTestResult CableDiagnosticsService::testPort(int portId) {
  emit testStarted(portId);
  auto result = simulateTest(portId);
  history_[portId].append(result);
  if (history_[portId].size() > kMaxHistory)
    history_[portId].removeFirst();
  lastResults_[portId] = result;
  emit testCompleted(portId, result);
  return result;
}

// Runs tests across all ports and returns an aggregated pass/fail report
CableDiagnosticsReport CableDiagnosticsService::testAllPorts(int portCount) {
  CableDiagnosticsReport report;
  report.timestamp = QDateTime::currentDateTime();
  for (int i = 0; i < portCount; i++) {
    auto result = testPort(i);
    report.results.append(result);
    if (result.status == CableTestStatus::Passed)
      report.passedCount++;
    else
      report.failedCount++;
  }
  report.allPassed = (report.failedCount == 0);
  emit diagnosticsCompleted(report);
  return report;
}

QVector<CableTestResult> CableDiagnosticsService::testHistory(int portId) const {
  return history_.value(portId);
}

CableTestResult CableDiagnosticsService::lastResult(int portId) const {
  return lastResults_.value(portId);
}

bool CableDiagnosticsService::clearHistory(int portId) {
  if (!history_.contains(portId))
    return false;
  history_.remove(portId);
  lastResults_.remove(portId);
  return true;
}

// Generates synthetic cable test data based on port ID (demo/offline mode)
CableTestResult CableDiagnosticsService::simulateTest(int portId) {
  CableTestResult result;
  result.portId = portId;
  result.timestamp = QDateTime::currentDateTime();
  result.status = CableTestStatus::Passed;
  result.faultType = CableFaultType::None;
  result.cableLengthM = 5.0 + (portId * 0.5);
  result.impedanceOhms = 100.0 + (portId % 3);
  result.signalQuality = 95.0 - (portId * 0.5);
  result.details = "Cable test passed";
  return result;
}
