// SdoOptimizationPluginTest — Tests for SdoOptimizationPlugin
//
// Test coverage:
//   - Plugin identity and ordering
//   - Widget creation
//   - Offline service apply fails closed
//   - Cache optimizer widget
//   - Batch optimizer widget
//   - Export button

#include <QTest>
#include <QLabel>
#include <QPushButton>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QTabWidget>
#include <QFile>
#include <QTcpServer>
#include <QTemporaryDir>
#include "plugins/sdooptimization/SdoOptimizationPlugin.h"
#include "plugins/sdooptimization/CacheOptimizerWidget.h"
#include "plugins/sdooptimization/BatchOptimizerWidget.h"
#include "services/SdoOptimizationService.h"
#include "infra/EcatClient.h"
#include "services/EventBus.h"

class SdoOptimizationPluginTest : public QObject {
  Q_OBJECT
private:
  static bool waitForConnected(EcatClient &client) {
    for (int i = 0; i < 50 && !client.isConnected(); ++i) {
      QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
      QTest::qWait(10);
    }
    return client.isConnected();
  }

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

  void testServiceApplyOptimizationOfflineFailsClosed() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationService svc(&client, &bus);
    QSignalSpy spy(&svc, &SdoOptimizationService::optimizationApplied);
    QVERIFY(spy.isValid());

    const SdoOptimizationResult result = svc.optimizeCache();
    QVERIFY(!svc.applyOptimization(result));
    QCOMPARE(spy.count(), 0);
    QVERIFY(svc.optimizationHistory().isEmpty());
  }

  void testServiceApplyOptimizationConnectedWithoutBackendAckFailsClosed() {
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 0));

    EcatClient client;
    client.connectToHost(QHostAddress::LocalHost, server.serverPort());
    QVERIFY(waitForConnected(client));

    EventBus bus;
    SdoOptimizationService svc(&client, &bus);
    QSignalSpy spy(&svc, &SdoOptimizationService::optimizationApplied);
    QVERIFY(spy.isValid());

    SdoOptimizationResult result;
    result.category = QStringLiteral("Cache");
    result.description = QStringLiteral("SDO cache optimization");
    result.after.insert(QStringLiteral("cacheSize"), 512);

    QVERIFY(!svc.applyOptimization(result));
    QCOMPARE(spy.count(), 0);
    QVERIFY(svc.optimizationHistory().isEmpty());
  }

  void testServiceOptimizeCacheDoesNotSynthesizeBackendData() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationService svc(&client, &bus);
    const SdoOptimizationResult result = svc.optimizeCache();
    QCOMPARE(result.category, QString("Cache"));
    QVERIFY(result.description.contains(QStringLiteral("backend"),
                                        Qt::CaseInsensitive));
    QVERIFY(result.before.isEmpty());
    QVERIFY(result.after.isEmpty());
    QCOMPARE(result.improvement, 0.0);
    QVERIFY(!result.recommendations.isEmpty());
    QVERIFY(!result.applied);
  }

  void testServiceOptimizeBatchDoesNotSynthesizeBackendData() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationService svc(&client, &bus);
    const SdoOptimizationResult result = svc.optimizeBatch();
    QCOMPARE(result.category, QString("Batch"));
    QVERIFY(result.before.isEmpty());
    QVERIFY(result.after.isEmpty());
    QCOMPARE(result.improvement, 0.0);
    QVERIFY(!result.recommendations.isEmpty());
  }

  void testServiceOptimizePerformanceDoesNotSynthesizeBackendData() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationService svc(&client, &bus);
    const SdoOptimizationResult result = svc.optimizePerformance();
    QCOMPARE(result.category, QString("Performance"));
    QVERIFY(result.before.isEmpty());
    QVERIFY(result.after.isEmpty());
    QCOMPARE(result.improvement, 0.0);
    QVERIFY(!result.recommendations.isEmpty());
  }

  void testServiceDoesNotEmitCompletedWithoutBackend() {
    EcatClient client;
    EventBus bus;
    SdoOptimizationService svc(&client, &bus);
    QSignalSpy spy(&svc, &SdoOptimizationService::optimizationCompleted);
    QVERIFY(spy.isValid());

    svc.optimizeCache();
    svc.optimizeBatch();
    svc.optimizePerformance();
    svc.optimizeErrorHandling();
    QCOMPARE(spy.count(), 0);
  }

  void testSourceDoesNotContainSyntheticOptimizationData() {
    QFile file(QStringLiteral(SOURCE_ROOT
                              "/apps/ecat-studio/services/SdoOptimizationService.cpp"));
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString source = QString::fromUtf8(file.readAll());

    QVERIFY2(!source.contains(QStringLiteral("emit optimizationCompleted(result)")),
             "Offline SDO optimization must not emit completion.");
    QVERIFY2(!source.contains(QStringLiteral("104.0")),
             "SDO cache optimization must not synthesize improvement values.");
    QVERIFY2(!source.contains(QStringLiteral("75.0")),
             "SDO batch optimization must not synthesize improvement values.");
    QVERIFY2(!source.contains(QStringLiteral("300.0")),
             "SDO performance optimization must not synthesize improvement values.");
    QVERIFY2(!source.contains(QStringLiteral("applied.applied = true")),
             "SDO optimization must not mark applied without backend acknowledgement.");
    QVERIFY2(!source.contains(QStringLiteral("emit optimizationApplied(applied)")),
             "SDO optimization must not emit applied success without backend acknowledgement.");
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

  void testExportReportReportsPersistenceOutcome() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    EcatClient client;
    EventBus bus;
    SdoOptimizationPlugin p(&client, &bus);

    const QString path = dir.filePath("sdo_optimization.md");
    QVERIFY(p.exportReportToFile(path));
    QVERIFY(QFile::exists(path));

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString markdown = QString::fromUtf8(file.readAll());
    QVERIFY(markdown.startsWith(QStringLiteral("# SDO Optimization Report\n")));
    QVERIFY(markdown.contains(QStringLiteral("No optimizations applied yet.")));

    QTest::failOnWarning(QRegularExpression(
        QStringLiteral("QFSFileEngine::open: No file name specified")));
    QVERIFY(!p.exportReportToFile(QString()));
    QVERIFY(!p.exportReportToFile(dir.path()));
  }
};

QTEST_MAIN(SdoOptimizationPluginTest)
#include "sdooptimization_plugin_test.moc"
