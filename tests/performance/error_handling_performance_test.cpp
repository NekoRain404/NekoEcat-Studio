#include "services/ErrorHandlingService.h"
#include <QElapsedTimer>
#include <QSignalSpy>
#include <QTest>

class ErrorHandlingPerformanceTest : public QObject {
    Q_OBJECT
private slots:
    void testReportErrorThroughput() {
        ErrorHandlingService svc;
        QElapsedTimer timer;
        timer.start();

        const int count = 100000;
        for (int i = 0; i < count; i++) {
            svc.reportError(0, i, QString("Error %1").arg(i), EcatErrorSeverity::Error,
                            EcatErrorCategory::Communication);
        }

        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 1000);
        qDebug() << "ReportError throughput:" << count << "in" << elapsed << "ms";
    }

    void testDetectErrorsThroughput() {
        ErrorHandlingService svc;
        for (int i = 0; i < 1000; i++) {
            svc.reportError(0, i, "Error", EcatErrorSeverity::Error, EcatErrorCategory::Communication);
        }

        QElapsedTimer timer;
        timer.start();

        const int count = 10000;
        volatile int sink = 0;
        for (int i = 0; i < count; i++) {
            sink = svc.detectErrors().size();
        }

        qint64 elapsed = timer.elapsed();
        Q_UNUSED(sink);
        QVERIFY(elapsed < 5000);
        qDebug() << "DetectErrors throughput:" << count << "in" << elapsed << "ms";
    }

    void testClassifyErrorThroughput() {
        ErrorHandlingService svc;
        EcatErrorInfo info;
        info.severity = EcatErrorSeverity::Error;
        info.category = EcatErrorCategory::Communication;

        QElapsedTimer timer;
        timer.start();

        const int count = 1000000;
        volatile int sink = 0;
        for (int i = 0; i < count; i++) {
            sink = static_cast<int>(svc.classifyError(info));
        }

        qint64 elapsed = timer.elapsed();
        Q_UNUSED(sink);
        QVERIFY(elapsed < 500);
        qDebug() << "ClassifyError throughput:" << count << "in" << elapsed << "ms";
    }

    void testErrorHistoryQueryLatency() {
        ErrorHandlingService svc;
        for (int i = 0; i < 1000; i++) {
            svc.reportError(0, i, "Error", EcatErrorSeverity::Error, EcatErrorCategory::Communication);
        }

        QElapsedTimer timer;
        timer.start();

        const int count = 100000;
        volatile int sink = 0;
        for (int i = 0; i < count; i++) {
            sink = svc.errorHistory().size();
        }

        qint64 elapsed = timer.elapsed();
        Q_UNUSED(sink);
        QVERIFY(elapsed < 500);
        qDebug() << "ErrorHistory query latency:" << count << "in" << elapsed << "ms";
    }

    void testMemoryStability() {
        for (int round = 0; round < 100; round++) {
            ErrorHandlingService svc;
            for (int i = 0; i < 1000; i++) {
                svc.reportError(0, i, "Error", EcatErrorSeverity::Error, EcatErrorCategory::Communication);
            }
            svc.detectErrors();
        }
        qDebug() << "Memory stability: 100 rounds of 1000 errors completed";
    }
};

QTEST_MAIN(ErrorHandlingPerformanceTest)
#include "error_handling_performance_test.moc"
