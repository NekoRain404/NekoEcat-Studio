// SdoOptimizationPluginTest — Tests for SdoOptimizationPlugin
//
// Test coverage:
//   - Plugin identity and ordering
//   - Widget creation
//   - Cache optimizer widget
//   - Batch optimizer widget
//   - Export button

#include <QTest>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include "plugins/sdooptimization/SdoOptimizationPlugin.h"
#include "plugins/sdooptimization/CacheOptimizerWidget.h"
#include "plugins/sdooptimization/BatchOptimizerWidget.h"
#include "services/SdoOptimizationService.h"
#include "infra/EcatClient.h"
#include "services/EventBus.h"

class SdoOptimizationPluginTest : public QObject {
  Q_OBJECT
private slots:
  void testIdentity() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationPlugin p(&client, &bus);
    QCOMPARE(p.id(), QString("sdooptimization"));
    QCOMPARE(p.displayName(), QString("SDO Optimization"));
    QCOMPARE(p.displayNameZh(), QString("SDO 优化"));
  }

  void testDefaultOrder() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationPlugin p(&client, &bus);
    QCOMPARE(p.defaultOrder(), 48);
  }

  void testVisible() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationPlugin p(&client, &bus);
    QVERIFY(p.visible());
  }

  void testWidgetNotNull() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationPlugin p(&client, &bus);
    QVERIFY(p.widget() != nullptr);
  }

  void testCacheOptimizerNotNull() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationPlugin p(&client, &bus);
    QVERIFY(p.cacheOptimizer() != nullptr);
  }

  void testCacheOptimizerLabels() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationPlugin p(&client, &bus);
    auto *w = p.cacheOptimizer();
    QVERIFY(w->cacheSizeLabel() != nullptr);
    QVERIFY(w->hitRateLabel() != nullptr);
    QVERIFY(w->missLatencyLabel() != nullptr);
  }

  void testCacheOptimizerUpdate() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationPlugin p(&client, &bus);
    auto *w = p.cacheOptimizer();
    w->updateCurrentCache(256, 0.80, 5.0);
    QCOMPARE(w->cacheSizeLabel()->text(), QString("256 entries"));
    QCOMPARE(w->hitRateLabel()->text(), QString("80.0%"));
    QCOMPARE(w->missLatencyLabel()->text(), QString("5.0 ms"));
  }

  void testCacheOptimizerButton() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationPlugin p(&client, &bus);
    auto *w = p.cacheOptimizer();
    QVERIFY(w->optimizeButton() != nullptr);
    QVERIFY(!w->optimizeButton()->isEnabled());
  }

  void testCacheOptimizerSetOptimized() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationPlugin p(&client, &bus);
    auto *w = p.cacheOptimizer();
    w->setOptimized();
    QVERIFY(!w->optimizeButton()->isEnabled());
    QCOMPARE(w->optimizeButton()->text(), QString("Optimization Applied"));
  }

  void testCacheOptimizerShowResult() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationPlugin p(&client, &bus);
    auto *w = p.cacheOptimizer();

    SdoOptimizationResult result;
    result.category = "Cache";
    result.description = "test";
    result.before["cacheSize"] = 128;
    result.after["cacheSize"] = 512;
    result.improvement = 104.0;

    w->showOptimizationResult(result);
    QVERIFY(w->optimizeButton()->isEnabled());
  }

  void testBatchOptimizerNotNull() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationPlugin p(&client, &bus);
    QVERIFY(p.batchOptimizer() != nullptr);
  }

  void testBatchOptimizerLabels() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationPlugin p(&client, &bus);
    auto *w = p.batchOptimizer();
    QVERIFY(w->batchSizeLabel() != nullptr);
    QVERIFY(w->transferTimeLabel() != nullptr);
    QVERIFY(w->overheadLabel() != nullptr);
  }

  void testBatchOptimizerUpdate() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationPlugin p(&client, &bus);
    auto *w = p.batchOptimizer();
    w->updateCurrentBatch(8, 60.0, 20.0);
    QCOMPARE(w->batchSizeLabel()->text(), QString("8"));
    QCOMPARE(w->transferTimeLabel()->text(), QString("60.0 ms"));
    QCOMPARE(w->overheadLabel()->text(), QString("20.0%"));
  }

  void testBatchOptimizerButton() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationPlugin p(&client, &bus);
    auto *w = p.batchOptimizer();
    QVERIFY(w->optimizeButton() != nullptr);
    QVERIFY(!w->optimizeButton()->isEnabled());
  }

  void testBatchOptimizerSetOptimized() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationPlugin p(&client, &bus);
    auto *w = p.batchOptimizer();
    w->setOptimized();
    QVERIFY(!w->optimizeButton()->isEnabled());
    QCOMPARE(w->optimizeButton()->text(), QString("Optimization Applied"));
  }

  void testBatchOptimizerShowResult() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationPlugin p(&client, &bus);
    auto *w = p.batchOptimizer();

    SdoOptimizationResult result;
    result.category = "Batch";
    result.description = "test";
    result.before["batchSize"] = 1;
    result.after["batchSize"] = 16;
    result.improvement = 75.0;

    w->showOptimizationResult(result);
    QVERIFY(w->optimizeButton()->isEnabled());
  }

  void testExportButton() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationPlugin p(&client, &bus);
    QVERIFY(p.exportButton() != nullptr);
  }
};

QTEST_MAIN(SdoOptimizationPluginTest)
#include "sdooptimization_plugin_test.moc"
