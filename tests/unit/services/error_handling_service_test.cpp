// ErrorHandlingServiceTest — Tests for ErrorHandlingService
//
// Test coverage:
//   - Error reporting and signal emission
//   - Error detection (active errors only)
//   - Error classification (transient, persistent, fatal)
//   - Error recovery (success and fatal failure)
//   - Error history and recovery action
//   - ID increment and max history limit

#include <QTest>
#include <QSignalSpy>
#include "services/ErrorHandlingService.h"

class ErrorHandlingServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Verify reportError returns positive ID and emits errorDetected
  void testReportError() {
    ErrorHandlingService svc;
    QSignalSpy spy(&svc, &ErrorHandlingService::errorDetected);
    int id = svc.reportError(0, 0x1234, "Test error",
                             EcatErrorSeverity::Error,
                             EcatErrorCategory::Communication);
    QVERIFY(id > 0);
    QCOMPARE(spy.count(), 1);
  }

  // Verify detectErrors returns only Error and Critical severity
  // Verify detectErrors returns only Error and Critical severity
  void testDetectErrors() {
    ErrorHandlingService svc;
    svc.reportError(0, 1, "info", EcatErrorSeverity::Info,
                    EcatErrorCategory::Communication);
    svc.reportError(0, 2, "warning", EcatErrorSeverity::Warning,
                    EcatErrorCategory::Device);
    svc.reportError(0, 3, "error", EcatErrorSeverity::Error,
                    EcatErrorCategory::Network);
    svc.reportError(0, 4, "critical", EcatErrorSeverity::Critical,
                    EcatErrorCategory::Protocol);
    auto active = svc.detectErrors();
    QCOMPARE(active.size(), 2);
  }

  // Verify Warning severity classifies as Transient
  // Verify Warning severity is classified as Transient
  void testClassifyTransient() {
    ErrorHandlingService svc;
    EcatErrorInfo e;
    e.severity = EcatErrorSeverity::Warning;
    QCOMPARE(svc.classifyError(e), EcatErrorClass::Transient);
  }

  // Verify Error severity with Communication classifies as Persistent
  // Verify Error severity with Communication category is Persistent
  void testClassifyPersistent() {
    ErrorHandlingService svc;
    EcatErrorInfo e;
    e.severity = EcatErrorSeverity::Error;
    e.category = EcatErrorCategory::Communication;
    QCOMPARE(svc.classifyError(e), EcatErrorClass::Persistent);
  }

  // Verify Critical severity classifies as Fatal
  // Verify Critical severity is classified as Fatal
  void testClassifyFatal() {
    ErrorHandlingService svc;
    EcatErrorInfo e;
    e.severity = EcatErrorSeverity::Critical;
    QCOMPARE(svc.classifyError(e), EcatErrorClass::Fatal);
  }

  // Verify recoverable error emits errorRecovered signal
  // Verify recoverable error returns true and emits errorRecovered
  void testRecoverFromError() {
    ErrorHandlingService svc;
    QSignalSpy spy(&svc, &ErrorHandlingService::errorRecovered);
    int id = svc.reportError(0, 1, "recoverable", EcatErrorSeverity::Error,
                             EcatErrorCategory::Network);
    auto errors = svc.errorHistory();
    QCOMPARE(errors.size(), 1);
    QVERIFY(svc.recoverFromError(errors[0]));
    QCOMPARE(spy.count(), 1);
  }

  // Verify fatal error cannot be recovered from
  // Verify fatal error cannot be recovered from
  void testRecoverFromFatalFails() {
    ErrorHandlingService svc;
    int id = svc.reportError(0, 1, "fatal", EcatErrorSeverity::Critical,
                             EcatErrorCategory::Protocol);
    auto errors = svc.errorHistory();
    QVERIFY(!svc.recoverFromError(errors[0]));
  }

  // Verify error history preserves order and messages
  // Verify error history preserves order and messages
  void testErrorHistory() {
    ErrorHandlingService svc;
    svc.reportError(0, 1, "first", EcatErrorSeverity::Error,
                    EcatErrorCategory::Communication);
    svc.reportError(1, 2, "second", EcatErrorSeverity::Warning,
                    EcatErrorCategory::Device);
    auto h = svc.errorHistory();
    QCOMPARE(h.size(), 2);
    QCOMPARE(h[0].errorMessage, "first");
    QCOMPARE(h[1].errorMessage, "second");
  }

  // Verify recovery action is stored in error info
  // Verify recovery action is stored in error history
  void testRecoveryAction() {
    ErrorHandlingService svc;
    svc.reportError(0, 1, "test", EcatErrorSeverity::Error,
                    EcatErrorCategory::Configuration, "Reset device");
    auto h = svc.errorHistory();
    QCOMPARE(h[0].recoveryAction, "Reset device");
  }

  // Verify error IDs are auto-incremented
  // Verify error IDs are monotonically increasing
  void testIdIncrement() {
    ErrorHandlingService svc;
    int id1 = svc.reportError(0, 1, "a", EcatErrorSeverity::Info,
                              EcatErrorCategory::Communication);
    int id2 = svc.reportError(0, 2, "b", EcatErrorSeverity::Info,
                              EcatErrorCategory::Communication);
    QVERIFY(id2 > id1);
  }

  // Verify error history caps at 1000 entries
  // Verify error history is capped at 1000 entries
  void testMaxHistory() {
    ErrorHandlingService svc;
    for (int i = 0; i < 1100; ++i) {
      svc.reportError(0, i, "msg", EcatErrorSeverity::Info,
                      EcatErrorCategory::Communication);
    }
    QCOMPARE(svc.errorHistory().size(), 1000);
  }
};

QTEST_MAIN(ErrorHandlingServiceTest)
#include "error_handling_service_test.moc"
