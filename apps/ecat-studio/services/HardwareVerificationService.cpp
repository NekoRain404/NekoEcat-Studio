#include "HardwareVerificationService.h"
#include "infra/EcatClient.h"

HardwareVerificationService::HardwareVerificationService(EcatClient* client, QObject* parent)
    : QObject(parent), client_(client) {}

VerificationResult HardwareVerificationService::verifyDevice(int position) {
    const QString verificationId = QStringLiteral("device_%1").arg(position);
    const QString verificationName = QStringLiteral("Device Verification (Position %1)").arg(position);
    QVector<TestResult> tests = {
        runDeviceIdentification(position),
        runDeviceCapability(position),
        runDevicePerformance(position),
        runDeviceCompliance(position),
    };
    return offlineResult(verificationId, verificationName, tests);
}

VerificationResult HardwareVerificationService::verifyNetwork() {
    QVector<TestResult> tests = {
        runLinkQuality(),
        runCableQuality(),
        runSignalQuality(),
        runErrorRate(),
    };
    return offlineResult(QStringLiteral("network"), QStringLiteral("Network Verification"), tests);
}

VerificationResult HardwareVerificationService::verifyTiming() {
    QVector<TestResult> tests = {
        runDcSyncTiming(),
        runProcessDataTiming(),
        runMailboxTiming(),
        runCycleTime(),
    };
    return offlineResult(QStringLiteral("timing"), QStringLiteral("Timing Verification"), tests);
}

VerificationResult HardwareVerificationService::verifyCompliance() {
    QVector<TestResult> tests = {
        runProtocolCompliance(),
        runStateMachineCompliance(),
        runSdoCompliance(),
        runPdoCompliance(),
    };
    return offlineResult(QStringLiteral("compliance"), QStringLiteral("Compliance Verification"), tests);
}

QVector<VerificationResult> HardwareVerificationService::allResults() const {
    return results_;
}

void HardwareVerificationService::clearResults() {
    results_.clear();
}

VerificationResult HardwareVerificationService::offlineResult(const QString& verificationId,
                                                              const QString& verificationName,
                                                              const QVector<TestResult>& tests) {
    VerificationResult result;
    result.verificationId = verificationId;
    result.verificationName = verificationName;
    result.recommendations.append(
        client_ && client_->isConnected()
            ? QStringLiteral("Hardware verification requires a real verification backend; daemon connectivity alone is "
                             "not enough.")
            : QStringLiteral("Connect to the EtherCAT daemon before running hardware verification."));

    emit verificationStarted(result.verificationId);
    for (TestResult test : tests) {
        test.passed = false;
        test.skipped = true;
        test.durationMs = 0.0;
        test.details = client_ && client_->isConnected()
                           ? QStringLiteral("Skipped: hardware verification backend is not available.")
                           : QStringLiteral("Skipped: EtherCAT daemon is not connected.");
        test.recommendation =
            client_ && client_->isConnected()
                ? QStringLiteral("Wire a real hardware verification backend and rerun this verification.")
                : QStringLiteral("Connect to the daemon and rerun this verification.");
        result.tests.append(test);
        ++result.skipped;
        emit verificationProgress(result.verificationId, result.tests.size(), tests.size());
    }

    results_.append(result);
    emit verificationCompleted(result);
    return result;
}

// ── Verification check descriptors ─────────────────────────────────────

TestResult HardwareVerificationService::runDeviceIdentification(int position) {
    TestResult r;
    r.testId = QStringLiteral("dev_id_%1").arg(position);
    r.testName = QStringLiteral("Device Identification");
    r.category = QStringLiteral("Device");
    r.details = QStringLiteral("Slave %1: vendor/product/version read").arg(position);
    return r;
}

TestResult HardwareVerificationService::runDeviceCapability(int position) {
    TestResult r;
    r.testId = QStringLiteral("dev_cap_%1").arg(position);
    r.testName = QStringLiteral("Device Capability");
    r.category = QStringLiteral("Device");
    r.details = QStringLiteral("Slave %1: supported modes and features check").arg(position);
    return r;
}

TestResult HardwareVerificationService::runDevicePerformance(int position) {
    TestResult r;
    r.testId = QStringLiteral("dev_perf_%1").arg(position);
    r.testName = QStringLiteral("Device Performance");
    r.category = QStringLiteral("Device");
    r.details = QStringLiteral("Slave %1: response time measurement").arg(position);
    return r;
}

TestResult HardwareVerificationService::runDeviceCompliance(int position) {
    TestResult r;
    r.testId = QStringLiteral("dev_comp_%1").arg(position);
    r.testName = QStringLiteral("Device Compliance");
    r.category = QStringLiteral("Device");
    r.details = QStringLiteral("Slave %1: EtherCAT device profile check").arg(position);
    return r;
}

TestResult HardwareVerificationService::runLinkQuality() {
    TestResult r;
    r.testId = QStringLiteral("net_link");
    r.testName = QStringLiteral("Link Quality");
    r.category = QStringLiteral("Network");
    r.details = QStringLiteral("All ports: link quality check");
    return r;
}

TestResult HardwareVerificationService::runCableQuality() {
    TestResult r;
    r.testId = QStringLiteral("net_cable");
    r.testName = QStringLiteral("Cable Quality");
    r.category = QStringLiteral("Network");
    r.details = QStringLiteral("Cable integrity check");
    return r;
}

TestResult HardwareVerificationService::runSignalQuality() {
    TestResult r;
    r.testId = QStringLiteral("net_signal");
    r.testName = QStringLiteral("Signal Quality");
    r.category = QStringLiteral("Network");
    r.details = QStringLiteral("Signal level measurement");
    return r;
}

TestResult HardwareVerificationService::runErrorRate() {
    TestResult r;
    r.testId = QStringLiteral("net_error");
    r.testName = QStringLiteral("Error Rate");
    r.category = QStringLiteral("Network");
    r.details = QStringLiteral("Frame error rate measurement");
    return r;
}

TestResult HardwareVerificationService::runDcSyncTiming() {
    TestResult r;
    r.testId = QStringLiteral("time_dcsync");
    r.testName = QStringLiteral("DC Sync Timing");
    r.category = QStringLiteral("Timing");
    r.details = QStringLiteral("DC sync jitter measurement");
    return r;
}

TestResult HardwareVerificationService::runProcessDataTiming() {
    TestResult r;
    r.testId = QStringLiteral("time_pd");
    r.testName = QStringLiteral("Process Data Timing");
    r.category = QStringLiteral("Timing");
    r.details = QStringLiteral("PDO exchange latency measurement");
    return r;
}

TestResult HardwareVerificationService::runMailboxTiming() {
    TestResult r;
    r.testId = QStringLiteral("time_mbx");
    r.testName = QStringLiteral("Mailbox Timing");
    r.category = QStringLiteral("Timing");
    r.details = QStringLiteral("Mailbox response time measurement");
    return r;
}

TestResult HardwareVerificationService::runCycleTime() {
    TestResult r;
    r.testId = QStringLiteral("time_cycle");
    r.testName = QStringLiteral("Cycle Time");
    r.category = QStringLiteral("Timing");
    r.details = QStringLiteral("Bus cycle time stability check");
    return r;
}

TestResult HardwareVerificationService::runProtocolCompliance() {
    TestResult r;
    r.testId = QStringLiteral("comp_proto");
    r.testName = QStringLiteral("Protocol Compliance");
    r.category = QStringLiteral("Compliance");
    r.details = QStringLiteral("EtherCAT protocol layer compliance check");
    return r;
}

TestResult HardwareVerificationService::runStateMachineCompliance() {
    TestResult r;
    r.testId = QStringLiteral("comp_sm");
    r.testName = QStringLiteral("State Machine Compliance");
    r.category = QStringLiteral("Compliance");
    r.details = QStringLiteral("State transition compliance check");
    return r;
}

TestResult HardwareVerificationService::runSdoCompliance() {
    TestResult r;
    r.testId = QStringLiteral("comp_sdo");
    r.testName = QStringLiteral("SDO Compliance");
    r.category = QStringLiteral("Compliance");
    r.details = QStringLiteral("SDO service compliance check");
    return r;
}

TestResult HardwareVerificationService::runPdoCompliance() {
    TestResult r;
    r.testId = QStringLiteral("comp_pdo");
    r.testName = QStringLiteral("PDO Compliance");
    r.category = QStringLiteral("Compliance");
    r.details = QStringLiteral("PDO mapping and exchange compliance check");
    return r;
}
