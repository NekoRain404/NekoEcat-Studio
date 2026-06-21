#pragma once

// HardwareVerificationService — verifies EtherCAT hardware devices and network.
//
// Provides device-level, network-wide, timing, and compliance verification
// tests for EtherCAT hardware. Reports pass/fail results with recommendations.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>
#include <QVector>

class EcatClient;

struct TestResult {
  QString testId;
  QString testName;
  QString category;
  bool passed = false;
  bool skipped = false;
  double durationMs = 0.0;
  QString details;
  QString recommendation;
};

struct VerificationResult {
  QString verificationId;
  QString verificationName;
  int passed = 0;
  int failed = 0;
  int skipped = 0;
  double totalDurationMs = 0.0;
  QVector<TestResult> tests;
  QStringList recommendations;

  bool allPassed() const { return failed == 0 && skipped == 0; }
  int totalTests() const { return passed + failed + skipped; }
};

class HardwareVerificationService : public QObject {
  Q_OBJECT
public:
  explicit HardwareVerificationService(EcatClient *client,
                                       QObject *parent = nullptr);

  VerificationResult verifyDevice(int position);
  VerificationResult verifyNetwork();
  VerificationResult verifyTiming();
  VerificationResult verifyCompliance();

  QVector<VerificationResult> allResults() const;
  void clearResults();

signals:
  void verificationStarted(const QString &verificationId);
  void verificationCompleted(const VerificationResult &result);
  void verificationProgress(const QString &verificationId, int current,
                            int total);

private:
  TestResult runDeviceIdentification(int position);
  TestResult runDeviceCapability(int position);
  TestResult runDevicePerformance(int position);
  TestResult runDeviceCompliance(int position);
  TestResult runLinkQuality();
  TestResult runCableQuality();
  TestResult runSignalQuality();
  TestResult runErrorRate();
  TestResult runDcSyncTiming();
  TestResult runProcessDataTiming();
  TestResult runMailboxTiming();
  TestResult runCycleTime();
  TestResult runProtocolCompliance();
  TestResult runStateMachineCompliance();
  TestResult runSdoCompliance();
  TestResult runPdoCompliance();

  EcatClient *client_;
  QVector<VerificationResult> results_;
};
