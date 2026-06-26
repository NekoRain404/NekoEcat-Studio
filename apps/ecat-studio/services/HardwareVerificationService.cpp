#include "HardwareVerificationService.h"
#include "infra/EcatClient.h"

HardwareVerificationService::HardwareVerificationService(EcatClient *client,
                                                         QObject *parent)
    : QObject(parent), client_(client) {}

VerificationResult HardwareVerificationService::verifyDevice(int position) {
  VerificationResult result;
  result.verificationId = QStringLiteral("device_%1").arg(position);
  result.verificationName =
      QStringLiteral("Device Verification (Position %1)").arg(position);

  if (!verificationBackendReady()) {
    QVector<TestResult> tests = {
        runDeviceIdentification(position),
        runDeviceCapability(position),
        runDevicePerformance(position),
        runDeviceCompliance(position),
    };
    return offlineResult(result.verificationId, result.verificationName, tests);
  }

  emit verificationStarted(result.verificationId);

  auto t1 = runDeviceIdentification(position);
  result.tests.append(t1);
  t1.passed ? ++result.passed : ++result.failed;
  emit verificationProgress(result.verificationId, 1, 4);

  auto t2 = runDeviceCapability(position);
  result.tests.append(t2);
  t2.passed ? ++result.passed : ++result.failed;
  emit verificationProgress(result.verificationId, 2, 4);

  auto t3 = runDevicePerformance(position);
  result.tests.append(t3);
  t3.passed ? ++result.passed : ++result.failed;
  emit verificationProgress(result.verificationId, 3, 4);

  auto t4 = runDeviceCompliance(position);
  result.tests.append(t4);
  t4.passed ? ++result.passed : ++result.failed;
  emit verificationProgress(result.verificationId, 4, 4);

  result.totalDurationMs = t1.durationMs + t2.durationMs + t3.durationMs +
                           t4.durationMs;
  results_.append(result);
  emit verificationCompleted(result);
  return result;
}

VerificationResult HardwareVerificationService::verifyNetwork() {
  VerificationResult result;
  result.verificationId = QStringLiteral("network");
  result.verificationName = QStringLiteral("Network Verification");

  if (!verificationBackendReady()) {
    QVector<TestResult> tests = {
        runLinkQuality(),
        runCableQuality(),
        runSignalQuality(),
        runErrorRate(),
    };
    return offlineResult(result.verificationId, result.verificationName, tests);
  }

  emit verificationStarted(result.verificationId);

  auto t1 = runLinkQuality();
  result.tests.append(t1);
  t1.passed ? ++result.passed : ++result.failed;
  emit verificationProgress(result.verificationId, 1, 4);

  auto t2 = runCableQuality();
  result.tests.append(t2);
  t2.passed ? ++result.passed : ++result.failed;
  emit verificationProgress(result.verificationId, 2, 4);

  auto t3 = runSignalQuality();
  result.tests.append(t3);
  t3.passed ? ++result.passed : ++result.failed;
  emit verificationProgress(result.verificationId, 3, 4);

  auto t4 = runErrorRate();
  result.tests.append(t4);
  t4.passed ? ++result.passed : ++result.failed;
  emit verificationProgress(result.verificationId, 4, 4);

  result.totalDurationMs = t1.durationMs + t2.durationMs + t3.durationMs +
                           t4.durationMs;
  results_.append(result);
  emit verificationCompleted(result);
  return result;
}

VerificationResult HardwareVerificationService::verifyTiming() {
  VerificationResult result;
  result.verificationId = QStringLiteral("timing");
  result.verificationName = QStringLiteral("Timing Verification");

  if (!verificationBackendReady()) {
    QVector<TestResult> tests = {
        runDcSyncTiming(),
        runProcessDataTiming(),
        runMailboxTiming(),
        runCycleTime(),
    };
    return offlineResult(result.verificationId, result.verificationName, tests);
  }

  emit verificationStarted(result.verificationId);

  auto t1 = runDcSyncTiming();
  result.tests.append(t1);
  t1.passed ? ++result.passed : ++result.failed;
  emit verificationProgress(result.verificationId, 1, 4);

  auto t2 = runProcessDataTiming();
  result.tests.append(t2);
  t2.passed ? ++result.passed : ++result.failed;
  emit verificationProgress(result.verificationId, 2, 4);

  auto t3 = runMailboxTiming();
  result.tests.append(t3);
  t3.passed ? ++result.passed : ++result.failed;
  emit verificationProgress(result.verificationId, 3, 4);

  auto t4 = runCycleTime();
  result.tests.append(t4);
  t4.passed ? ++result.passed : ++result.failed;
  emit verificationProgress(result.verificationId, 4, 4);

  result.totalDurationMs = t1.durationMs + t2.durationMs + t3.durationMs +
                           t4.durationMs;
  results_.append(result);
  emit verificationCompleted(result);
  return result;
}

VerificationResult HardwareVerificationService::verifyCompliance() {
  VerificationResult result;
  result.verificationId = QStringLiteral("compliance");
  result.verificationName = QStringLiteral("Compliance Verification");

  if (!verificationBackendReady()) {
    QVector<TestResult> tests = {
        runProtocolCompliance(),
        runStateMachineCompliance(),
        runSdoCompliance(),
        runPdoCompliance(),
    };
    return offlineResult(result.verificationId, result.verificationName, tests);
  }

  emit verificationStarted(result.verificationId);

  auto t1 = runProtocolCompliance();
  result.tests.append(t1);
  t1.passed ? ++result.passed : ++result.failed;
  emit verificationProgress(result.verificationId, 1, 4);

  auto t2 = runStateMachineCompliance();
  result.tests.append(t2);
  t2.passed ? ++result.passed : ++result.failed;
  emit verificationProgress(result.verificationId, 2, 4);

  auto t3 = runSdoCompliance();
  result.tests.append(t3);
  t3.passed ? ++result.passed : ++result.failed;
  emit verificationProgress(result.verificationId, 3, 4);

  auto t4 = runPdoCompliance();
  result.tests.append(t4);
  t4.passed ? ++result.passed : ++result.failed;
  emit verificationProgress(result.verificationId, 4, 4);

  result.totalDurationMs = t1.durationMs + t2.durationMs + t3.durationMs +
                           t4.durationMs;
  results_.append(result);
  emit verificationCompleted(result);
  return result;
}

QVector<VerificationResult> HardwareVerificationService::allResults() const {
  return results_;
}

void HardwareVerificationService::clearResults() { results_.clear(); }

bool HardwareVerificationService::verificationBackendReady() const {
  return false;
}

VerificationResult HardwareVerificationService::offlineResult(
    const QString &verificationId, const QString &verificationName,
    const QVector<TestResult> &tests) {
  VerificationResult result;
  result.verificationId = verificationId;
  result.verificationName = verificationName;
  result.recommendations.append(
      client_ && client_->isConnected()
          ? QStringLiteral("Hardware verification requires a real verification backend; daemon connectivity alone is not enough.")
          : QStringLiteral("Connect to the EtherCAT daemon before running hardware verification."));

  emit verificationStarted(result.verificationId);
  for (TestResult test : tests) {
    test.passed = false;
    test.skipped = true;
    test.durationMs = 0.0;
    test.details =
        client_ && client_->isConnected()
            ? QStringLiteral("Skipped: hardware verification backend is not available.")
            : QStringLiteral("Skipped: EtherCAT daemon is not connected.");
    test.recommendation =
        client_ && client_->isConnected()
            ? QStringLiteral("Wire a real hardware verification backend and rerun this verification.")
            : QStringLiteral("Connect to the daemon and rerun this verification.");
    result.tests.append(test);
    ++result.skipped;
    emit verificationProgress(result.verificationId, result.tests.size(),
                              tests.size());
  }

  results_.append(result);
  emit verificationCompleted(result);
  return result;
}

// ── Device test stubs ──────────────────────────────────────────────────

TestResult HardwareVerificationService::runDeviceIdentification(int position) {
  TestResult r;
  r.testId = QStringLiteral("dev_id_%1").arg(position);
  r.testName = QStringLiteral("Device Identification");
  r.category = QStringLiteral("Device");
  r.passed = true;
  r.durationMs = 12.0;
  r.details = QStringLiteral("Slave %1: vendor/product/version read OK")
                  .arg(position);
  return r;
}

TestResult HardwareVerificationService::runDeviceCapability(int position) {
  TestResult r;
  r.testId = QStringLiteral("dev_cap_%1").arg(position);
  r.testName = QStringLiteral("Device Capability");
  r.category = QStringLiteral("Device");
  r.passed = true;
  r.durationMs = 25.0;
  r.details = QStringLiteral("Slave %1: supported modes and features verified")
                  .arg(position);
  return r;
}

TestResult HardwareVerificationService::runDevicePerformance(int position) {
  TestResult r;
  r.testId = QStringLiteral("dev_perf_%1").arg(position);
  r.testName = QStringLiteral("Device Performance");
  r.category = QStringLiteral("Device");
  r.passed = true;
  r.durationMs = 50.0;
  r.details = QStringLiteral("Slave %1: response time within specification")
                  .arg(position);
  return r;
}

TestResult HardwareVerificationService::runDeviceCompliance(int position) {
  TestResult r;
  r.testId = QStringLiteral("dev_comp_%1").arg(position);
  r.testName = QStringLiteral("Device Compliance");
  r.category = QStringLiteral("Device");
  r.passed = true;
  r.durationMs = 30.0;
  r.details = QStringLiteral("Slave %1: EtherCAT device profile compliant")
                  .arg(position);
  return r;
}

// ── Network test stubs ─────────────────────────────────────────────────

TestResult HardwareVerificationService::runLinkQuality() {
  TestResult r;
  r.testId = QStringLiteral("net_link");
  r.testName = QStringLiteral("Link Quality");
  r.category = QStringLiteral("Network");
  r.passed = true;
  r.durationMs = 15.0;
  r.details = QStringLiteral("All ports: link established, no errors detected");
  return r;
}

TestResult HardwareVerificationService::runCableQuality() {
  TestResult r;
  r.testId = QStringLiteral("net_cable");
  r.testName = QStringLiteral("Cable Quality");
  r.category = QStringLiteral("Network");
  r.passed = true;
  r.durationMs = 20.0;
  r.details = QStringLiteral("Cable integrity: all segments within spec");
  return r;
}

TestResult HardwareVerificationService::runSignalQuality() {
  TestResult r;
  r.testId = QStringLiteral("net_signal");
  r.testName = QStringLiteral("Signal Quality");
  r.category = QStringLiteral("Network");
  r.passed = true;
  r.durationMs = 18.0;
  r.details = QStringLiteral("Signal levels: nominal across all ports");
  return r;
}

TestResult HardwareVerificationService::runErrorRate() {
  TestResult r;
  r.testId = QStringLiteral("net_error");
  r.testName = QStringLiteral("Error Rate");
  r.category = QStringLiteral("Network");
  r.passed = true;
  r.durationMs = 30.0;
  r.details = QStringLiteral("Frame error rate: 0 (threshold < 0.001%%)");
  return r;
}

// ── Timing test stubs ──────────────────────────────────────────────────

TestResult HardwareVerificationService::runDcSyncTiming() {
  TestResult r;
  r.testId = QStringLiteral("time_dcsync");
  r.testName = QStringLiteral("DC Sync Timing");
  r.category = QStringLiteral("Timing");
  r.passed = true;
  r.durationMs = 100.0;
  r.details = QStringLiteral("DC sync jitter: < 1us (specification: < 10us)");
  return r;
}

TestResult HardwareVerificationService::runProcessDataTiming() {
  TestResult r;
  r.testId = QStringLiteral("time_pd");
  r.testName = QStringLiteral("Process Data Timing");
  r.category = QStringLiteral("Timing");
  r.passed = true;
  r.durationMs = 80.0;
  r.details =
      QStringLiteral("PDO exchange latency: nominal within cycle time");
  return r;
}

TestResult HardwareVerificationService::runMailboxTiming() {
  TestResult r;
  r.testId = QStringLiteral("time_mbx");
  r.testName = QStringLiteral("Mailbox Timing");
  r.category = QStringLiteral("Timing");
  r.passed = true;
  r.durationMs = 40.0;
  r.details = QStringLiteral("Mailbox response time: within specification");
  return r;
}

TestResult HardwareVerificationService::runCycleTime() {
  TestResult r;
  r.testId = QStringLiteral("time_cycle");
  r.testName = QStringLiteral("Cycle Time");
  r.category = QStringLiteral("Timing");
  r.passed = true;
  r.durationMs = 60.0;
  r.details = QStringLiteral("Bus cycle time: stable, no overruns detected");
  return r;
}

// ── Compliance test stubs ──────────────────────────────────────────────

TestResult HardwareVerificationService::runProtocolCompliance() {
  TestResult r;
  r.testId = QStringLiteral("comp_proto");
  r.testName = QStringLiteral("Protocol Compliance");
  r.category = QStringLiteral("Compliance");
  r.passed = true;
  r.durationMs = 35.0;
  r.details = QStringLiteral("EtherCAT protocol layer: compliant with IEC 61158");
  return r;
}

TestResult HardwareVerificationService::runStateMachineCompliance() {
  TestResult r;
  r.testId = QStringLiteral("comp_sm");
  r.testName = QStringLiteral("State Machine Compliance");
  r.category = QStringLiteral("Compliance");
  r.passed = true;
  r.durationMs = 45.0;
  r.details = QStringLiteral("State transitions: all slaves follow EtherCAT state machine");
  return r;
}

TestResult HardwareVerificationService::runSdoCompliance() {
  TestResult r;
  r.testId = QStringLiteral("comp_sdo");
  r.testName = QStringLiteral("SDO Compliance");
  r.category = QStringLiteral("Compliance");
  r.passed = true;
  r.durationMs = 55.0;
  r.details = QStringLiteral("SDO services: read/write/info operations compliant");
  return r;
}

TestResult HardwareVerificationService::runPdoCompliance() {
  TestResult r;
  r.testId = QStringLiteral("comp_pdo");
  r.testName = QStringLiteral("PDO Compliance");
  r.category = QStringLiteral("Compliance");
  r.passed = true;
  r.durationMs = 50.0;
  r.details = QStringLiteral("PDO mapping and exchange: compliant with specification");
  return r;
}
