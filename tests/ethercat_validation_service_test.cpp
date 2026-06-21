// EtherCATValidationServiceTest — Tests for EtherCATValidationService
//
// Test coverage:
//   - Configuration, network, timing, and safety validation
//   - Error count verification (zero errors)
//   - Validation details and recommendations
//   - Validation type identification

#include <QTest>
#include "services/EtherCATValidationService.h"

class EtherCATValidationServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Validate configuration passes with no errors
  // Validate configuration passes
  void testValidateConfiguration() {
    EtherCATValidationService svc;
    EtherCATValidationResult result = svc.validateConfiguration();
    QVERIFY(result.valid);
    QCOMPARE(result.errors.size(), 0);
  }

  // Validate network passes with no errors
  // Validate network passes
  void testValidateNetwork() {
    EtherCATValidationService svc;
    EtherCATValidationResult result = svc.validateNetwork();
    QVERIFY(result.valid);
    QCOMPARE(result.errors.size(), 0);
  }

  // Validate timing passes with no errors
  // Validate timing passes
  void testValidateTiming() {
    EtherCATValidationService svc;
    EtherCATValidationResult result = svc.validateTiming();
    QVERIFY(result.valid);
    QCOMPARE(result.errors.size(), 0);
  }

  // Validate safety passes with no errors
  // Validate safety passes
  void testValidateSafety() {
    EtherCATValidationService svc;
    EtherCATValidationResult result = svc.validateSafety();
    QVERIFY(result.valid);
    QCOMPARE(result.errors.size(), 0);
  }

  // Configuration validation returns zero errors
  // Configuration validation has no errors
  void testConfigurationNoErrors() {
    EtherCATValidationService svc;
    EtherCATValidationResult result = svc.validateConfiguration();
    QCOMPARE(result.errors.size(), 0);
  }

  // Network validation returns zero errors
  // Network validation has no errors
  void testNetworkNoErrors() {
    EtherCATValidationService svc;
    EtherCATValidationResult result = svc.validateNetwork();
    QCOMPARE(result.errors.size(), 0);
  }

  // Timing validation returns zero errors
  // Timing validation has no errors
  void testTimingNoErrors() {
    EtherCATValidationService svc;
    EtherCATValidationResult result = svc.validateTiming();
    QCOMPARE(result.errors.size(), 0);
  }

  // Safety validation returns zero errors
  // Safety validation has no errors
  void testSafetyNoErrors() {
    EtherCATValidationService svc;
    EtherCATValidationResult result = svc.validateSafety();
    QCOMPARE(result.errors.size(), 0);
  }

  // Configuration validation includes details
  // Configuration validation has details
  void testConfigurationDetails() {
    EtherCATValidationService svc;
    EtherCATValidationResult result = svc.validateConfiguration();
    QVERIFY(!result.details.isEmpty());
  }

  // Configuration validation includes recommendations
  // Configuration validation has recommendations
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
};

QTEST_MAIN(EtherCATValidationServiceTest)
#include "ethercat_validation_service_test.moc"
