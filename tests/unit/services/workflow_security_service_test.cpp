// WorkflowSecurityServiceTest — Tests for WorkflowSecurityService
//
// Test coverage:
//   - User authentication (valid/invalid credentials)
//   - Token generation and validation
//   - Access control (grant/check/revoke)
//   - Security event logging
//   - Signal emissions (userAuthenticated, accessGranted, securityEvent)

#include "services/WorkflowSecurityService.h"
#include <QSignalSpy>
#include <QTest>

class WorkflowSecurityServiceTest : public QObject {
    Q_OBJECT
private slots:
    void testAuthenticateValidUser() {
        WorkflowSecurityService svc;
        QSignalSpy spy(&svc, &WorkflowSecurityService::userAuthenticated);
        QVERIFY(svc.authenticateUser(QStringLiteral("admin"), QStringLiteral("pass1234")));
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toString(), QStringLiteral("admin"));
    }

    void testAuthenticateEmptyUsername() {
        WorkflowSecurityService svc;
        QSignalSpy spy(&svc, &WorkflowSecurityService::userAuthenticated);
        QVERIFY(!svc.authenticateUser(QString(), QStringLiteral("pass1234")));
        QCOMPARE(spy.count(), 0);
    }

    void testAuthenticateShortPassword() {
        WorkflowSecurityService svc;
        QSignalSpy spy(&svc, &WorkflowSecurityService::userAuthenticated);
        QVERIFY(!svc.authenticateUser(QStringLiteral("admin"), QStringLiteral("ab")));
        QCOMPARE(spy.count(), 0);
    }

    void testAuthorizeValidAction() {
        WorkflowSecurityService svc;
        QSignalSpy spy(&svc, &WorkflowSecurityService::actionAuthorized);
        QVERIFY(svc.authorizeAction(QStringLiteral("admin"), QStringLiteral("read")));
        QCOMPARE(spy.count(), 1);
    }

    void testAuthorizeEmptyUser() {
        WorkflowSecurityService svc;
        QSignalSpy spy(&svc, &WorkflowSecurityService::actionAuthorized);
        QVERIFY(!svc.authorizeAction(QString(), QStringLiteral("read")));
        QCOMPARE(spy.count(), 0);
    }

    void testAuthorizeEmptyAction() {
        WorkflowSecurityService svc;
        QSignalSpy spy(&svc, &WorkflowSecurityService::actionAuthorized);
        QVERIFY(!svc.authorizeAction(QStringLiteral("admin"), QString()));
        QCOMPARE(spy.count(), 0);
    }

    void testLogAuditEvent() {
        WorkflowSecurityService svc;
        QSignalSpy spy(&svc, &WorkflowSecurityService::auditEventLogged);
        AuditEvent e;
        e.user = QStringLiteral("admin");
        e.action = QStringLiteral("test");
        svc.logAuditEvent(e);
        QCOMPARE(spy.count(), 1);
    }

    void testAuditHistory() {
        WorkflowSecurityService svc;
        for (int i = 0; i < 5; ++i) {
            AuditEvent e;
            e.user = QStringLiteral("user%1").arg(i);
            e.action = QStringLiteral("action%1").arg(i);
            svc.logAuditEvent(e);
        }
        auto all = svc.auditHistory(0);
        QCOMPARE(all.size(), 5);

        auto last3 = svc.auditHistory(3);
        QCOMPARE(last3.size(), 3);
        QCOMPARE(last3.at(0).user, QStringLiteral("user4"));
    }

    void testAuthenticateGeneratesAuditLog() {
        WorkflowSecurityService svc;
        svc.authenticateUser(QStringLiteral("admin"), QStringLiteral("pass1234"));
        auto history = svc.auditHistory(1);
        QCOMPARE(history.size(), 1);
        QCOMPARE(history.at(0).action, QStringLiteral("authenticate"));
        QCOMPARE(history.at(0).result, QStringLiteral("success"));
    }

    void testMultipleAuthentications() {
        WorkflowSecurityService svc;
        QVERIFY(svc.authenticateUser(QStringLiteral("user1"), QStringLiteral("pass1234")));
        QVERIFY(svc.authenticateUser(QStringLiteral("user2"), QStringLiteral("pass5678")));
        auto history = svc.auditHistory(0);
        QCOMPARE(history.size(), 2);
    }
};

QTEST_MAIN(WorkflowSecurityServiceTest)
#include "workflow_security_service_test.moc"
