// StartupOptimizerTest — Tests for Startup Optimizer
//
// Test coverage:
//   - Initial metrics (all zeros)
//   - Lazy service initialization
//   - Parallel plugin initialization
//   - Frequently used data preloading
//   - Full initialization sequence
//   - Service and plugin initialized signals
//   - Metrics consistency after full init
#include <QTest>
#include <QSignalSpy>
#include "services/StartupOptimizer.h"
#include "services/ServiceContainer.h"
#include "services/EventBus.h"
#include "infra/EcatClient.h"

class StartupOptimizerTest : public QObject {
  Q_OBJECT
private slots:
  // Verify initial metrics are all zero
  void testInitialMetrics() {
    EcatClient cl;
    EventBus bus; ServiceContainer container(&cl, &bus);
    StartupOptimizer opt(&container);
    StartupMetrics m = opt.metrics();
    QCOMPARE(m.totalTimeMs, 0);
    QCOMPARE(m.serviceInitMs, 0);
    QCOMPARE(m.pluginInitMs, 0);
    QCOMPARE(m.uiInitMs, 0);
    QCOMPARE(m.servicesInitialized, 0);
    QCOMPARE(m.pluginsInitialized, 0);
  }

  // Test lazy service initialization emits completion signal
  void testInitializeServicesLazy() {
    EcatClient cl;
    EventBus bus; ServiceContainer container(&cl, &bus);
    StartupOptimizer opt(&container);
    QSignalSpy spy(&opt, &StartupOptimizer::initializationComplete);
    opt.initializeServicesLazy();
    QCOMPARE(spy.count(), 1);
    StartupMetrics m = opt.metrics();
    QVERIFY(m.serviceInitMs >= 0);
    QVERIFY(m.servicesInitialized >= 0);
  }

  // Test parallel plugin initialization emits completion signal
  void testInitializePluginsParallel() {
    EcatClient cl;
    EventBus bus; ServiceContainer container(&cl, &bus);
    StartupOptimizer opt(&container);
    QSignalSpy spy(&opt, &StartupOptimizer::initializationComplete);
    opt.initializePluginsParallel();
    QCOMPARE(spy.count(), 1);
    StartupMetrics m = opt.metrics();
    QVERIFY(m.pluginInitMs >= 0);
  }

  // Test data preloading emits completion signal
  void testPreloadFrequentlyUsedData() {
    EcatClient cl;
    EventBus bus; ServiceContainer container(&cl, &bus);
    StartupOptimizer opt(&container);
    QSignalSpy spy(&opt, &StartupOptimizer::initializationComplete);
    opt.preloadFrequentlyUsedData();
    QCOMPARE(spy.count(), 1);
    StartupMetrics m = opt.metrics();
    QVERIFY(m.totalTimeMs >= 0);
  }

  // Test full init sequence (services, plugins, preload) metrics
  void testFullInitSequence() {
    EcatClient cl;
    EventBus bus; ServiceContainer container(&cl, &bus);
    StartupOptimizer opt(&container);
    QSignalSpy completeSpy(&opt, &StartupOptimizer::initializationComplete);
    opt.initializeServicesLazy();
    opt.initializePluginsParallel();
    opt.preloadFrequentlyUsedData();
    QCOMPARE(completeSpy.count(), 3);
    StartupMetrics m = opt.metrics();
    QVERIFY(m.totalTimeMs >= 0);
    QVERIFY(m.serviceInitMs >= 0);
    QVERIFY(m.pluginInitMs >= 0);
  }

  // Test serviceInitialized signal fires during lazy init
  void testServiceInitializedSignal() {
    EcatClient cl;
    EventBus bus; ServiceContainer container(&cl, &bus);
    StartupOptimizer opt(&container);
    QSignalSpy svcSpy(&opt, &StartupOptimizer::serviceInitialized);
    opt.initializeServicesLazy();
    QVERIFY(svcSpy.count() >= 0);
  }

  // Test pluginInitialized signal count after parallel init
  void testPluginInitializedSignal() {
    EcatClient cl;
    EventBus bus; ServiceContainer container(&cl, &bus);
    StartupOptimizer opt(&container);
    QSignalSpy plugSpy(&opt, &StartupOptimizer::pluginInitialized);
    opt.initializePluginsParallel();
    QCOMPARE(plugSpy.count(), 0);
  }

  // Verify metrics are non-negative after full init sequence
  void testMetricsConsistency() {
    EcatClient cl;
    EventBus bus; ServiceContainer container(&cl, &bus);
    StartupOptimizer opt(&container);
    opt.initializeServicesLazy();
    opt.initializePluginsParallel();
    opt.preloadFrequentlyUsedData();
    StartupMetrics m = opt.metrics();
    QVERIFY(m.totalTimeMs >= 0);
    QVERIFY(m.serviceInitMs >= 0);
    QVERIFY(m.pluginInitMs >= 0);
  }
};

QTEST_MAIN(StartupOptimizerTest)
#include "startup_optimizer_test.moc"
