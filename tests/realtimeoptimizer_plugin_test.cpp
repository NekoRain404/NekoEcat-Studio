// realtimeoptimizer_plugin_test — unit tests for RealtimeOptimizerPlugin,
// LatencyOptimizerWidget, ThroughputOptimizerWidget, and RealtimeOptimizerService.

#include "plugins/realtimeoptimizer/RealtimeOptimizerPlugin.h"
#include "plugins/realtimeoptimizer/LatencyOptimizerWidget.h"
#include "plugins/realtimeoptimizer/ThroughputOptimizerWidget.h"
#include "services/RealtimeOptimizerService.h"
#include "services/EtherCATOptimizerService.h"

#include <QApplication>
#include <QSignalSpy>
#include <QTest>

class RealtimeOptimizerPluginTest : public QObject {
  Q_OBJECT
private slots:
  void testPluginIdentity() {
    RealtimeOptimizerService svc;
    RealtimeOptimizerPlugin plugin(&svc);
    QCOMPARE(plugin.id(), QString("realtimeoptimizer"));
    QCOMPARE(plugin.displayName(), QString("Real-time Optimizer"));
    QCOMPARE(plugin.displayNameZh(), QString("实时优化器"));
  }

  void testDefaultOrder() {
    RealtimeOptimizerService svc;
    RealtimeOptimizerPlugin plugin(&svc);
    QCOMPARE(plugin.defaultOrder(), 38);
  }

  void testVisible() {
    RealtimeOptimizerService svc;
    RealtimeOptimizerPlugin plugin(&svc);
    QVERIFY(plugin.visible());
  }

  void testWidgetNotNull() {
    RealtimeOptimizerService svc;
    RealtimeOptimizerPlugin plugin(&svc);
    QVERIFY(plugin.widget() != nullptr);
  }

  void testServiceAccessor() {
    RealtimeOptimizerService svc;
    RealtimeOptimizerPlugin plugin(&svc);
    QCOMPARE(plugin.service(), &svc);
  }

  void testLatencyOptimizerAccessor() {
    RealtimeOptimizerService svc;
    RealtimeOptimizerPlugin plugin(&svc);
    QVERIFY(plugin.latencyOptimizer() != nullptr);
  }

  void testThroughputOptimizerAccessor() {
    RealtimeOptimizerService svc;
    RealtimeOptimizerPlugin plugin(&svc);
    QVERIFY(plugin.throughputOptimizer() != nullptr);
  }

  void testServiceOptimizeLatency() {
    RealtimeOptimizerService svc;
    QSignalSpy spy(&svc, &RealtimeOptimizerService::optimizationCompleted);
    auto result = svc.optimizeLatency();
    QCOMPARE(result.category, QString("Latency"));
    QVERIFY(result.before > result.after);
    QVERIFY(result.improvement > 0.0);
    QVERIFY(result.recommendations.size() > 0);
    QCOMPARE(spy.count(), 1);
  }

  void testServiceOptimizeThroughput() {
    RealtimeOptimizerService svc;
    QSignalSpy spy(&svc, &RealtimeOptimizerService::optimizationCompleted);
    auto result = svc.optimizeThroughput();
    QCOMPARE(result.category, QString("Throughput"));
    QVERIFY(result.after > result.before);
    QVERIFY(result.improvement > 0.0);
    QVERIFY(result.recommendations.size() > 0);
    QCOMPARE(spy.count(), 1);
  }

  void testServiceOptimizeResources() {
    RealtimeOptimizerService svc;
    QSignalSpy spy(&svc, &RealtimeOptimizerService::optimizationCompleted);
    auto result = svc.optimizeResources();
    QCOMPARE(result.category, QString("Resources"));
    QVERIFY(result.before > result.after);
    QVERIFY(result.improvement > 0.0);
    QVERIFY(result.recommendations.size() > 0);
    QCOMPARE(spy.count(), 1);
  }

  void testServiceOptimizePriorities() {
    RealtimeOptimizerService svc;
    QSignalSpy spy(&svc, &RealtimeOptimizerService::optimizationCompleted);
    auto result = svc.optimizePriorities();
    QCOMPARE(result.category, QString("Priorities"));
    QVERIFY(result.after > result.before);
    QVERIFY(result.improvement > 0.0);
    QVERIFY(result.recommendations.size() > 0);
    QCOMPARE(spy.count(), 1);
  }

  void testServiceApplyOptimization() {
    RealtimeOptimizerService svc;
    auto result = svc.optimizeLatency();

    QSignalSpy spy(&svc, &RealtimeOptimizerService::optimizationApplied);
    bool applied = svc.applyOptimization(result);
    QVERIFY(applied);
    QCOMPARE(spy.count(), 1);
  }

  void testServiceOptimizationHistory() {
    RealtimeOptimizerService svc;
    svc.optimizeLatency();
    svc.optimizeThroughput();
    svc.optimizeResources();

    auto history = svc.optimizationHistory();
    QCOMPARE(history.size(), 3);
    QCOMPARE(history[0].category, QString("Latency"));
    QCOMPARE(history[1].category, QString("Throughput"));
    QCOMPARE(history[2].category, QString("Resources"));
  }

  void testServiceClearHistory() {
    RealtimeOptimizerService svc;
    svc.optimizeLatency();
    svc.optimizeThroughput();

    svc.clearHistory();
    auto history = svc.optimizationHistory();
    QCOMPARE(history.size(), 0);
  }

  void testLatencyOptimizerUpdateResult() {
    LatencyOptimizerWidget w;
    OptimizationResult result;
    result.category = "Latency";
    result.before = 150.0;
    result.after = 85.0;
    result.improvement = 43.3;
    result.recommendations = {"Test recommendation"};
    w.updateResult(result);
  }

  void testThroughputOptimizerUpdateResult() {
    ThroughputOptimizerWidget w;
    OptimizationResult result;
    result.category = "Throughput";
    result.before = 1000.0;
    result.after = 1450.0;
    result.improvement = 45.0;
    result.recommendations = {"Test recommendation"};
    w.updateResult(result);
  }

  void testOptimizeAllSignal() {
    RealtimeOptimizerService svc;
    QSignalSpy spy(&svc, &RealtimeOptimizerService::optimizationCompleted);
    svc.optimizeLatency();
    svc.optimizeThroughput();
    svc.optimizeResources();
    svc.optimizePriorities();
    QCOMPARE(spy.count(), 4);
  }
};

QTEST_MAIN(RealtimeOptimizerPluginTest)
#include "realtimeoptimizer_plugin_test.moc"
