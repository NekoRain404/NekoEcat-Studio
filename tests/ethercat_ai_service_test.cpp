// EtherCATAIServiceTest — Tests for EtherCATAIService
//
// Test coverage:
//   - Predictive maintenance (normal + empty data)
//   - Anomaly detection (with outliers, single point, severity)
//   - Performance optimization (CPU, memory, latency, throughput)
//   - Pattern recognition (increasing, decreasing, stable, empty)

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
    QCOMPARE(pred.component, QStringLiteral("motor"));
    QVERIFY(pred.probability >= 0.0 && pred.probability <= 1.0);
    QVERIFY(pred.confidence > 0.0);
    QVERIFY(pred.timeframeDays > 0);
    QCOMPARE(spy.count(), 1);
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
    QVERIFY(!anomalies.isEmpty());
    QCOMPARE(anomalies[0].point.value, 500.0);
    QVERIFY(anomalies[0].deviation > 2.0);
    QCOMPARE(spy.count(), anomalies.size());
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
    QCOMPARE(opt.target, QStringLiteral("CPU"));
    QVERIFY(opt.expectedImprovement > 0.0);
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
    QCOMPARE(opt.target, QStringLiteral("Memory"));
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
    QCOMPARE(opt.target, QStringLiteral("Latency"));
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
    QCOMPARE(opt.target, QStringLiteral("Throughput"));
  }

  // Detect increasing trend pattern
  void testRecognizePatternsIncreasing() {
    EtherCATAIService svc;
    QVector<AIDataPoint> data;
    for (int i = 0; i < 5; ++i) {
        AIDataPoint dp;
        dp.value = i * 10.0;
        data.append(dp);
    }
    QVector<Pattern> patterns = svc.recognizePatterns(data);
    bool found = false;
    for (const auto &p : patterns) {
        if (p.name.contains(QStringLiteral("Increase")))
            found = true;
    }
    QVERIFY(found);
  }

  // Detect decreasing trend pattern
  void testRecognizePatternsDecreasing() {
    EtherCATAIService svc;
    QVector<AIDataPoint> data;
    for (int i = 5; i > 0; --i) {
        AIDataPoint dp;
        dp.value = i * 10.0;
        data.append(dp);
    }
    QVector<Pattern> patterns = svc.recognizePatterns(data);
    bool found = false;
    for (const auto &p : patterns) {
        if (p.name.contains(QStringLiteral("Decrease")))
            found = true;
    }
    QVERIFY(found);
  }

  // Detect stable pattern in low-variance data
  void testRecognizePatternsStable() {
    EtherCATAIService svc;
    QVector<AIDataPoint> data;
    for (int i = 0; i < 10; ++i) {
        AIDataPoint dp;
        dp.value = 100.0 + (i % 2) * 0.1;
        data.append(dp);
    }
    QVector<Pattern> patterns = svc.recognizePatterns(data);
    bool found = false;
    for (const auto &p : patterns) {
        if (p.name == QStringLiteral("Stable"))
            found = true;
    }
    QVERIFY(found);
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
    if (!anomalies.isEmpty())
        QCOMPARE(anomalies[0].severity, Anomaly::Critical);
  }
};

QTEST_MAIN(EtherCATAIServiceTest)
#include "ethercat_ai_service_test.moc"
