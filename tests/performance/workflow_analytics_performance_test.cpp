#include "services/WorkflowAnalyticsService.h"
#include <QElapsedTimer>
#include <QSignalSpy>
#include <QTest>

class WorkflowAnalyticsPerformanceTest : public QObject {
    Q_OBJECT
private slots:
    void testExecutionRecordingThroughput() {
        WorkflowAnalyticsService svc;
        QElapsedTimer timer;
        timer.start();

        const int count = 5000;
        for (int i = 0; i < count; i++) {
            svc.recordExecution(QStringLiteral("wf_%1").arg(i % 100), i % 3 != 0, 50.0 + (i % 200));
        }

        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 5000);
        qDebug() << "Execution recording throughput:" << count << "records in" << elapsed << "ms";
    }

    void testErrorRecordingThroughput() {
        WorkflowAnalyticsService svc;
        QElapsedTimer timer;
        timer.start();

        const int count = 5000;
        for (int i = 0; i < count; i++) {
            svc.recordError(QStringLiteral("wf_%1").arg(i % 100), QStringLiteral("Type_%1").arg(i % 10),
                            QStringLiteral("Error message %1").arg(i));
        }

        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 5000);
        qDebug() << "Error recording throughput:" << count << "records in" << elapsed << "ms";
    }

    void testResourceRecordingThroughput() {
        WorkflowAnalyticsService svc;
        QElapsedTimer timer;
        timer.start();

        const int count = 5000;
        for (int i = 0; i < count; i++) {
            svc.recordResourceUsage(QStringLiteral("wf_%1").arg(i % 100), 50.0 + (i % 50), 256.0 + (i % 512),
                                    100.0 + (i % 200));
        }

        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 5000);
        qDebug() << "Resource recording throughput:" << count << "records in" << elapsed << "ms";
    }

    void testAnalysisLatency() {
        WorkflowAnalyticsService svc;
        for (int i = 0; i < 1000; i++) {
            svc.recordExecution(QStringLiteral("wf_perf"), i % 2 == 0, 100.0 + i);
            svc.recordError(QStringLiteral("wf_perf"), QStringLiteral("Timeout"), QStringLiteral("err %1").arg(i));
            svc.recordResourceUsage(QStringLiteral("wf_perf"), 60.0, 512.0, 150.0);
        }

        QElapsedTimer timer;
        timer.start();

        const int iterations = 1000;
        for (int i = 0; i < iterations; i++) {
            svc.analyzeExecution(QStringLiteral("wf_perf"));
            svc.analyzePerformance(QStringLiteral("wf_perf"));
            svc.analyzeErrors(QStringLiteral("wf_perf"));
            svc.analyzeResources(QStringLiteral("wf_perf"));
        }

        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 10000);
        qDebug() << "Analysis latency:" << iterations * 4 << "analyses in" << elapsed << "ms";
    }

    void testSignalThroughput() {
        WorkflowAnalyticsService svc;
        QSignalSpy spy(&svc, &WorkflowAnalyticsService::analysisCompleted);

        QElapsedTimer timer;
        timer.start();

        const int count = 1000;
        for (int i = 0; i < count; i++) {
            svc.recordExecution(QStringLiteral("wf_sig"), true, 100.0);
            svc.analyzeExecution(QStringLiteral("wf_sig"));
        }

        qint64 elapsed = timer.elapsed();
        QCOMPARE(spy.count(), count);
        QVERIFY(elapsed < 5000);
        qDebug() << "Signal throughput:" << count << "signals in" << elapsed << "ms";
    }

    void testMemoryStability() {
        WorkflowAnalyticsService svc;

        for (int round = 0; round < 10; round++) {
            for (int i = 0; i < 100; i++) {
                svc.recordExecution(QStringLiteral("wf_%1_%2").arg(round).arg(i), i % 2 == 0, 50.0 + i);
                svc.recordError(QStringLiteral("wf_%1_%2").arg(round).arg(i), QStringLiteral("Type"),
                                QStringLiteral("msg"));
                svc.recordResourceUsage(QStringLiteral("wf_%1_%2").arg(round).arg(i), 50.0, 256.0, 100.0);
            }
        }

        for (int i = 0; i < 100; i++) {
            svc.analyzeExecution(QStringLiteral("wf_0_%1").arg(i));
        }

        qDebug() << "Memory stability: 3000 records across 1000 workflows";
    }
};

QTEST_MAIN(WorkflowAnalyticsPerformanceTest)
#include "workflow_analytics_performance_test.moc"
