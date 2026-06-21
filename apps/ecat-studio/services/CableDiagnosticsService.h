#pragma once

// CableDiagnosticsService — performs cable diagnostics on EtherCAT network.
// Tests cable quality, length, and detects faults.
//
// DEMO STUB — This service generates synthetic data for UI demonstration.
// Replace with real hardware integration for production use.
//
// This service provides cable diagnostic capabilities for the EtherCAT
// network. It handles:
//   - Cable quality testing per port
//   - Cable length measurement
//   - Fault detection (Open, Short, Impedance, Crosstalk)
//   - Signal quality assessment
//   - Test history tracking
//   - Batch testing of all ports
//
// Usage:
//   CableDiagnosticsService cableDiag;
//   CableTestResult result = cableDiag.testPort(0);
//   if (result.status == CableTestStatus::Passed) {
//     double length = result.cableLengthM;
//     double quality = result.signalQuality;
//   }
//   CableDiagnosticsReport report = cableDiag.testAllPorts(4);
//   QVector<CableTestResult> history = cableDiag.testHistory(0);
//   CableTestResult last = cableDiag.lastResult(0);
//   cableDiag.clearHistory(0);
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. Cable
//   testing is synchronous and blocks the calling thread.
//
// Performance:
//   - Single port test is O(1)
//   - All ports test is O(n) where n is number of ports
//   - History retrieval is O(n) where n is history size

#include <QObject>
#include <QVector>
#include <QDateTime>

// Cable test status enumeration.
enum class CableTestStatus { 
  NotRun,  // Test has not been run
  Running, // Test is currently running
  Passed,  // Test passed
  Failed,  // Test failed
  Error    // Test error
};

// Cable fault type enumeration.
enum class CableFaultType { 
  None,       // No fault detected
  Open,       // Open circuit fault
  Short,      // Short circuit fault
  Impedance,  // Impedance mismatch
  Crosstalk,  // Crosstalk fault
  Unknown     // Unknown fault type
};

// Cable test result structure.
struct CableTestResult {
  int portId = 0;                                    // Port ID
  CableTestStatus status = CableTestStatus::NotRun;  // Test status
  CableFaultType faultType = CableFaultType::None;   // Fault type
  double cableLengthM = 0.0;                         // Cable length in meters
  double impedanceOhms = 0.0;                        // Impedance in ohms
  double signalQuality = 0.0;                        // Signal quality (0.0-1.0)
  QDateTime timestamp;                               // Test timestamp
  QString details;                                   // Test details
};

// Cable diagnostics report structure.
struct CableDiagnosticsReport {
  QDateTime timestamp;                    // Report timestamp
  QVector<CableTestResult> results;       // Test results per port
  int passedCount = 0;                    // Number of passed tests
  int failedCount = 0;                    // Number of failed tests
  bool allPassed = false;                 // Whether all tests passed
};

class CableDiagnosticsService : public QObject {
  Q_OBJECT
public:
  explicit CableDiagnosticsService(QObject *parent = nullptr);

  // Test a specific port.
  // @param portId  Port ID to test
  // @return CableTestResult structure
  CableTestResult testPort(int portId);

  // Test all ports.
  // @param portCount  Number of ports to test
  // @return CableDiagnosticsReport with results
  CableDiagnosticsReport testAllPorts(int portCount);

  // Get test history for a specific port.
  // @param portId  Port ID
  // @return Vector of CableTestResult structures
  QVector<CableTestResult> testHistory(int portId) const;

  // Get the last test result for a specific port.
  // @param portId  Port ID
  // @return CableTestResult structure
  CableTestResult lastResult(int portId) const;

  // Clear test history for a specific port.
  // @param portId  Port ID
  // @return true if history was cleared
  bool clearHistory(int portId);

signals:
  // Emitted when a test starts.
  // @param portId  Port ID being tested
  void testStarted(int portId);

  // Emitted when a test completes.
  // @param portId  Port ID tested
  // @param result  CableTestResult structure
  void testCompleted(int portId, const CableTestResult &result);

  // Emitted when all port diagnostics complete.
  // @param report  CableDiagnosticsReport with results
  void diagnosticsCompleted(const CableDiagnosticsReport &report);

private:
  // Simulate a cable test (for demo/testing).
  CableTestResult simulateTest(int portId);

  QHash<int, QVector<CableTestResult>> history_;  // Test history per port
  QHash<int, CableTestResult> lastResults_;        // Last result per port
  static constexpr int kMaxHistory = 100;          // Maximum history entries
};
