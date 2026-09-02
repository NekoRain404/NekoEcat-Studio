#include "services/EtherCATAnalyzerService.h"
#include <QElapsedTimer>
#include <QTest>

class EtherCATAnalyzerPerformanceTest : public QObject {
    Q_OBJECT
private slots:
    void testAnalyzeFramesPerformance() {
        EtherCATAnalyzerService svc(nullptr, nullptr);
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 1000; i++) {
            svc.analyzeFrames(100);
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 500);
        qDebug() << "1000 analyzeFrames(100):" << elapsed << "ms";
    }

    void testAnalyzeErrorsPerformance() {
        EtherCATAnalyzerService svc(nullptr, nullptr);
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 1000; i++) {
            svc.analyzeErrors(100);
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 500);
        qDebug() << "1000 analyzeErrors(100):" << elapsed << "ms";
    }

    void testAnalyzePerformancePerformance() {
        EtherCATAnalyzerService svc(nullptr, nullptr);
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 1000; i++) {
            svc.analyzePerformance(5000);
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 500);
        qDebug() << "1000 analyzePerformance(5000):" << elapsed << "ms";
    }

    void testAnalyzeTrendPerformance() {
        EtherCATAnalyzerService svc(nullptr, nullptr);
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 1000; i++) {
            svc.analyzeTrend(60000);
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 500);
        qDebug() << "1000 analyzeTrend(60000):" << elapsed << "ms";
    }

    void testMixedAnalysisPerformance() {
        EtherCATAnalyzerService svc(nullptr, nullptr);
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 500; i++) {
            svc.analyzeFrames(50);
            svc.analyzeErrors(50);
            svc.analyzePerformance(1000);
            svc.analyzeTrend(5000);
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 1000);
        qDebug() << "500 mixed analysis cycles:" << elapsed << "ms";
    }
};

QTEST_MAIN(EtherCATAnalyzerPerformanceTest)
#include "ethercat_analyzer_performance_test.moc"
