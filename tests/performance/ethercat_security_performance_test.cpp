#include "services/EtherCATSecurityService.h"
#include <QElapsedTimer>
#include <QTest>

class EtherCATSecurityPerformanceTest : public QObject {
    Q_OBJECT
private slots:
    void testPolicySetPerformance() {
        EtherCATSecurityService svc;
        SecurityPolicy p;
        p.level = 1;
        p.encryptionEnabled = false;
        p.authenticationRequired = false;
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 10000; i++) {
            svc.setSecurityPolicy(p);
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 1000);
        qDebug() << "10000 setSecurityPolicy() calls:" << elapsed << "ms";
    }

    void testAuditPerformance() {
        EtherCATSecurityService svc;
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 10000; i++) {
            svc.runAudit();
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 1000);
        qDebug() << "10000 runAudit() calls:" << elapsed << "ms";
    }

    void testValidateAccessPerformance() {
        EtherCATSecurityService svc;
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 10000; i++) {
            svc.validateAccess("admin", "resource1");
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 1000);
        qDebug() << "10000 validateAccess() calls:" << elapsed << "ms";
    }

    void testRecentEventsPerformance() {
        EtherCATSecurityService svc;
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 10000; i++) {
            svc.recentEvents(10);
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 1000);
        qDebug() << "10000 recentEvents() calls:" << elapsed << "ms";
    }
};

QTEST_MAIN(EtherCATSecurityPerformanceTest)
#include "ethercat_security_performance_test.moc"
