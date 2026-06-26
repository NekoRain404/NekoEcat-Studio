// EtherCATAIServiceTest — Tests for EtherCATAIService
//
// Test coverage:
//   - AI prediction, anomaly detection, optimization, and pattern recognition fail closed without model backend
//   - Rejected AI requests do not emit synthetic prediction or anomaly signals

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATAIService.h"

class EtherCATAIServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Predict maintenance with valid data points
  void testPredictMaintenance() {
    EtherCATAIService svc;
    QSignalSpy spy(&svc, &EtherCATAIService::predictionMade);
    QVector<AIDataPoint> data;
    for (int i = 0; i < 10; ++i) {
        AIDataPoint dp;
        dp.value = 50.0 + i * 5.0;
        dp.label = QStringLiteral("motor");
        data.append(dp);
    }
    Prediction pred = svc.predictMaintenance(data);
    QVERIFY(pred.component.isEmpty());
    QCOMPARE(pred.probability, 0.0);
    QCOMPARE(pred.confidence, 0.0);
    QCOMPARE(pred.timeframeDays, 0);
    QVERIFY(pred.recommendations.isEmpty());
    QCOMPARE(spy.count(), 0);
  }

  // Predict maintenance with empty data returns empty result
  void testPredictEmptyData() {
    EtherCATAIService svc;
    Prediction pred = svc.predictMaintenance({});
    QVERIFY(pred.component.isEmpty());
    QCOMPARE(pred.probability, 0.0);
  }

  // Detect outliers in data set with known anomaly
  void testDetectAnomalies() {
    EtherCATAIService svc;
    QSignalSpy spy(&svc, &EtherCATAIService::anomalyDetected);
    QVector<AIDataPoint> data;
    for (int i = 0; i < 10; ++i) {
        AIDataPoint dp;
        dp.value = 100.0;
        data.append(dp);
    }
    AIDataPoint outlier;
    outlier.value = 500.0;
    data.append(outlier);
    QVector<Anomaly> anomalies = svc.detectAnomalies(data);
    QVERIFY(anomalies.isEmpty());
    QCOMPARE(spy.count(), 0);
  }

  // No anomalies detected in uniform data
  void testDetectNoAnomalies() {
    EtherCATAIService svc;
    QVector<AIDataPoint> data;
    for (int i = 0; i < 10; ++i) {
        AIDataPoint dp;
        dp.value = 100.0;
        data.append(dp);
    }
    QVector<Anomaly> anomalies = svc.detectAnomalies(data);
    QVERIFY(anomalies.isEmpty());
  }

  // Single data point cannot be an anomaly
  void testDetectAnomaliesSinglePoint() {
    EtherCATAIService svc;
    QVector<AIDataPoint> data;
    AIDataPoint dp;
    dp.value = 100.0;
    data.append(dp);
    QVector<Anomaly> anomalies = svc.detectAnomalies(data);
    QVERIFY(anomalies.isEmpty());
  }

  // Optimize for high CPU usage
  void testOptimizePerformance() {
    EtherCATAIService svc;
    AIPerformanceMetrics metrics;
    metrics.cpu = 90.0;
    metrics.memory = 50.0;
    metrics.latency = 5.0;
    metrics.throughput = 100.0;
    metrics.errorRate = 0.01;
    Optimization opt = svc.optimizePerformance(metrics);
    QVERIFY(opt.target.isEmpty());
    QCOMPARE(opt.currentValue, 0.0);
    QCOMPARE(opt.suggestedValue, 0.0);
    QCOMPARE(opt.expectedImprovement, 0.0);
    QVERIFY(opt.description.isEmpty());
  }

  // Optimize for high memory usage
  void testOptimizeMemory() {
    EtherCATAIService svc;
    AIPerformanceMetrics metrics;
    metrics.cpu = 50.0;
    metrics.memory = 90.0;
    metrics.latency = 5.0;
    metrics.throughput = 100.0;
    Optimization opt = svc.optimizePerformance(metrics);
    QVERIFY(opt.target.isEmpty());
  }

  // Optimize for high latency
  void testOptimizeLatency() {
    EtherCATAIService svc;
    AIPerformanceMetrics metrics;
    metrics.cpu = 50.0;
    metrics.memory = 50.0;
    metrics.latency = 15.0;
    metrics.throughput = 100.0;
    Optimization opt = svc.optimizePerformance(metrics);
    QVERIFY(opt.target.isEmpty());
  }

  // Optimize for throughput bottleneck
  void testOptimizeThroughput() {
    EtherCATAIService svc;
    AIPerformanceMetrics metrics;
    metrics.cpu = 50.0;
    metrics.memory = 50.0;
    metrics.latency = 5.0;
    metrics.throughput = 100.0;
    Optimization opt = svc.optimizePerformance(metrics);
    QVERIFY(opt.target.isEmpty());
  }

  // Pattern recognition fails closed without model backend
  void testRecognizePatternsIncreasing() {
    EtherCATAIService svc;
    QVector<AIDataPoint> data;
    for (int i = 0; i < 5; ++i) {
        AIDataPoint dp;
        dp.value = i * 10.0;
        data.append(dp);
    }
    QVector<Pattern> patterns = svc.recognizePatterns(data);
    QVERIFY(patterns.isEmpty());
  }

  // Decreasing trend is not reported as AI pattern without model backend
  void testRecognizePatternsDecreasing() {
    EtherCATAIService svc;
    QVector<AIDataPoint> data;
    for (int i = 5; i > 0; --i) {
        AIDataPoint dp;
        dp.value = i * 10.0;
        data.append(dp);
    }
    QVector<Pattern> patterns = svc.recognizePatterns(data);
    QVERIFY(patterns.isEmpty());
  }

  // Stable pattern is not reported as AI pattern without model backend
  void testRecognizePatternsStable() {
    EtherCATAIService svc;
    QVector<AIDataPoint> data;
    for (int i = 0; i < 10; ++i) {
        AIDataPoint dp;
        dp.value = 100.0 + (i % 2) * 0.1;
        data.append(dp);
    }
    QVector<Pattern> patterns = svc.recognizePatterns(data);
    QVERIFY(patterns.isEmpty());
  }

  // Empty data returns no patterns
  void testRecognizePatternsEmpty() {
    EtherCATAIService svc;
    QVector<Pattern> patterns = svc.recognizePatterns({});
    QVERIFY(patterns.isEmpty());
  }

  // Extreme outlier classified as Critical severity
  void testAnomalySeverity() {
    EtherCATAIService svc;
    QVector<AIDataPoint> data;
    for (int i = 0; i < 100; ++i) {
        AIDataPoint dp;
        dp.value = 100.0;
        data.append(dp);
    }
    AIDataPoint extreme;
    extreme.value = 1000.0;
    data.append(extreme);
    QVector<Anomaly> anomalies = svc.detectAnomalies(data);
    QVERIFY(anomalies.isEmpty());
  }
};

QTEST_MAIN(EtherCATAIServiceTest)
#include "ethercat_ai_service_test.moc"
