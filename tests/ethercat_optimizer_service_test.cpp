// EtherCATOptimizerServiceTest — Tests for EtherCATOptimizerService
//
// Test coverage:
//   - Configuration, timing, buffer, and priority optimization
//   - Improvement calculation accuracy
//   - Recommendations count (>= 3 per category)
//   - Signal emission on optimization completion

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATOptimizerService.h"

class EtherCATOptimizerServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Optimize configuration and verify category, improvement, and recommendations
  void testOptimizeConfiguration() {
    EtherCATOptimizerService svc(nullptr, nullptr);
    auto result = svc.optimizeConfiguration();
    QCOMPARE(result.category, QStringLiteral("Configuration"));
    QVERIFY(result.before > result.after);
    QVERIFY(result.improvement > 0.0);
    QVERIFY(!result.recommendations.isEmpty());
  }

  // Optimize timing and verify improvement
  void testOptimizeTiming() {
    EtherCATOptimizerService svc(nullptr, nullptr);
    auto result = svc.optimizeTiming();
    QCOMPARE(result.category, QStringLiteral("Timing"));
    QVERIFY(result.before > result.after);
  }

  // Optimize buffers and verify improvement
  void testOptimizeBuffers() {
    EtherCATOptimizerService svc(nullptr, nullptr);
    auto result = svc.optimizeBuffers();
    QCOMPARE(result.category, QStringLiteral("Buffers"));
    QVERIFY(result.before > result.after);
  }

  // Optimize priorities and verify improvement
  void testOptimizePriorities() {
    EtherCATOptimizerService svc(nullptr, nullptr);
    auto result = svc.optimizePriorities();
    QCOMPARE(result.category, QStringLiteral("Priorities"));
    QVERIFY(result.before > result.after);
  }

  // Verify improvement percentage calculation accuracy
  void testImprovementCalculation() {
    EtherCATOptimizerService svc(nullptr, nullptr);
    auto result = svc.optimizeConfiguration();
    double expectedImprovement = ((result.before - result.after) / result.before) * 100.0;
    QCOMPARE(result.improvement, expectedImprovement);
  }

  // Each optimization category returns at least 3 recommendations
  void testRecommendationsNotEmpty() {
    EtherCATOptimizerService svc(nullptr, nullptr);
    auto r1 = svc.optimizeConfiguration();
    auto r2 = svc.optimizeTiming();
    auto r3 = svc.optimizeBuffers();
    auto r4 = svc.optimizePriorities();
    QVERIFY(r1.recommendations.size() >= 3);
    QVERIFY(r2.recommendations.size() >= 3);
    QVERIFY(r3.recommendations.size() >= 3);
    QVERIFY(r4.recommendations.size() >= 3);
  }

  // Verify optimizationCompleted signal is emitted
  void testSignalEmission() {
    EtherCATOptimizerService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATOptimizerService::optimizationCompleted);
    svc.optimizeConfiguration();
    QCOMPARE(spy.count(), 1);
  }
};

QTEST_MAIN(EtherCATOptimizerServiceTest)
#include "ethercat_optimizer_service_test.moc"
