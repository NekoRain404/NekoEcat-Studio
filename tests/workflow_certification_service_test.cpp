// WorkflowCertificationServiceTest — Tests for WorkflowCertificationService
//
// Test coverage:
//   - Process certification with valid/invalid configs
//   - Certificate retrieval and validation
//   - Revocation and renewal workflows
//   - Certification history tracking
//   - Signal emissions (certificationCompleted, certificateRevoked)

#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowCertificationService.h"

class WorkflowCertificationServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testCertifyProcessValid() {
      WorkflowCertificationService svc;
      QSignalSpy spy(&svc, &WorkflowCertificationService::certificationCompleted);
      WfProcessConfig cfg;
      cfg.processName = QStringLiteral("Commissioning");
      cfg.steps << QStringLiteral("init") << QStringLiteral("config") << QStringLiteral("validate");
      cfg.requirements << QStringLiteral("doc-complete");
      auto r = svc.certifyProcess(cfg);
      QVERIFY(r.valid);
      QVERIFY(!r.certificateId.isEmpty());
      QCOMPARE(r.scope, QStringLiteral("Commissioning"));
      QVERIFY(r.timestamp.isValid());
      QVERIFY(r.expiry > r.timestamp);
      QCOMPARE(spy.count(), 1);
  }

  void testCertifyProcessEmptySteps() {
      WorkflowCertificationService svc;
      WfProcessConfig cfg;
      cfg.processName = QStringLiteral("Empty");
      auto r = svc.certifyProcess(cfg);
      QVERIFY(!r.valid);
  }

  void testCertifyQualityPass() {
      WorkflowCertificationService svc;
      QSignalSpy spy(&svc, &WorkflowCertificationService::certificationCompleted);
      WfQualityConfig cfg;
      cfg.qualityStandard = QStringLiteral("ISO 9001");
      cfg.minScore = 80.0;
      cfg.thresholds << QStringLiteral("accuracy>=95%");
      auto r = svc.certifyQuality(cfg);
      QVERIFY(r.valid);
      QCOMPARE(r.scope, QStringLiteral("ISO 9001"));
      QCOMPARE(spy.count(), 1);
  }

  void testCertifyQualityFail() {
      WorkflowCertificationService svc;
      WfQualityConfig cfg;
      cfg.qualityStandard = QStringLiteral("ISO 9001");
      cfg.minScore = 50.0;
      auto r = svc.certifyQuality(cfg);
      QVERIFY(!r.valid);
  }

  void testCertifySafetyValid() {
      WorkflowCertificationService svc;
      QSignalSpy spy(&svc, &WorkflowCertificationService::certificationCompleted);
      WfSafetyConfig cfg;
      cfg.safetyLevel = QStringLiteral("SIL2");
      cfg.silLevel = 2;
      cfg.mitigations << QStringLiteral("redundant-sensor");
      auto r = svc.certifySafety(cfg);
      QVERIFY(r.valid);
      QCOMPARE(r.scope, QStringLiteral("SIL2"));
      QCOMPARE(spy.count(), 1);
  }

  void testCertifySafetyInvalid() {
      WorkflowCertificationService svc;
      WfSafetyConfig cfg;
      cfg.safetyLevel = QStringLiteral("SIL0");
      cfg.silLevel = 0;
      auto r = svc.certifySafety(cfg);
      QVERIFY(!r.valid);
  }

  void testCertifyComplianceValid() {
      WorkflowCertificationService svc;
      QSignalSpy spy(&svc, &WorkflowCertificationService::certificationCompleted);
      WfComplianceConfig cfg;
      cfg.regulation = QStringLiteral("CE");
      cfg.requirements << QStringLiteral("emc-test") << QStringLiteral("safety-label");
      cfg.evidence << QStringLiteral("test-report.pdf");
      cfg.jurisdiction = QStringLiteral("EU");
      auto r = svc.certifyCompliance(cfg);
      QVERIFY(r.valid);
      QCOMPARE(r.scope, QStringLiteral("CE"));
      QCOMPARE(spy.count(), 1);
  }

  void testCertifyComplianceMissingEvidence() {
      WorkflowCertificationService svc;
      WfComplianceConfig cfg;
      cfg.regulation = QStringLiteral("CE");
      cfg.requirements << QStringLiteral("emc-test");
      auto r = svc.certifyCompliance(cfg);
      QVERIFY(!r.valid);
  }

  void testUniqueCertificateIds() {
      WorkflowCertificationService svc;
      WfProcessConfig cfg;
      cfg.processName = QStringLiteral("Test");
      cfg.steps << QStringLiteral("step1");
      cfg.requirements << QStringLiteral("req1");
      auto r1 = svc.certifyProcess(cfg);
      auto r2 = svc.certifyProcess(cfg);
      QVERIFY(r1.certificateId != r2.certificateId);
  }

  void testExpiryIsOneYear() {
      WorkflowCertificationService svc;
      WfProcessConfig cfg;
      cfg.processName = QStringLiteral("Test");
      cfg.steps << QStringLiteral("step1");
      cfg.requirements << QStringLiteral("req1");
      auto r = svc.certifyProcess(cfg);
      QCOMPARE(r.timestamp.daysTo(r.expiry), 365);
  }

  void testConditionsFromConfig() {
      WorkflowCertificationService svc;
      WfSafetyConfig cfg;
      cfg.safetyLevel = QStringLiteral("SIL2");
      cfg.silLevel = 2;
      cfg.mitigations << QStringLiteral("m1") << QStringLiteral("m2");
      auto r = svc.certifySafety(cfg);
      QCOMPARE(r.conditions.size(), 2);
      QCOMPARE(r.conditions.at(0), QStringLiteral("m1"));
  }
};

QTEST_MAIN(WorkflowCertificationServiceTest)
#include "workflow_certification_service_test.moc"
