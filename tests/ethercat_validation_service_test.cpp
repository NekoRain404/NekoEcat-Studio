// EtherCATValidationServiceTest — Tests for EtherCATValidationService
//
// Test coverage:
//   - Configuration, network, timing, and safety validation
//   - Error count verification (zero errors)
//   - Validation details and recommendations
//   - Validation type identification

#include <QTest>
#include <QSignalSpy>
#include <QFile>
#include "services/EtherCATValidationService.h"

class EtherCATValidationServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Configuration validation fails closed without a real validation backend.
  void testValidateConfigurationFailsClosedWithoutBackend() {
    EtherCATValidationService svc;
    QSignalSpy spy(&svc, &EtherCATValidationService::validationCompleted);
    EtherCATValidationResult result = svc.validateConfiguration();
    QVERIFY(!result.valid);
    QVERIFY(!result.details.isEmpty());
    QCOMPARE(spy.count(), 0);
  }

  // Network validation fails closed without a real validation backend.
  void testValidateNetworkFailsClosedWithoutBackend() {
    EtherCATValidationService svc;
    EtherCATValidationResult result = svc.validateNetwork();
    QVERIFY(!result.valid);
    QCOMPARE(result.validationType, QString("Network"));
  }

  // Timing validation fails closed without a real validation backend.
  void testValidateTimingFailsClosedWithoutBackend() {
    EtherCATValidationService svc;
    EtherCATValidationResult result = svc.validateTiming();
    QVERIFY(!result.valid);
    QCOMPARE(result.validationType, QString("Timing"));
  }

  // Safety validation fails closed without a real validation backend.
  void testValidateSafetyFailsClosedWithoutBackend() {
    EtherCATValidationService svc;
    EtherCATValidationResult result = svc.validateSafety();
    QVERIFY(!result.valid);
    QCOMPARE(result.validationType, QString("Safety"));
  }

  // Configuration validation records an error explaining why it could not run.
  void testConfigurationReportsRejectedError() {
    EtherCATValidationService svc;
    EtherCATValidationResult result = svc.validateConfiguration();
    QVERIFY(result.errors.size() > 0);
    QCOMPARE(result.errors.first().code, QString("VALIDATION_BACKEND_REQUIRED"));
  }

  // Network validation records an error explaining why it could not run.
  void testNetworkReportsRejectedError() {
    EtherCATValidationService svc;
    EtherCATValidationResult result = svc.validateNetwork();
    QVERIFY(result.errors.size() > 0);
  }

  // Timing validation records an error explaining why it could not run.
  void testTimingReportsRejectedError() {
    EtherCATValidationService svc;
    EtherCATValidationResult result = svc.validateTiming();
    QVERIFY(result.errors.size() > 0);
  }

  // Safety validation records an error explaining why it could not run.
  void testSafetyReportsRejectedError() {
    EtherCATValidationService svc;
    EtherCATValidationResult result = svc.validateSafety();
    QVERIFY(result.errors.size() > 0);
  }

  // Configuration validation includes details
  // Configuration validation has details
  void testConfigurationDetails() {
    EtherCATValidationService svc;
    EtherCATValidationResult result = svc.validateConfiguration();
    QVERIFY(!result.details.isEmpty());
  }

  // Configuration validation includes recommendations
  void testConfigurationRecommendations() {
    EtherCATValidationService svc;
    EtherCATValidationResult result = svc.validateConfiguration();
    QVERIFY(result.recommendations.size() > 0);
  }

  // Validation type is "Configuration"
  // Validation type is Configuration
  void testValidationTypeConfiguration() {
    EtherCATValidationService svc;
    EtherCATValidationResult result = svc.validateConfiguration();
    QCOMPARE(result.validationType, QString("Configuration"));
  }

  // Validation type is "Network"
  // Validation type is Network
  void testValidationTypeNetwork() {
    EtherCATValidationService svc;
    EtherCATValidationResult result = svc.validateNetwork();
    QCOMPARE(result.validationType, QString("Network"));
  }

  // Implementation must not keep synthetic validation success paths.
  void testImplementationDoesNotContainSyntheticValidationSuccessPath() {
    QFile source(QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/services/EtherCATValidationService.cpp"));
    QVERIFY2(source.open(QIODevice::ReadOnly | QIODevice::Text),
             qPrintable(source.errorString()));
    const QString text = QString::fromUtf8(source.readAll());

    QVERIFY2(!text.contains(QStringLiteral("createPassingResult")),
             "Validation service must not use a passing result helper without evidence.");
    QVERIFY2(!text.contains(QStringLiteral("result.valid = true")),
             "Validation service must not synthesize valid results.");
    QVERIFY2(!text.contains(QStringLiteral("validation passed")),
             "Validation service must not synthesize passed detail text.");
    QVERIFY2(!text.contains(QStringLiteral("No issues found")),
             "Validation service must not synthesize no-issue recommendations.");
    QVERIFY2(!text.contains(QStringLiteral("emit validationCompleted(result)")),
             "Validation service must not emit validation completion for rejected checks.");
  }
};

QTEST_MAIN(EtherCATValidationServiceTest)
#include "ethercat_validation_service_test.moc"
