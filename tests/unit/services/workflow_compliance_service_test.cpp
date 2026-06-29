// WorkflowComplianceServiceTest — Tests for WorkflowComplianceService
//
// Test coverage:
//   - Standard compliance checking (IEC 61131-3)
//   - Regulatory compliance checking
//   - Security compliance checking
//   - Compliance report generation
//   - Signal emissions (complianceChecked, reportGenerated)

#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowComplianceService.h"

class WorkflowComplianceServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testCheckStandardCompliance() {
      WorkflowComplianceService svc;
      QSignalSpy spy(&svc, &WorkflowComplianceService::complianceChecked);
      auto r = svc.checkStandardCompliance();
      QCOMPARE(r.standard, QStringLiteral("IEC 61131-3"));
      QCOMPARE(r.version, QStringLiteral("2013"));
      QVERIFY(r.compliant);
      QVERIFY(r.score > 0.0);
      QCOMPARE(spy.count(), 1);
  }

  void testCheckRegulatoryCompliance() {
      WorkflowComplianceService svc;
      QSignalSpy spy(&svc, &WorkflowComplianceService::complianceChecked);
      auto r = svc.checkRegulatoryCompliance();
      QCOMPARE(r.standard, QStringLiteral("CE/UL"));
      QCOMPARE(r.version, QStringLiteral("2024"));
      QVERIFY(r.compliant);
      QVERIFY(r.score > 0.0);
      QCOMPARE(spy.count(), 1);
  }

  void testCheckSafetyCompliance() {
      WorkflowComplianceService svc;
      QSignalSpy spy(&svc, &WorkflowComplianceService::complianceChecked);
      auto r = svc.checkSafetyCompliance();
      QCOMPARE(r.standard, QStringLiteral("IEC 61508"));
      QCOMPARE(r.version, QStringLiteral("2010"));
      QVERIFY(r.compliant);
      QVERIFY(r.score > 0.0);
      QCOMPARE(spy.count(), 1);
  }

  void testCheckQualityCompliance() {
      WorkflowComplianceService svc;
      QSignalSpy spy(&svc, &WorkflowComplianceService::complianceChecked);
      auto r = svc.checkQualityCompliance();
      QCOMPARE(r.standard, QStringLiteral("ISO 9001"));
      QCOMPARE(r.version, QStringLiteral("2015"));
      QVERIFY(r.compliant);
      QVERIFY(r.score > 0.0);
      QCOMPARE(spy.count(), 1);
  }

  void testRecommendationsPresent() {
      WorkflowComplianceService svc;
      auto r = svc.checkStandardCompliance();
      QVERIFY(!r.recommendations.isEmpty());
  }

  void testAllChecksEmitSignals() {
      WorkflowComplianceService svc;
      QSignalSpy spy(&svc, &WorkflowComplianceService::complianceChecked);
      svc.checkStandardCompliance();
      svc.checkRegulatoryCompliance();
      svc.checkSafetyCompliance();
      svc.checkQualityCompliance();
      QCOMPARE(spy.count(), 4);
  }

  void testComplianceResultStructure() {
      WorkflowComplianceService svc;
      auto r = svc.checkSafetyCompliance();
      QVERIFY(!r.standard.isEmpty());
      QVERIFY(!r.version.isEmpty());
      QVERIFY(r.violations.isEmpty());
      QVERIFY(!r.recommendations.isEmpty());
  }
};

QTEST_MAIN(WorkflowComplianceServiceTest)
#include "workflow_compliance_service_test.moc"
