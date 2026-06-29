// EtherCATAnalyticsServiceTest — Tests for EtherCATAnalyticsService
//
// Test coverage:
//   - Data analysis (empty, single point, multiple points)
//   - Anomaly detection in data sets
//   - Performance, error, and usage analysis
//   - Signal emission and low-variance pattern detection

#include <QTest>
#include <QSignalSpy>
#include <QDateTime>
#include "services/EtherCATAnalyticsService.h"

class EtherCATAnalyticsServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Analyze empty data set returns default category
  void testAnalyzeEmptyData() {
    EtherCATAnalyticsService svc(nullptr, nullptr);
    QVector<DataPoint> data;
    auto result = svc.analyzeData(data);
    QCOMPARE(result.category, QStringLiteral("Data"));
    QVERIFY(result.recommendations.size() >= 1);
  }

  // Analyze single data point
  void testAnalyzeSinglePoint() {
    EtherCATAnalyticsService svc(nullptr, nullptr);
    QVector<DataPoint> data;
    DataPoint dp;
    dp.timestamp = QDateTime::currentMSecsSinceEpoch();
    dp.value = 42.0;
    dp.source = QStringLiteral("test");
    data << dp;
    auto result = svc.analyzeData(data);
    QCOMPARE(result.category, QStringLiteral("Data"));
    QVERIFY(result.summary.contains(QStringLiteral("1 points")));
  }

  // Analyze multiple data points and detect trends
  void testAnalyzeMultiplePoints() {
    EtherCATAnalyticsService svc(nullptr, nullptr);
    QVector<DataPoint> data;
    for (int i = 0; i < 10; ++i) {
      DataPoint dp;
      dp.timestamp = QDateTime::currentMSecsSinceEpoch() + i;
      dp.value = 10.0 + i;
      data << dp;
    }
    auto result = svc.analyzeData(data);
    QVERIFY(result.summary.contains(QStringLiteral("10 points")));
    QVERIFY(!result.trends.isEmpty());
  }

  // Detect anomalies in data set
  void testAnomalyDetection() {
    EtherCATAnalyticsService svc(nullptr, nullptr);
    QVector<DataPoint> data;
    for (int i = 0; i < 10; ++i) {
      DataPoint dp;
      dp.timestamp = QDateTime::currentMSecsSinceEpoch() + i;
      dp.value = 10.0;
      data << dp;
    }
    DataPoint anomaly;
    anomaly.timestamp = QDateTime::currentMSecsSinceEpoch() + 100;
    anomaly.value = 1000.0;
    data << anomaly;
    auto result = svc.analyzeData(data);
    QVERIFY(!result.anomalies.isEmpty());
  }

  // Analyze performance metrics
  void testAnalyzePerformance() {
    EtherCATAnalyticsService svc(nullptr, nullptr);
    auto result = svc.analyzePerformance(60);
    QCOMPARE(result.category, QStringLiteral("Performance"));
    QVERIFY(!result.recommendations.isEmpty());
  }

  // Analyze error patterns
  void testAnalyzeErrors() {
    EtherCATAnalyticsService svc(nullptr, nullptr);
    auto result = svc.analyzeErrors(60);
    QCOMPARE(result.category, QStringLiteral("Errors"));
    QVERIFY(!result.recommendations.isEmpty());
  }

  // Analyze usage statistics
  void testAnalyzeUsage() {
    EtherCATAnalyticsService svc(nullptr, nullptr);
    auto result = svc.analyzeUsage(60);
    QCOMPARE(result.category, QStringLiteral("Usage"));
    QVERIFY(!result.recommendations.isEmpty());
  }

  // Signal emitted on analysis completion
  void testSignalEmission() {
    EtherCATAnalyticsService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATAnalyticsService::analysisCompleted);
    QVector<DataPoint> data;
    DataPoint dp;
    dp.value = 42.0;
    data << dp;
    svc.analyzeData(data);
    QCOMPARE(spy.count(), 1);
  }

  // Detect low-variance pattern in near-constant data
  void testLowVariancePattern() {
    EtherCATAnalyticsService svc(nullptr, nullptr);
    QVector<DataPoint> data;
    for (int i = 0; i < 10; ++i) {
      DataPoint dp;
      dp.timestamp = QDateTime::currentMSecsSinceEpoch() + i;
      dp.value = 100.0 + (i * 0.001);
      data << dp;
    }
    auto result = svc.analyzeData(data);
    QVERIFY(result.patterns.size() >= 1);
  }
};

QTEST_MAIN(EtherCATAnalyticsServiceTest)
#include "ethercat_analytics_service_test.moc"
