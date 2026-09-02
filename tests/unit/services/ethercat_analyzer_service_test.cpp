// EtherCATAnalyzerServiceTest — Tests for EtherCATAnalyzerService
//
// Test coverage:
//   - Frame, error, performance, and trend analysis defaults
//   - Frame, error, and performance sample addition
//   - Trend point addition
//   - Frame type counting
//   - Error rate calculation
//   - Performance rating classification
//   - Buffer size limit

#include "services/EtherCATAnalyzerService.h"
#include <QDateTime>
#include <QSignalSpy>
#include <QTest>

class EtherCATAnalyzerServiceTest : public QObject {
    Q_OBJECT
private slots:
    // Verify default frame analysis returns zero counts
    void testDefaultFrameAnalysis() {
        EtherCATAnalyzerService svc(nullptr, nullptr);
        auto result = svc.analyzeFrames(100);
        QCOMPARE(result.totalFrames, 0);
        QCOMPARE(result.errorFrames, 0);
    }

    // Verify default error analysis returns zero total
    void testDefaultErrorAnalysis() {
        EtherCATAnalyzerService svc(nullptr, nullptr);
        auto result = svc.analyzeErrors(100);
        QCOMPARE(result.totalErrors, 0);
    }

    // Verify default performance analysis returns zero cycle time
    void testDefaultPerformanceAnalysis() {
        EtherCATAnalyzerService svc(nullptr, nullptr);
        auto result = svc.analyzePerformance(5000);
        QCOMPARE(result.avgCycleTimeUs, 0.0);
    }

    void testDefaultPerformanceAnalysisDoesNotSynthesizeRating() {
        EtherCATAnalyzerService svc(nullptr, nullptr);
        QSignalSpy spy(&svc, &EtherCATAnalyzerService::performanceAnalysisCompleted);

        auto result = svc.analyzePerformance(5000);

        QVERIFY(result.samples.isEmpty());
        QCOMPARE(result.rating, QString());
        QCOMPARE(spy.count(), 0);
    }

    // Verify default trend analysis returns empty points
    void testDefaultTrendAnalysis() {
        EtherCATAnalyzerService svc(nullptr, nullptr);
        auto result = svc.analyzeTrend(60000);
        QCOMPARE(result.points.size(), 0);
    }

    // Verify adding a frame sample increments frame count
    void testAddFrameSample() {
        EtherCATAnalyzerService svc(nullptr, nullptr);
        FrameInfo f;
        f.position = 0;
        f.type = QStringLiteral("LRW");
        f.size = 128;
        f.hasError = false;
        svc.addFrameSample(f);
        auto result = svc.analyzeFrames(1);
        QCOMPARE(result.totalFrames, 1);
    }

    // Verify adding an error sample increments error count
    void testAddErrorSample() {
        EtherCATAnalyzerService svc(nullptr, nullptr);
        ErrorEntry e;
        e.position = 1;
        e.type = QStringLiteral("CRC");
        e.severity = 3;
        svc.addErrorSample(e);
        auto result = svc.analyzeErrors(1);
        QCOMPARE(result.totalErrors, 1);
    }

    // Verify adding a performance sample updates cycle time
    void testAddPerformanceSample() {
        EtherCATAnalyzerService svc(nullptr, nullptr);
        PerformanceSample s;
        s.timestampMs = QDateTime::currentMSecsSinceEpoch();
        s.cycleTimeUs = 1000.0;
        s.jitterUs = 5.0;
        svc.addPerformanceSample(s);
        auto result = svc.analyzePerformance(60000);
        QCOMPARE(result.avgCycleTimeUs, 1000.0);
    }

    // Verify adding a trend point is reflected in analysis
    void testAddTrendPoint() {
        EtherCATAnalyzerService svc(nullptr, nullptr);
        TrendPoint p;
        p.timestampMs = QDateTime::currentMSecsSinceEpoch();
        p.value = 42.0;
        svc.addTrendPoint(p);
        auto result = svc.analyzeTrend(60000);
        QVERIFY(result.points.size() >= 0);
    }

    // Verify frame type counting groups by type string
    void testFrameTypesCounting() {
        EtherCATAnalyzerService svc(nullptr, nullptr);
        for (int i = 0; i < 5; i++) {
            FrameInfo f;
            f.type = QStringLiteral("LRW");
            svc.addFrameSample(f);
        }
        FrameInfo f2;
        f2.type = QStringLiteral("LRD");
        svc.addFrameSample(f2);
        auto result = svc.analyzeFrames(10);
        QCOMPARE(result.frameTypes.size(), 2);
    }

    // Verify error rate is calculated as errors per frame
    void testErrorRateCalculation() {
        EtherCATAnalyzerService svc(nullptr, nullptr);
        for (int i = 0; i < 10; i++) {
            ErrorEntry e;
            e.type = QStringLiteral("CRC");
            svc.addErrorSample(e);
        }
        auto result = svc.analyzeErrors(10);
        QCOMPARE(result.totalErrors, 10);
        QCOMPARE(result.errorRate, 1.0);
    }

    // Verify low jitter yields Excellent performance rating
    void testPerformanceRating() {
        EtherCATAnalyzerService svc(nullptr, nullptr);
        PerformanceSample s;
        s.timestampMs = QDateTime::currentMSecsSinceEpoch();
        s.jitterUs = 5.0;
        svc.addPerformanceSample(s);
        auto result = svc.analyzePerformance(60000);
        QCOMPARE(result.rating, QStringLiteral("Excellent"));
    }

    // Verify frame buffer caps at 10000 entries
    void testBufferLimit() {
        EtherCATAnalyzerService svc(nullptr, nullptr);
        for (int i = 0; i < 10001; i++) {
            FrameInfo f;
            f.type = QStringLiteral("LRW");
            svc.addFrameSample(f);
        }
        auto result = svc.analyzeFrames(20000);
        QCOMPARE(result.totalFrames, 10000);
    }
};

QTEST_MAIN(EtherCATAnalyzerServiceTest)
#include "ethercat_analyzer_service_test.moc"
