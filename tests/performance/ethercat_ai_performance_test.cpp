#include <QTest>
#include <QElapsedTimer>
#include "services/EtherCATAIService.h"

class EtherCATAIPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testPredictThroughput() {
    EtherCATAIService svc(nullptr);
    QVector<AIDataPoint> dataPoints;
    for (int i = 0; i < 50; i++) {
      AIDataPoint dp;
      dp.value = static_cast<double>(i) * 0.1;
      dp.timestamp = i * 1000;
      dp.label = QString("Sensor_%1").arg(i);
      dataPoints.append(dp);
    }

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      QVERIFY(svc.predictMaintenance(dataPoints).component.isEmpty());
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "AI predict offline rejection throughput:" << count
             << "requests in" << elapsed << "ms";
  }

  void testAnomalyDetectionThroughput() {
    EtherCATAIService svc(nullptr);
    QVector<AIDataPoint> dataPoints;
    for (int i = 0; i < 100; i++) {
      AIDataPoint dp;
      dp.value = static_cast<double>(i) * 0.05;
      dp.timestamp = i * 1000;
      dataPoints.append(dp);
    }

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      QVERIFY(svc.detectAnomalies(dataPoints).isEmpty());
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "AI anomaly offline rejection throughput:" << count
             << "requests in" << elapsed << "ms";
  }

  void testOptimizeThroughput() {
    EtherCATAIService svc(nullptr);
    AIPerformanceMetrics metrics;
    metrics.cpu = 85.0;
    metrics.memory = 70.0;
    metrics.latency = 5.0;
    metrics.throughput = 1000.0;

    QElapsedTimer timer;
    timer.start();

    const int count = 10000;
    for (int i = 0; i < count; i++) {
      QVERIFY(svc.optimizePerformance(metrics).target.isEmpty());
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "AI optimize offline rejection throughput:" << count
             << "requests in" << elapsed << "ms";
  }

  void testPatternRecognitionThroughput() {
    EtherCATAIService svc(nullptr);
    QVector<AIDataPoint> dataPoints;
    for (int i = 0; i < 20; i++) {
      AIDataPoint dp;
      dp.value = static_cast<double>(i) * 0.25;
      dp.timestamp = i * 1000;
      dataPoints.append(dp);
    }

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      QVERIFY(svc.recognizePatterns(dataPoints).isEmpty());
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "AI pattern offline rejection throughput:" << count
             << "requests in" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATAIPerformanceTest)
#include "ethercat_ai_performance_test.moc"
