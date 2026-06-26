#include "CableDiagnosticsService.h"
#include <QtMath>

// CableDiagnosticsService.cpp — Cable quality testing and fault detection
//
// Implementation notes:
//   - Per-port and all-port test runs fail closed until a real physical cable
//     diagnostics backend is wired.
//   - Maintains per-port test history capped at kMaxHistory once backend results
//     are available.

CableDiagnosticsService::CableDiagnosticsService(QObject *parent)
    : QObject(parent) {}

CableTestResult CableDiagnosticsService::testPort(int portId) {
  emit testStarted(portId);
  auto result = backendUnavailableResult(portId);
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

CableTestResult CableDiagnosticsService::backendUnavailableResult(int portId) {
  CableTestResult result;
  result.portId = portId;
  result.timestamp = QDateTime::currentDateTime();
  result.status = CableTestStatus::Error;
  result.faultType = CableFaultType::Unknown;
  result.details = "Cable diagnostics require a connected physical cable test backend";
  return result;
}
