#include "services/WorkflowComplianceManagerService.h"
#include <QElapsedTimer>
#include <QTest>

class WorkflowComplianceManagerPerformanceTest : public QObject {
    Q_OBJECT
private slots:
    void testAddRuleThroughput() {
        WorkflowComplianceManagerService svc;
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 10000; i++) {
            svc.addRule(QStringLiteral("R%1").arg(i), QStringLiteral("cat"), QStringLiteral("req"));
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 1000);
        QCOMPARE(svc.ruleCount(), 10000);
        qDebug() << "10000 addRule() calls:" << elapsed << "ms";
    }

    void testQueryLatency() {
        WorkflowComplianceManagerService svc;
        for (int i = 0; i < 1000; i++)
            svc.addRule(QStringLiteral("R%1").arg(i), QStringLiteral("cat"), QStringLiteral("req"));
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 10000; i++) {
            svc.rule(QStringLiteral("R500"));
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 500);
        qDebug() << "10000 rule() lookups:" << elapsed << "ms";
    }

    void testAuditThroughput() {
        WorkflowComplianceManagerService svc;
        QVector<QString> ids;
        for (int i = 0; i < 1000; i++)
            ids.append(svc.addRule(QStringLiteral("R%1").arg(i), QStringLiteral("c"), QStringLiteral("r")));
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 10000; i++) {
            svc.auditRule(ids[i % ids.size()]);
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 1000);
        qDebug() << "10000 auditRule() calls:" << elapsed << "ms";
    }

    void testCategoryFilterThroughput() {
        WorkflowComplianceManagerService svc;
        for (int i = 0; i < 5000; i++)
            svc.addRule(QStringLiteral("R%1").arg(i), i % 3 == 0 ? QStringLiteral("safety") : QStringLiteral("quality"),
                        QStringLiteral("req"));
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 10000; i++) {
            svc.rulesByCategory(QStringLiteral("safety"));
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 5000);
        qDebug() << "10000 rulesByCategory() calls:" << elapsed << "ms";
    }
};

QTEST_MAIN(WorkflowComplianceManagerPerformanceTest)
#include "workflow_compliance_manager_performance_test.moc"
