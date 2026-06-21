// EtherCATCertificationServiceTest — Tests for EtherCATCertificationService
//
// Test coverage:
//   - Requirement management (add, remove, defaults)
//   - Certification execution and per-requirement testing
//   - Device, network, system, and operator certification
//   - Certificate metadata (ID, timestamp, expiry, conditions)

#include <QTest>
#include <QSignalSpy>
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

  // Run full certification and verify pass
  void testRunCertification() {
    EtherCATCertificationService svc;
    CertificationReport report = svc.runCertification();
    QVERIFY(report.overallPass);
    QCOMPARE(report.passedCount, report.totalRequirements);
  }

  // Test individual requirement passes
  void testTestRequirement() {
    EtherCATCertificationService svc;
    CertificationTestResult result = svc.testRequirement("CONF-001");
    QCOMPARE(result.status, CertificationTestStatus::Pass);
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

  // Certify a device by position
  void testCertifyDevice() {
    EtherCATCertificationService svc;
    CertificationResult result = svc.certifyDevice(1);
    QVERIFY(result.valid);
    QCOMPARE(result.scope, QString("Device"));
  }

  // Certify the network
  void testCertifyNetwork() {
    EtherCATCertificationService svc;
    CertificationResult result = svc.certifyNetwork();
    QVERIFY(result.valid);
    QCOMPARE(result.scope, QString("Network"));
  }

  // Certify the system
  void testCertifySystem() {
    EtherCATCertificationService svc;
    CertificationResult result = svc.certifySystem();
    QVERIFY(result.valid);
    QCOMPARE(result.scope, QString("System"));
  }

  // Certify an operator by name
  void testCertifyOperator() {
    EtherCATCertificationService svc;
    CertificationResult result = svc.certifyOperator("engineer1");
    QVERIFY(result.valid);
    QCOMPARE(result.scope, QString("Operator"));
  }

  // Certificate result has non-empty ID
  void testCertResultCertificateId() {
    EtherCATCertificationService svc;
    CertificationResult result = svc.certifyDevice(0);
    QVERIFY(!result.certificateId.isEmpty());
  }

  // Certificate result has valid timestamp
  void testCertResultTimestamp() {
    EtherCATCertificationService svc;
    CertificationResult result = svc.certifyNetwork();
    QVERIFY(result.timestamp.isValid());
  }

  // Certificate expiry is after timestamp
  void testCertResultExpiry() {
    EtherCATCertificationService svc;
    CertificationResult result = svc.certifySystem();
    QVERIFY(result.expiry.isValid());
    QVERIFY(result.expiry > result.timestamp);
  }

  // Certificate has conditions attached
  void testCertResultConditions() {
    EtherCATCertificationService svc;
    CertificationResult result = svc.certifyDevice(0);
    QVERIFY(result.conditions.size() > 0);
  }

  // deviceCertified signal fires on certification
  void testDeviceCertifiedSignal() {
    EtherCATCertificationService svc;
    QSignalSpy spy(&svc, &EtherCATCertificationService::deviceCertified);
    svc.certifyNetwork();
    QCOMPARE(spy.size(), 1);
  }
};

QTEST_MAIN(EtherCATCertificationServiceTest)
#include "ethercat_certification_service_test.moc"
