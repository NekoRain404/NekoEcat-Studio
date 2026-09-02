#include "services/WorkflowSchedulingService.h"
#include <QElapsedTimer>
#include <QSignalSpy>
#include <QTest>

class WorkflowSchedulingPerformanceTest : public QObject {
    Q_OBJECT
private:
    WorkflowConfig makeConfig(int id) {
        WorkflowConfig cfg;
        cfg.workflowId = QStringLiteral("wf_%1").arg(id);
        cfg.name = QStringLiteral("Workflow %1").arg(id);
        cfg.description = QStringLiteral("Performance test workflow");
        cfg.scheduleType = ScheduleType::Priority;
        cfg.schedule = QStringLiteral("*/5 * * * *");
        cfg.triggers << QStringLiteral("event1");
        cfg.steps.append(QJsonObject{{"action", "step1"}});
        cfg.steps.append(QJsonObject{{"action", "step2"}});
        cfg.priority = id % 10;
        cfg.timeoutMs = 5000;
        return cfg;
    }

private slots:
    void testScheduleThroughput() {
        WorkflowSchedulingService svc;
        QElapsedTimer timer;
        timer.start();

        const int count = 1000;
        for (int i = 0; i < count; i++) {
            svc.scheduleWorkflow(makeConfig(i));
        }

        qint64 elapsed = timer.elapsed();
        QCOMPARE(svc.workflowCount(), count);
        QVERIFY(elapsed < 5000);
        qDebug() << "Schedule throughput:" << count << "workflows in" << elapsed << "ms";
    }

    void testTriggerThroughput() {
        WorkflowSchedulingService svc;
        const int count = 500;
        for (int i = 0; i < count; i++) {
            svc.scheduleWorkflow(makeConfig(i));
        }

        QElapsedTimer timer;
        timer.start();

        for (int i = 0; i < count; i++) {
            svc.triggerWorkflow(QStringLiteral("wf_%1").arg(i));
        }

        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 5000);
        qDebug() << "Trigger throughput:" << count << "workflows in" << elapsed << "ms";
    }

    void testMultipleRunsPerformance() {
        WorkflowSchedulingService svc;
        svc.scheduleWorkflow(makeConfig(0));

        QElapsedTimer timer;
        timer.start();

        const int count = 1000;
        for (int i = 0; i < count; i++) {
            svc.triggerWorkflow(QStringLiteral("wf_0"));
        }

        qint64 elapsed = timer.elapsed();
        QCOMPARE(svc.runs(QStringLiteral("wf_0")).size(), count);
        QVERIFY(elapsed < 5000);
        qDebug() << "Multiple runs:" << count << "runs in" << elapsed << "ms";
    }

    void testQueryLatency() {
        WorkflowSchedulingService svc;
        const int count = 500;
        for (int i = 0; i < count; i++) {
            svc.scheduleWorkflow(makeConfig(i));
        }

        QElapsedTimer timer;
        timer.start();

        const int iterations = 10000;
        for (int i = 0; i < iterations; i++) {
            svc.workflow(QStringLiteral("wf_%1").arg(i % count));
        }

        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 1000);
        qDebug() << "Query latency:" << iterations << "queries in" << elapsed << "ms";
    }

    void testAllWorkflowsQuery() {
        WorkflowSchedulingService svc;
        const int count = 1000;
        for (int i = 0; i < count; i++) {
            svc.scheduleWorkflow(makeConfig(i));
        }

        QElapsedTimer timer;
        timer.start();

        auto all = svc.allWorkflows();
        qint64 elapsed = timer.elapsed();

        QCOMPARE(all.size(), count);
        QVERIFY(elapsed < 1000);
        qDebug() << "All workflows query:" << count << "workflows in" << elapsed << "ms";
    }

    void testSignalThroughput() {
        WorkflowSchedulingService svc;
        QSignalSpy spy(&svc, &WorkflowSchedulingService::workflowScheduled);

        QElapsedTimer timer;
        timer.start();

        const int count = 1000;
        for (int i = 0; i < count; i++) {
            svc.scheduleWorkflow(makeConfig(i));
        }

        qint64 elapsed = timer.elapsed();
        QCOMPARE(spy.count(), count);
        QVERIFY(elapsed < 5000);
        qDebug() << "Signal throughput:" << count << "signals in" << elapsed << "ms";
    }

    void testCancelThroughput() {
        WorkflowSchedulingService svc;
        const int count = 500;
        for (int i = 0; i < count; i++) {
            svc.scheduleWorkflow(makeConfig(i));
        }

        QElapsedTimer timer;
        timer.start();

        for (int i = 0; i < count; i++) {
            svc.cancelWorkflow(QStringLiteral("wf_%1").arg(i));
        }

        qint64 elapsed = timer.elapsed();
        QCOMPARE(svc.workflowCount(), 0);
        QVERIFY(elapsed < 3000);
        qDebug() << "Cancel throughput:" << count << "cancellations in" << elapsed << "ms";
    }
};

QTEST_MAIN(WorkflowSchedulingPerformanceTest)
#include "workflow_scheduling_performance_test.moc"
