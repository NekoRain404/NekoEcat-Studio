// EtherCATCertificationServiceTest — Tests for EtherCATCertificationService
//
// Test coverage:
//   - Requirement management (add, remove, defaults)
//   - Certification execution and per-requirement testing
//   - Device, network, system, and operator certification
//   - Certificate metadata (ID, timestamp, expiry, conditions)

#include <QTest>
#include <QSignalSpy>
#include <QFile>
#include "services/EtherCATCertificationService.h"

class EtherCATCertificationServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Add a new certification requirement
  void testAddRequirement() {
    EtherCATCertificationService svc;
    CertificationRequirement req;
    req.requirementId = "TEST-001";
    req.category = "Test";
    req.description = "Test requirement.";
    svc.addRequirement(req);
    QVERIFY(svc.requirements().size() > 4);
  }

  // Remove existing requirement
  void testRemoveRequirement() {
    EtherCATCertificationService svc;
    QVERIFY(svc.removeRequirement("CONF-001"));
    QCOMPARE(svc.requirements().size(), 3);
  }

  // Remove nonexistent requirement returns false
  void testRemoveNonExistent() {
    EtherCATCertificationService svc;
    QVERIFY(!svc.removeRequirement("NONEXISTENT"));
  }

  // Run full certification must not synthesize passing evidence.
  void testRunCertificationFailsClosedWithoutBackend() {
    EtherCATCertificationService svc;
    QSignalSpy spy(&svc, &EtherCATCertificationService::certificationCompleted);
    CertificationReport report = svc.runCertification();
    QVERIFY(!report.overallPass);
    QCOMPARE(report.passedCount, 0);
    QCOMPARE(report.failedCount, 0);
    QCOMPARE(report.notTestedCount, report.totalRequirements);
    QVERIFY(report.certificationLevel.isEmpty());
    QCOMPARE(spy.count(), 0);
  }

  // Individual requirement tests must not synthesize pass evidence.
  void testRequirementNotTestedWithoutBackend() {
    EtherCATCertificationService svc;
    CertificationTestResult result = svc.testRequirement("CONF-001");
    QCOMPARE(result.status, CertificationTestStatus::NotTested);
    QVERIFY(result.evidence.isEmpty());
  }

  // Test nonexistent requirement returns NotTested
  void testTestRequirementNotFound() {
    EtherCATCertificationService svc;
    CertificationTestResult result = svc.testRequirement("NONEXISTENT");
    QCOMPARE(result.status, CertificationTestStatus::NotTested);
  }

  // Default requirements count is 4
  void testDefaultRequirements() {
    EtherCATCertificationService svc;
    QCOMPARE(svc.requirements().size(), 4);
  }

  // Certify a device by position fails closed without certification evidence.
  void testCertifyDeviceFailsClosedWithoutBackend() {
    EtherCATCertificationService svc;
    QSignalSpy spy(&svc, &EtherCATCertificationService::deviceCertified);
    CertificationResult result = svc.certifyDevice(1);
    QVERIFY(!result.valid);
    QCOMPARE(result.scope, QString("Device"));
    QVERIFY(result.certificateId.isEmpty());
    QCOMPARE(spy.count(), 0);
  }

  // Certify the network fails closed without certification evidence.
  void testCertifyNetworkFailsClosedWithoutBackend() {
    EtherCATCertificationService svc;
    CertificationResult result = svc.certifyNetwork();
    QVERIFY(!result.valid);
    QCOMPARE(result.scope, QString("Network"));
  }

  // Certify the system fails closed without certification evidence.
  void testCertifySystemFailsClosedWithoutBackend() {
    EtherCATCertificationService svc;
    CertificationResult result = svc.certifySystem();
    QVERIFY(!result.valid);
    QCOMPARE(result.scope, QString("System"));
  }

  // Certify an operator fails closed without certification evidence.
  void testCertifyOperatorFailsClosedWithoutBackend() {
    EtherCATCertificationService svc;
    CertificationResult result = svc.certifyOperator("engineer1");
    QVERIFY(!result.valid);
    QCOMPARE(result.scope, QString("Operator"));
  }

  // Rejected certificate result has no certificate ID.
  void testRejectedCertResultHasNoCertificateId() {
    EtherCATCertificationService svc;
    CertificationResult result = svc.certifyDevice(0);
    QVERIFY(result.certificateId.isEmpty());
  }

  // Rejected certificate result still records a request timestamp.
  void testCertResultTimestamp() {
    EtherCATCertificationService svc;
    CertificationResult result = svc.certifyNetwork();
    QVERIFY(result.timestamp.isValid());
  }

  // Rejected certificate result has no expiry.
  void testRejectedCertResultHasNoExpiry() {
    EtherCATCertificationService svc;
    CertificationResult result = svc.certifySystem();
    QVERIFY(!result.expiry.isValid());
  }

  // Rejected certificate explains why no certificate was issued.
  void testRejectedCertResultConditions() {
    EtherCATCertificationService svc;
    CertificationResult result = svc.certifyDevice(0);
    QVERIFY(result.conditions.size() > 0);
  }

  // deviceCertified signal is not emitted for rejected certification requests.
  void testDeviceCertifiedSignalNotEmittedWithoutBackend() {
    EtherCATCertificationService svc;
    QSignalSpy spy(&svc, &EtherCATCertificationService::deviceCertified);
    svc.certifyNetwork();
    QCOMPARE(spy.size(), 0);
  }

  // Implementation must not keep synthetic pass/certificate paths.
  void testImplementationDoesNotContainSyntheticCertificationSuccessPath() {
    QFile source(QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/services/EtherCATCertificationService.cpp"));
    QVERIFY2(source.open(QIODevice::ReadOnly | QIODevice::Text),
             qPrintable(source.errorString()));
    const QString text = QString::fromUtf8(source.readAll());

    QVERIFY2(!text.contains(QStringLiteral("CertificationTestStatus::Pass")),
             "Certification tests must not synthesize passing results without evidence.");
    QVERIFY2(!text.contains(QStringLiteral("overallPass = true")),
             "Certification report must not synthesize overall pass.");
    QVERIFY2(!text.contains(QStringLiteral("certificationLevel = QStringLiteral(\"Gold\")")),
             "Certification report must not synthesize a certification level.");
    QVERIFY2(!text.contains(QStringLiteral("valid = true")),
             "Certification result must not synthesize valid certificates.");
    QVERIFY2(!text.contains(QStringLiteral("emit deviceCertified(result)")),
             "Certification service must not emit certificate success without evidence.");
  }
};

QTEST_MAIN(EtherCATCertificationServiceTest)
#include "ethercat_certification_service_test.moc"
