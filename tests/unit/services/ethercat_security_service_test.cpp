// EtherCATSecurityServiceTest — Tests for EtherCATSecurityService
//
// Test coverage:
//   - Default security policy (level, encryption, auth, ACL)
//   - Custom policy configuration
//   - Security audit execution
//   - Access control validation
//   - Recent events retrieval
//   - Signal emission for policy change and audit

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATSecurityService.h"

class EtherCATSecurityServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Verify default security policy settings
  // Default policy has encryption and auth enabled
  void testDefaultPolicy() {
    EtherCATSecurityService svc;
    SecurityPolicy p = svc.currentPolicy();
    QCOMPARE(p.level, 2);
    QVERIFY(p.encryptionEnabled);
    QVERIFY(p.authenticationRequired);
    QCOMPARE(p.accessControlList.size(), 2);
    QCOMPARE(p.accessControlList.at(0), QString("admin"));
    QCOMPARE(p.accessControlList.at(1), QString("operator"));
  }

  // Set custom policy and verify all fields
  // Set custom security policy
  void testSetPolicy() {
    EtherCATSecurityService svc;
    SecurityPolicy p;
    p.level = 3;
    p.encryptionEnabled = false;
    p.authenticationRequired = false;
    p.accessControlList.append("user1");
    svc.setSecurityPolicy(p);
    SecurityPolicy q = svc.currentPolicy();
    QCOMPARE(q.level, 3);
    QVERIFY(!q.encryptionEnabled);
    QVERIFY(!q.authenticationRequired);
    QCOMPARE(q.accessControlList.size(), 1);
    QCOMPARE(q.accessControlList.at(0), QString("user1"));
  }

  // Run security audit and verify entries
  // Run security audit returns events
  void testRunAudit() {
    EtherCATSecurityService svc;
    SecurityAuditResult result = svc.runAudit();
    QVERIFY(result.totalEvents > 0);
    QVERIFY(result.entries.size() > 0);
  }

  // Validate access for known and unknown users
  // Validate access for admin, operator, unknown
  void testValidateAccess() {
    EtherCATSecurityService svc;
    QVERIFY(svc.validateAccess("admin", "resource1"));
    QVERIFY(svc.validateAccess("operator", "resource1"));
    QVERIFY(!svc.validateAccess("unknown", "resource1"));
  }

  // Retrieve recent security events
  // Recent events include system_start
  void testRecentEvents() {
    EtherCATSecurityService svc;
    QVector<SecurityAuditEntry> events = svc.recentEvents(10);
    QVERIFY(events.size() > 0);
    QCOMPARE(events.first().eventType, QString("system_start"));
  }

  // Verify policyChanged signal emission
  // policyChanged signal fires on set
  void testPolicyChangedSignal() {
    EtherCATSecurityService svc;
    QSignalSpy spy(&svc, &EtherCATSecurityService::policyChanged);
    SecurityPolicy p;
    p.level = 1;
    svc.setSecurityPolicy(p);
    QCOMPARE(spy.count(), 1);
  }

  // Verify auditCompleted signal emission
  // auditCompleted signal fires on audit
  void testAuditCompletedSignal() {
    EtherCATSecurityService svc;
    QSignalSpy spy(&svc, &EtherCATSecurityService::auditCompleted);
    svc.runAudit();
    QCOMPARE(spy.count(), 1);
  }
};

QTEST_MAIN(EtherCATSecurityServiceTest)
#include "ethercat_security_service_test.moc"
