// WorkflowSecurityManagerServiceTest — Tests for WorkflowSecurityManagerService
//
// Test coverage:
//   - Policy CRUD (add, remove, get)
//   - Policy enable/disable
//   - Policy enforcement
//   - Policy count and listing
//   - Signal emissions

#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowSecurityManagerService.h"

class WorkflowSecurityManagerServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testAddPolicy() {
      WorkflowSecurityManagerService svc;
      QSignalSpy spy(&svc, &WorkflowSecurityManagerService::policyAdded);
      auto id = svc.addPolicy(QStringLiteral("Auth Policy"),
                              QStringLiteral("Require authentication"),
                              QStringLiteral("high"));
      QVERIFY(!id.isEmpty());
      QCOMPARE(spy.count(), 1);
      QCOMPARE(svc.policyCount(), 1);
  }

  void testRemovePolicy() {
      WorkflowSecurityManagerService svc;
      auto id = svc.addPolicy(QStringLiteral("Test"), QStringLiteral("Desc"));
      QSignalSpy spy(&svc, &WorkflowSecurityManagerService::policyRemoved);
      QVERIFY(svc.removePolicy(id));
      QCOMPARE(spy.count(), 1);
      QCOMPARE(svc.policyCount(), 0);
  }

  void testRemoveNonexistentPolicy() {
      WorkflowSecurityManagerService svc;
      QVERIFY(!svc.removePolicy(QStringLiteral("nonexistent")));
  }

  void testEnableDisablePolicy() {
      WorkflowSecurityManagerService svc;
      auto id = svc.addPolicy(QStringLiteral("Test"), QStringLiteral("Desc"));
      QSignalSpy disableSpy(&svc, &WorkflowSecurityManagerService::policyDisabled);
      QVERIFY(svc.disablePolicy(id));
      QCOMPARE(disableSpy.count(), 1);

      QSignalSpy enableSpy(&svc, &WorkflowSecurityManagerService::policyEnabled);
      QVERIFY(svc.enablePolicy(id));
      QCOMPARE(enableSpy.count(), 1);
  }

  void testGetPolicy() {
      WorkflowSecurityManagerService svc;
      auto id = svc.addPolicy(QStringLiteral("Auth"), QStringLiteral("Desc"),
                              QStringLiteral("critical"));
      auto p = svc.policy(id);
      QCOMPARE(p.name, QStringLiteral("Auth"));
      QCOMPARE(p.severity, QStringLiteral("critical"));
      QVERIFY(p.enabled);
  }

  void testGetNonexistentPolicy() {
      WorkflowSecurityManagerService svc;
      auto p = svc.policy(QStringLiteral("bad"));
      QVERIFY(p.id.isEmpty());
  }

  void testAllPolicies() {
      WorkflowSecurityManagerService svc;
      svc.addPolicy(QStringLiteral("P1"), QStringLiteral("D1"));
      svc.addPolicy(QStringLiteral("P2"), QStringLiteral("D2"));
      svc.addPolicy(QStringLiteral("P3"), QStringLiteral("D3"));
      QCOMPARE(svc.allPolicies().size(), 3);
  }

  void testEnforcePolicy() {
      WorkflowSecurityManagerService svc;
      auto id = svc.addPolicy(QStringLiteral("Test"), QStringLiteral("Desc"));
      QSignalSpy spy(&svc, &WorkflowSecurityManagerService::policyEnforced);
      QVERIFY(svc.enforcePolicy(id));
      QCOMPARE(spy.count(), 1);
  }

  void testEnforceDisabledPolicy() {
      WorkflowSecurityManagerService svc;
      auto id = svc.addPolicy(QStringLiteral("Test"), QStringLiteral("Desc"));
      svc.disablePolicy(id);
      QVERIFY(!svc.enforcePolicy(id));
  }

  void testEnabledPolicyCount() {
      WorkflowSecurityManagerService svc;
      auto id1 = svc.addPolicy(QStringLiteral("P1"), QStringLiteral("D1"));
      svc.addPolicy(QStringLiteral("P2"), QStringLiteral("D2"));
      svc.addPolicy(QStringLiteral("P3"), QStringLiteral("D3"));
      svc.disablePolicy(id1);
      QCOMPARE(svc.enabledPolicyCount(), 2);
  }

  void testPolicyEnableNonexistent() {
      WorkflowSecurityManagerService svc;
      QVERIFY(!svc.enablePolicy(QStringLiteral("bad")));
  }

  void testPolicyDisableNonexistent() {
      WorkflowSecurityManagerService svc;
      QVERIFY(!svc.disablePolicy(QStringLiteral("bad")));
  }
};

QTEST_MAIN(WorkflowSecurityManagerServiceTest)
#include "workflow_security_manager_service_test.moc"
