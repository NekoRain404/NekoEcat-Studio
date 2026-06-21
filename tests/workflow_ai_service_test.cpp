#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowAIService.h"

class WorkflowAIServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testPredictMaintenance() {
    WorkflowAIService svc;
    QSignalSpy spy(&svc, &WorkflowAIService::predictionMade);
    QVector<WfDataPoint> data;
    for (int i = 0; i < 10; ++i) {
      WfDataPoint dp;
      dp.value = 50.0;
      dp.timestamp = i;
      dp.label = "motor";
      data.append(dp);
    }
    auto pred = svc.predictMaintenance(data);
    QCOMPARE(pred.component, QString("motor"));
    QCOMPARE(pred.probability, 0.75);
    QCOMPARE(pred.timeframeDays, 30);
    QCOMPARE(spy.count(), 1);
  }

  void testPredictMaintenanceEmpty() {
    WorkflowAIService svc;
    auto pred = svc.predictMaintenance({});
    QVERIFY(pred.component.isEmpty());
    QCOMPARE(pred.probability, 0.0);
  }

  void testPredictMaintenanceConfidence() {
    WorkflowAIService svc;
    QVector<WfDataPoint> data;
    WfDataPoint dp;
    dp.value = 90.0;
    dp.label = "sensor";
    data.append(dp);
    auto pred = svc.predictMaintenance(data);
    QCOMPARE(pred.confidence, 0.85);
    QVERIFY(!pred.recommendations.isEmpty());
  }

  void testDetectAnomalies() {
    WorkflowAIService svc;
    QSignalSpy spy(&svc, &WorkflowAIService::anomalyDetected);
    QVector<WfDataPoint> data;
    for (int i = 0; i < 5; ++i) {
      WfDataPoint dp;
      dp.value = 10.0;
      data.append(dp);
    }
    WfDataPoint outlier;
    outlier.value = 100.0;
    data.append(outlier);
    auto anomalies = svc.detectAnomalies(data);
    QVERIFY(!anomalies.isEmpty());
    QVERIFY(spy.count() >= 1);
  }

  void testDetectAnomaliesEmpty() {
    WorkflowAIService svc;
    auto anomalies = svc.detectAnomalies({});
    QVERIFY(anomalies.isEmpty());
  }

  void testDetectAnomaliesSingleElement() {
    WorkflowAIService svc;
    QVector<WfDataPoint> data;
    WfDataPoint dp;
    dp.value = 50.0;
    data.append(dp);
    auto anomalies = svc.detectAnomalies(data);
    QVERIFY(anomalies.isEmpty());
  }

  void testOptimizePerformance() {
    WorkflowAIService svc;
    WfAIPerformanceMetrics metrics;
    metrics.cpu = 80.0;
    metrics.memory = 60.0;
    metrics.latency = 10.0;
    metrics.throughput = 1000.0;
    metrics.errorRate = 0.01;
    auto opt = svc.optimizePerformance(metrics);
    QCOMPARE(opt.target, QString("throughput"));
    QCOMPARE(opt.currentValue, 1000.0);
    QVERIFY(opt.suggestedValue > opt.currentValue);
    QCOMPARE(opt.expectedImprovement, 15.0);
  }

  void testRecognizePatterns() {
    WorkflowAIService svc;
    QVector<WfDataPoint> data;
    for (int i = 0; i < 10; ++i) {
      WfDataPoint dp;
      dp.value = 42.0;
      dp.label = "steady";
      data.append(dp);
    }
    auto patterns = svc.recognizePatterns(data);
    QCOMPARE(patterns.size(), 1);
    QCOMPARE(patterns[0].name, QString("steady-state"));
    QCOMPARE(patterns[0].confidence, 0.9);
  }

  void testRecognizePatternsEmpty() {
    WorkflowAIService svc;
    auto patterns = svc.recognizePatterns({});
    QVERIFY(patterns.isEmpty());
  }

  void testAnomalySeverity() {
    WorkflowAIService svc;
    QVector<WfDataPoint> data;
    for (int i = 0; i < 5; ++i) {
      WfDataPoint dp;
      dp.value = 10.0;
      data.append(dp);
    }
    WfDataPoint outlier;
    outlier.value = 200.0;
    data.append(outlier);
    auto anomalies = svc.detectAnomalies(data);
    if (!anomalies.isEmpty()) {
      QCOMPARE(anomalies[0].severity, WfAnomaly::Medium);
    }
  }

  void testOptimizationDescription() {
    WorkflowAIService svc;
    WfAIPerformanceMetrics metrics;
    metrics.throughput = 500.0;
    auto opt = svc.optimizePerformance(metrics);
    QVERIFY(!opt.description.isEmpty());
  }
};

QTEST_MAIN(WorkflowAIServiceTest)
#include "workflow_ai_service_test.moc"
