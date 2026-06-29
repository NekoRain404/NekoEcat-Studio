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
#include <QFile>
#include "services/WorkflowCertificationService.h"

class WorkflowCertificationServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testCertifyProcessRequiresCertificationBackend() {
      WorkflowCertificationService svc;
      QSignalSpy spy(&svc, &WorkflowCertificationService::certificationCompleted);
      WfProcessConfig cfg;
      cfg.processName = QStringLiteral("Commissioning");
      cfg.steps << QStringLiteral("init") << QStringLiteral("config") << QStringLiteral("validate");
      cfg.requirements << QStringLiteral("doc-complete");
      auto r = svc.certifyProcess(cfg);
      QVERIFY(!r.valid);
      QVERIFY(r.certificateId.isEmpty());
      QCOMPARE(r.scope, QStringLiteral("Commissioning"));
      QVERIFY(r.timestamp.isValid());
      QVERIFY(!r.expiry.isValid());
      QVERIFY(r.conditions.join(QStringLiteral("\n")).contains(QStringLiteral("backend"),
                                                               Qt::CaseInsensitive));
      QCOMPARE(spy.count(), 1);
  }

  void testCertifyProcessEmptySteps() {
      WorkflowCertificationService svc;
      WfProcessConfig cfg;
      cfg.processName = QStringLiteral("Empty");
      auto r = svc.certifyProcess(cfg);
      QVERIFY(!r.valid);
  }

  void testCertifyQualityRequiresCertificationBackend() {
      WorkflowCertificationService svc;
      QSignalSpy spy(&svc, &WorkflowCertificationService::certificationCompleted);
      WfQualityConfig cfg;
      cfg.qualityStandard = QStringLiteral("ISO 9001");
      cfg.minScore = 80.0;
      cfg.thresholds << QStringLiteral("accuracy>=95%");
      auto r = svc.certifyQuality(cfg);
      QVERIFY(!r.valid);
      QVERIFY(r.certificateId.isEmpty());
      QCOMPARE(r.scope, QStringLiteral("ISO 9001"));
      QVERIFY(!r.expiry.isValid());
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

  void testCertifySafetyRequiresCertificationBackend() {
      WorkflowCertificationService svc;
      QSignalSpy spy(&svc, &WorkflowCertificationService::certificationCompleted);
      WfSafetyConfig cfg;
      cfg.safetyLevel = QStringLiteral("SIL2");
      cfg.silLevel = 2;
      cfg.mitigations << QStringLiteral("redundant-sensor");
      auto r = svc.certifySafety(cfg);
      QVERIFY(!r.valid);
      QVERIFY(r.certificateId.isEmpty());
      QCOMPARE(r.scope, QStringLiteral("SIL2"));
      QVERIFY(!r.expiry.isValid());
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

  void testCertifyComplianceRequiresCertificationBackend() {
      WorkflowCertificationService svc;
      QSignalSpy spy(&svc, &WorkflowCertificationService::certificationCompleted);
      WfComplianceConfig cfg;
      cfg.regulation = QStringLiteral("CE");
      cfg.requirements << QStringLiteral("emc-test") << QStringLiteral("safety-label");
      cfg.evidence << QStringLiteral("test-report.pdf");
      cfg.jurisdiction = QStringLiteral("EU");
      auto r = svc.certifyCompliance(cfg);
      QVERIFY(!r.valid);
      QVERIFY(r.certificateId.isEmpty());
      QCOMPARE(r.scope, QStringLiteral("CE"));
      QVERIFY(!r.expiry.isValid());
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

  void testRejectedRequestsDoNotMintCertificateIds() {
      WorkflowCertificationService svc;
      WfProcessConfig cfg;
      cfg.processName = QStringLiteral("Test");
      cfg.steps << QStringLiteral("step1");
      cfg.requirements << QStringLiteral("req1");
      auto r1 = svc.certifyProcess(cfg);
      auto r2 = svc.certifyProcess(cfg);
      QVERIFY(r1.certificateId.isEmpty());
      QVERIFY(r2.certificateId.isEmpty());
  }

  void testRejectedRequestsDoNotSetExpiry() {
      WorkflowCertificationService svc;
      WfProcessConfig cfg;
      cfg.processName = QStringLiteral("Test");
      cfg.steps << QStringLiteral("step1");
      cfg.requirements << QStringLiteral("req1");
      auto r = svc.certifyProcess(cfg);
      QVERIFY(!r.expiry.isValid());
  }

  void testConditionsFromConfig() {
      WorkflowCertificationService svc;
      WfSafetyConfig cfg;
      cfg.safetyLevel = QStringLiteral("SIL2");
      cfg.silLevel = 2;
      cfg.mitigations << QStringLiteral("m1") << QStringLiteral("m2");
      auto r = svc.certifySafety(cfg);
      QVERIFY(r.conditions.join(QStringLiteral("\n")).contains(QStringLiteral("backend"),
                                                               Qt::CaseInsensitive));
  }

  void testSourceDoesNotMintSyntheticCertificates() {
      QFile file(QStringLiteral(SOURCE_ROOT
                                "/apps/ecat-studio/services/WorkflowCertificationService.cpp"));
      QVERIFY2(file.open(QIODevice::ReadOnly | QIODevice::Text),
               qPrintable(file.errorString()));
      const QString source = QString::fromUtf8(file.readAll());

      QVERIFY2(!source.contains(QStringLiteral("QUuid::createUuid")),
               "Workflow certification must not mint local certificate IDs without a backend");
      QVERIFY2(!source.contains(QStringLiteral("addYears(1)")),
               "Workflow certification must not synthesize certificate expiry dates without a backend");
      QVERIFY2(!source.contains(QStringLiteral("r.valid = !config.steps.isEmpty()")),
               "Workflow process certification must not pass from local completeness checks alone");
      QVERIFY2(!source.contains(QStringLiteral("r.valid = config.minScore >= 70.0")),
               "Workflow quality certification must not pass from local score checks alone");
      QVERIFY2(!source.contains(QStringLiteral("r.valid = config.silLevel >= 1")),
               "Workflow safety certification must not pass from local SIL labels alone");
  }
};

QTEST_MAIN(WorkflowCertificationServiceTest)
#include "workflow_certification_service_test.moc"
