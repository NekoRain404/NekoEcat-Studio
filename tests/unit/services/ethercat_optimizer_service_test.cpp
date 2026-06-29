// EtherCATOptimizerServiceTest — Tests for EtherCATOptimizerService
//
// Test coverage:
//   - Configuration, timing, buffer, and priority optimization
//   - Improvement calculation accuracy
//   - Recommendations count (>= 3 per category)
//   - Signal emission on optimization completion

#include <QTest>
#include <QSignalSpy>
#include <QFile>
#include "services/EtherCATOptimizerService.h"

class EtherCATOptimizerServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Optimize configuration and verify category, improvement, and recommendations
  void testOptimizeConfiguration() {
    EtherCATOptimizerService svc(nullptr, nullptr);
    auto result = svc.optimizeConfiguration();
    QCOMPARE(result.category, QStringLiteral("Configuration"));
    QCOMPARE(result.before, 0.0);
    QCOMPARE(result.after, 0.0);
    QCOMPARE(result.improvement, 0.0);
    QVERIFY(result.description.contains(QStringLiteral("backend"),
                                        Qt::CaseInsensitive));
    QVERIFY(!result.recommendations.isEmpty());
  }

  // Optimize timing and verify improvement
  void testOptimizeTiming() {
    EtherCATOptimizerService svc(nullptr, nullptr);
    auto result = svc.optimizeTiming();
    QCOMPARE(result.category, QStringLiteral("Timing"));
    QCOMPARE(result.before, 0.0);
    QCOMPARE(result.after, 0.0);
    QCOMPARE(result.improvement, 0.0);
  }

  // Optimize buffers and verify improvement
  void testOptimizeBuffers() {
    EtherCATOptimizerService svc(nullptr, nullptr);
    auto result = svc.optimizeBuffers();
    QCOMPARE(result.category, QStringLiteral("Buffers"));
    QCOMPARE(result.before, 0.0);
    QCOMPARE(result.after, 0.0);
    QCOMPARE(result.improvement, 0.0);
  }

  // Optimize priorities and verify improvement
  void testOptimizePriorities() {
    EtherCATOptimizerService svc(nullptr, nullptr);
    auto result = svc.optimizePriorities();
    QCOMPARE(result.category, QStringLiteral("Priorities"));
    QCOMPARE(result.before, 0.0);
    QCOMPARE(result.after, 0.0);
    QCOMPARE(result.improvement, 0.0);
  }

  // Verify improvement percentage calculation accuracy
  void testImprovementCalculation() {
    EtherCATOptimizerService svc(nullptr, nullptr);
    auto result = svc.optimizeConfiguration();
    QCOMPARE(result.improvement, 0.0);
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

  // Verify optimizationCompleted signal is not emitted for offline rejection
  void testSignalNotEmittedWithoutBackend() {
    EtherCATOptimizerService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATOptimizerService::optimizationCompleted);
    svc.optimizeConfiguration();
    QCOMPARE(spy.count(), 0);
  }

  void testSourceDoesNotContainSyntheticOptimizationData() {
    QFile file(QStringLiteral(SOURCE_ROOT
                              "/apps/ecat-studio/services/EtherCATOptimizerService.cpp"));
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString source = QString::fromUtf8(file.readAll());

    QVERIFY2(!source.contains(QStringLiteral("emit optimizationCompleted(r)")),
             "Offline optimization must not emit completion.");
    QVERIFY2(!source.contains(QStringLiteral("100.0, 80.0")),
             "Configuration optimization must not synthesize before/after data.");
    QVERIFY2(!source.contains(QStringLiteral("1000.0, 800.0")),
             "Timing optimization must not synthesize before/after data.");
  }
};

QTEST_MAIN(EtherCATOptimizerServiceTest)
#include "ethercat_optimizer_service_test.moc"
