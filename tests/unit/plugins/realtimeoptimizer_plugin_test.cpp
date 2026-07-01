// realtimeoptimizer_plugin_test — unit tests for RealtimeOptimizerPlugin,
// LatencyOptimizerWidget, ThroughputOptimizerWidget, and RealtimeOptimizerService.

#include "plugins/realtimeoptimizer/RealtimeOptimizerPlugin.h"
#include "plugins/realtimeoptimizer/LatencyOptimizerWidget.h"
#include "plugins/realtimeoptimizer/ThroughputOptimizerWidget.h"
#include "services/RealtimeOptimizerService.h"
#include "services/EtherCATOptimizerService.h"

#include <QApplication>
#include <QFile>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QTest>
#include <QTemporaryDir>

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
    QVERIFY(!plugin.visible());
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
    QVERIFY(result.description.contains("backend", Qt::CaseInsensitive));
    QCOMPARE(result.before, 0.0);
    QCOMPARE(result.after, 0.0);
    QCOMPARE(result.improvement, 0.0);
    QVERIFY(result.recommendations.size() > 0);
    QCOMPARE(spy.count(), 0);
  }

  void testServiceOptimizeThroughput() {
    RealtimeOptimizerService svc;
    QSignalSpy spy(&svc, &RealtimeOptimizerService::optimizationCompleted);
    auto result = svc.optimizeThroughput();
    QCOMPARE(result.category, QString("Throughput"));
    QVERIFY(result.description.contains("backend", Qt::CaseInsensitive));
    QCOMPARE(result.before, 0.0);
    QCOMPARE(result.after, 0.0);
    QCOMPARE(result.improvement, 0.0);
    QVERIFY(result.recommendations.size() > 0);
    QCOMPARE(spy.count(), 0);
  }

  void testServiceOptimizeResources() {
    RealtimeOptimizerService svc;
    QSignalSpy spy(&svc, &RealtimeOptimizerService::optimizationCompleted);
    auto result = svc.optimizeResources();
    QCOMPARE(result.category, QString("Resources"));
    QVERIFY(result.description.contains("backend", Qt::CaseInsensitive));
    QCOMPARE(result.before, 0.0);
    QCOMPARE(result.after, 0.0);
    QCOMPARE(result.improvement, 0.0);
    QVERIFY(result.recommendations.size() > 0);
    QCOMPARE(spy.count(), 0);
  }

  void testServiceOptimizePriorities() {
    RealtimeOptimizerService svc;
    QSignalSpy spy(&svc, &RealtimeOptimizerService::optimizationCompleted);
    auto result = svc.optimizePriorities();
    QCOMPARE(result.category, QString("Priorities"));
    QVERIFY(result.description.contains("backend", Qt::CaseInsensitive));
    QCOMPARE(result.before, 0.0);
    QCOMPARE(result.after, 0.0);
    QCOMPARE(result.improvement, 0.0);
    QVERIFY(result.recommendations.size() > 0);
    QCOMPARE(spy.count(), 0);
  }

  void testServiceApplyOptimizationFailsWithoutExecutionBackend() {
    RealtimeOptimizerService svc;
    auto result = svc.optimizeLatency();
    const int historyBefore = svc.optimizationHistory().size();

    QSignalSpy spy(&svc, &RealtimeOptimizerService::optimizationApplied);
    bool applied = svc.applyOptimization(result);
    QVERIFY(!applied);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(svc.optimizationHistory().size(), historyBefore);
  }

  void testServiceOptimizationHistory() {
    RealtimeOptimizerService svc;
    svc.optimizeLatency();
    svc.optimizeThroughput();
    svc.optimizeResources();

    auto history = svc.optimizationHistory();
    QCOMPARE(history.size(), 0);
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
    QCOMPARE(spy.count(), 0);
  }

  void testExportReportReportsPersistenceOutcome() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    RealtimeOptimizerService svc;
    RealtimeOptimizerPlugin plugin(&svc);

    const QString path = dir.filePath("optimization_report.csv");
    QVERIFY(plugin.exportReportToFile(path));
    QVERIFY(QFile::exists(path));

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString csv = QString::fromUtf8(file.readAll());
    QVERIFY(csv.startsWith(QStringLiteral("Category,Description,Before,After,Improvement\n")));

    QTest::failOnWarning(QRegularExpression(
        QStringLiteral("QFSFileEngine::open: No file name specified")));
    QVERIFY(!plugin.exportReportToFile(QString()));
    QVERIFY(!plugin.exportReportToFile(dir.path()));
  }

  void testSourceDoesNotContainSyntheticRealtimeOptimizationData() {
    QFile source(QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/services/RealtimeOptimizerService.cpp"));
    QVERIFY(source.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString text = QString::fromUtf8(source.readAll());

    QVERIFY2(!text.contains(QStringLiteral("emit optimizationCompleted(result)")),
             "Realtime optimizer must not report completion without backend evidence");
    QVERIFY2(!text.contains(QStringLiteral("result.before = 150.0")),
             "Realtime optimizer must not synthesize latency baselines");
    QVERIFY2(!text.contains(QStringLiteral("result.after = 1450.0")),
             "Realtime optimizer must not synthesize throughput targets");
    QVERIFY2(!text.contains(QStringLiteral("result.improvement = 90.0")),
             "Realtime optimizer must not synthesize priority improvement");
  }
};

QTEST_MAIN(RealtimeOptimizerPluginTest)
#include "realtimeoptimizer_plugin_test.moc"
