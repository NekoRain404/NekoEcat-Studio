// WorkflowComplianceManagerServiceTest — Tests for WorkflowComplianceManagerService
//
// Test coverage:
//   - Rule CRUD (add, remove, get)
//   - Rule activate/deactivate
//   - Rule audit
//   - Category filtering
//   - Signal emissions

#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowComplianceManagerService.h"

class WorkflowComplianceManagerServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testAddRule() {
      WorkflowComplianceManagerService svc;
      QSignalSpy spy(&svc, &WorkflowComplianceManagerService::ruleAdded);
      auto id = svc.addRule(QStringLiteral("IEC Compliance"),
                            QStringLiteral("safety"),
                            QStringLiteral("IEC 61508 SIL2"));
      QVERIFY(!id.isEmpty());
      QCOMPARE(spy.count(), 1);
      QCOMPARE(svc.ruleCount(), 1);
  }

  void testRemoveRule() {
      WorkflowComplianceManagerService svc;
      auto id = svc.addRule(QStringLiteral("Test"), QStringLiteral("cat"),
                            QStringLiteral("req"));
      QSignalSpy spy(&svc, &WorkflowComplianceManagerService::ruleRemoved);
      QVERIFY(svc.removeRule(id));
      QCOMPARE(spy.count(), 1);
      QCOMPARE(svc.ruleCount(), 0);
  }

  void testRemoveNonexistentRule() {
      WorkflowComplianceManagerService svc;
      QVERIFY(!svc.removeRule(QStringLiteral("nonexistent")));
  }

  void testActivateDeactivateRule() {
      WorkflowComplianceManagerService svc;
      auto id = svc.addRule(QStringLiteral("Test"), QStringLiteral("cat"),
                            QStringLiteral("req"));
      QSignalSpy deactivateSpy(&svc, &WorkflowComplianceManagerService::ruleDeactivated);
      QVERIFY(svc.deactivateRule(id));
      QCOMPARE(deactivateSpy.count(), 1);

      QSignalSpy activateSpy(&svc, &WorkflowComplianceManagerService::ruleActivated);
      QVERIFY(svc.activateRule(id));
      QCOMPARE(activateSpy.count(), 1);
  }

  void testGetRule() {
      WorkflowComplianceManagerService svc;
      auto id = svc.addRule(QStringLiteral("Quality Rule"),
                            QStringLiteral("quality"),
                            QStringLiteral("ISO 9001"));
      auto r = svc.rule(id);
      QCOMPARE(r.name, QStringLiteral("Quality Rule"));
      QCOMPARE(r.category, QStringLiteral("quality"));
      QVERIFY(r.active);
  }

  void testGetNonexistentRule() {
      WorkflowComplianceManagerService svc;
      auto r = svc.rule(QStringLiteral("bad"));
      QVERIFY(r.id.isEmpty());
  }

  void testAllRules() {
      WorkflowComplianceManagerService svc;
      svc.addRule(QStringLiteral("R1"), QStringLiteral("c1"), QStringLiteral("req1"));
      svc.addRule(QStringLiteral("R2"), QStringLiteral("c2"), QStringLiteral("req2"));
      QCOMPARE(svc.allRules().size(), 2);
  }

  void testActiveRuleCount() {
      WorkflowComplianceManagerService svc;
      auto id1 = svc.addRule(QStringLiteral("R1"), QStringLiteral("c"), QStringLiteral("r1"));
      svc.addRule(QStringLiteral("R2"), QStringLiteral("c"), QStringLiteral("r2"));
      svc.deactivateRule(id1);
      QCOMPARE(svc.activeRuleCount(), 1);
  }

  void testAuditRule() {
      WorkflowComplianceManagerService svc;
      auto id = svc.addRule(QStringLiteral("Test"), QStringLiteral("cat"),
                            QStringLiteral("req"));
      QSignalSpy spy(&svc, &WorkflowComplianceManagerService::ruleAudited);
      QVERIFY(svc.auditRule(id));
      QCOMPARE(spy.count(), 1);
      auto r = svc.rule(id);
      QVERIFY(r.lastAudit.isValid());
  }

  void testAuditNonexistentRule() {
      WorkflowComplianceManagerService svc;
      QVERIFY(!svc.auditRule(QStringLiteral("bad")));
  }

  void testRulesByCategory() {
      WorkflowComplianceManagerService svc;
      svc.addRule(QStringLiteral("R1"), QStringLiteral("safety"), QStringLiteral("req1"));
      svc.addRule(QStringLiteral("R2"), QStringLiteral("quality"), QStringLiteral("req2"));
      svc.addRule(QStringLiteral("R3"), QStringLiteral("safety"), QStringLiteral("req3"));
      auto safetyRules = svc.rulesByCategory(QStringLiteral("safety"));
      QCOMPARE(safetyRules.size(), 2);
      auto qualityRules = svc.rulesByCategory(QStringLiteral("quality"));
      QCOMPARE(qualityRules.size(), 1);
  }

  void testActivateNonexistent() {
      WorkflowComplianceManagerService svc;
      QVERIFY(!svc.activateRule(QStringLiteral("bad")));
  }
};

QTEST_MAIN(WorkflowComplianceManagerServiceTest)
#include "workflow_compliance_manager_service_test.moc"
