// FreeRunOptimizationPluginTest — Tests for FreeRunOptimizationPlugin
//
// Test coverage:
//   - Plugin identity and ordering
//   - Widget creation
//   - Offline service apply fails closed
//   - Cycle time optimizer widget
//   - Data mapping optimizer widget
//   - Export button

#include <QTest>
#include <QFile>
#include <QLabel>
#include <QPushButton>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QTabWidget>
#include <QTcpServer>
#include <QTemporaryDir>
#include "plugins/freerunoptimization/FreeRunOptimizationPlugin.h"
#include "plugins/freerunoptimization/CycleTimeOptimizerWidget.h"
#include "plugins/freerunoptimization/DataMappingOptimizerWidget.h"
#include "services/FreeRunOptimizationService.h"
#include "infra/EcatClient.h"
#include "services/EventBus.h"

class FreeRunOptimizationPluginTest : public QObject {
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
    FreeRunOptimizationPlugin p(&client, &bus);
    QCOMPARE(p.id(), QString("freerunoptimization"));
    QCOMPARE(p.displayName(), QString("Free Run Optimization"));
    QCOMPARE(p.displayNameZh(), QString("自由运行优化"));
  }

  void testDefaultOrder() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationPlugin p(&client, &bus);
    QCOMPARE(p.defaultOrder(), 44);
  }

  void testVisible() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationPlugin p(&client, &bus);
    QVERIFY(!p.visible());
  }

  void testWidgetNotNull() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationPlugin p(&client, &bus);
    QVERIFY(p.widget() != nullptr);
  }

  void testServiceApplyOptimizationOfflineFailsClosed() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationService svc(&client, &bus);
    QSignalSpy spy(&svc, &FreeRunOptimizationService::optimizationApplied);
    QVERIFY(spy.isValid());

    const FreeRunOptimizationResult result = svc.optimizeCycleTime();
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
    FreeRunOptimizationService svc(&client, &bus);
    QSignalSpy spy(&svc, &FreeRunOptimizationService::optimizationApplied);
    QVERIFY(spy.isValid());

    FreeRunOptimizationResult result;
    result.category = QStringLiteral("Cycle Time");
    result.description = QStringLiteral("Free Run cycle-time optimization");
    result.after.insert(QStringLiteral("cycleTimeUs"), 500);

    QVERIFY(!svc.applyOptimization(result));
    QCOMPARE(spy.count(), 0);
    QVERIFY(svc.optimizationHistory().isEmpty());
  }

  void testServiceOptimizeCycleTimeDoesNotSynthesizeBackendData() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationService svc(&client, &bus);

    const FreeRunOptimizationResult result = svc.optimizeCycleTime();

    QCOMPARE(result.category, QString("Cycle Time"));
    QVERIFY(result.description.contains("backend", Qt::CaseInsensitive));
    QVERIFY(result.before.isEmpty());
    QVERIFY(result.after.isEmpty());
    QCOMPARE(result.improvement, 0.0);
    QVERIFY(!result.recommendations.isEmpty());
    QVERIFY(!result.applied);
  }

  void testServiceOptimizeDataMappingDoesNotSynthesizeBackendData() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationService svc(&client, &bus);

    const FreeRunOptimizationResult result = svc.optimizeDataMapping();

    QCOMPARE(result.category, QString("Data Mapping"));
    QVERIFY(result.description.contains("backend", Qt::CaseInsensitive));
    QVERIFY(result.before.isEmpty());
    QVERIFY(result.after.isEmpty());
    QCOMPARE(result.improvement, 0.0);
    QVERIFY(!result.recommendations.isEmpty());
    QVERIFY(!result.applied);
  }

  void testServiceOptimizePerformanceDoesNotSynthesizeBackendData() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationService svc(&client, &bus);

    const FreeRunOptimizationResult result = svc.optimizePerformance();

    QCOMPARE(result.category, QString("Performance"));
    QVERIFY(result.description.contains("backend", Qt::CaseInsensitive));
    QVERIFY(result.before.isEmpty());
    QVERIFY(result.after.isEmpty());
    QCOMPARE(result.improvement, 0.0);
    QVERIFY(!result.recommendations.isEmpty());
    QVERIFY(!result.applied);
  }

  void testServiceDoesNotEmitCompletedWithoutBackend() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationService svc(&client, &bus);
    QSignalSpy spy(&svc, &FreeRunOptimizationService::optimizationCompleted);
    QVERIFY(spy.isValid());

    svc.optimizeCycleTime();
    svc.optimizeDataMapping();
    svc.optimizePerformance();
    svc.optimizeErrorHandling();

    QCOMPARE(spy.count(), 0);
  }

  void testSourceDoesNotContainSyntheticOptimizationData() {
    QFile source(QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/services/FreeRunOptimizationService.cpp"));
    QVERIFY(source.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString text = QString::fromUtf8(source.readAll());

    QVERIFY2(!text.contains(QStringLiteral("emit optimizationCompleted(result)")),
             "Free Run optimization must not report completion without backend evidence");
    QVERIFY2(!text.contains(QStringLiteral("before[\"cycleTimeUs\"] = 1000")),
             "Free Run optimization must not retain synthetic cycle-time baselines");
    QVERIFY2(!text.contains(QStringLiteral("after[\"cycleTimeUs\"] = 500")),
             "Free Run optimization must not retain synthetic cycle-time targets");
    QVERIFY2(!text.contains(QStringLiteral("before[\"totalPdoBytes\"] = 256")),
             "Free Run optimization must not retain synthetic PDO mapping baselines");
    QVERIFY2(!text.contains(QStringLiteral("before[\"errorRecoveryTimeMs\"] = 500")),
             "Free Run optimization must not retain synthetic error-handling baselines");
    QVERIFY2(!text.contains(QStringLiteral("applied.applied = true")),
             "Free Run optimization must not mark applied without backend acknowledgement");
    QVERIFY2(!text.contains(QStringLiteral("emit optimizationApplied(applied)")),
             "Free Run optimization must not emit applied success without backend acknowledgement");
  }

  void testCycleTimeOptimizerNotNull() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationPlugin p(&client, &bus);
    QVERIFY(p.cycleTimeOptimizer() != nullptr);
  }

  void testCycleTimeOptimizerLabels() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationPlugin p(&client, &bus);
    auto *w = p.cycleTimeOptimizer();
    QVERIFY(w->cycleTimeLabel() != nullptr);
    QVERIFY(w->jitterLabel() != nullptr);
  }

  void testCycleTimeOptimizerUpdate() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationPlugin p(&client, &bus);
    auto *w = p.cycleTimeOptimizer();
    w->updateCurrentCycleTime(500.0, 15.0);
    QCOMPARE(w->cycleTimeLabel()->text(), QString("500 us"));
    QCOMPARE(w->jitterLabel()->text(), QString("15.0 us"));
  }

  void testCycleTimeOptimizerButton() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationPlugin p(&client, &bus);
    auto *w = p.cycleTimeOptimizer();
    QVERIFY(w->optimizeButton() != nullptr);
    QVERIFY(!w->optimizeButton()->isEnabled());
  }

  void testCycleTimeOptimizerSetOptimized() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationPlugin p(&client, &bus);
    auto *w = p.cycleTimeOptimizer();
    w->setOptimized();
    QVERIFY(!w->optimizeButton()->isEnabled());
    QCOMPARE(w->optimizeButton()->text(), QString("Optimization Applied"));
  }

  void testCycleTimeOptimizerShowResult() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationPlugin p(&client, &bus);
    auto *w = p.cycleTimeOptimizer();

    FreeRunOptimizationResult result;
    result.category = "Cycle Time";
    result.description = "test";
    result.before["cycleTimeUs"] = 1000;
    result.after["cycleTimeUs"] = 500;
    result.improvement = 50.0;

    w->showOptimizationResult(result);
    QVERIFY(w->optimizeButton()->isEnabled());
  }

  void testDataMappingOptimizerNotNull() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationPlugin p(&client, &bus);
    QVERIFY(p.dataMappingOptimizer() != nullptr);
  }

  void testDataMappingOptimizerLabels() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationPlugin p(&client, &bus);
    auto *w = p.dataMappingOptimizer();
    QVERIFY(w->totalBytesLabel() != nullptr);
    QVERIFY(w->entriesLabel() != nullptr);
  }

  void testDataMappingOptimizerUpdate() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationPlugin p(&client, &bus);
    auto *w = p.dataMappingOptimizer();
    w->updateCurrentMapping(192, 12, 0);
    QCOMPARE(w->totalBytesLabel()->text(), QString("192 bytes"));
    QCOMPARE(w->entriesLabel()->text(), QString("12"));
  }

  void testDataMappingOptimizerButton() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationPlugin p(&client, &bus);
    auto *w = p.dataMappingOptimizer();
    QVERIFY(w->optimizeButton() != nullptr);
    QVERIFY(!w->optimizeButton()->isEnabled());
  }

  void testDataMappingOptimizerSetOptimized() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationPlugin p(&client, &bus);
    auto *w = p.dataMappingOptimizer();
    w->setOptimized();
    QVERIFY(!w->optimizeButton()->isEnabled());
    QCOMPARE(w->optimizeButton()->text(), QString("Optimization Applied"));
  }

  void testDataMappingOptimizerShowResult() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationPlugin p(&client, &bus);
    auto *w = p.dataMappingOptimizer();

    FreeRunOptimizationResult result;
    result.category = "Data Mapping";
    result.description = "test";
    result.before["totalPdoBytes"] = 256;
    result.after["totalPdoBytes"] = 192;
    result.improvement = 25.0;

    w->showOptimizationResult(result);
    QVERIFY(w->optimizeButton()->isEnabled());
  }

  void testExportButton() {
    EcatClient client;
    EventBus bus;
    FreeRunOptimizationPlugin p(&client, &bus);
    QVERIFY(p.exportButton() != nullptr);
  }

  void testExportReportReportsPersistenceOutcome() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    EcatClient client;
    EventBus bus;
    FreeRunOptimizationPlugin p(&client, &bus);

    const QString path = dir.filePath("free_run_optimization.md");
    QVERIFY(p.exportReportToFile(path));
    QVERIFY(QFile::exists(path));

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString markdown = QString::fromUtf8(file.readAll());
    QVERIFY(markdown.startsWith(QStringLiteral("# Free Run Optimization Report\n")));
    QVERIFY(markdown.contains(QStringLiteral("No optimizations applied yet.")));

    QTest::failOnWarning(QRegularExpression(
        QStringLiteral("QFSFileEngine::open: No file name specified")));
    QVERIFY(!p.exportReportToFile(QString()));
    QVERIFY(!p.exportReportToFile(dir.path()));
  }
};

QTEST_MAIN(FreeRunOptimizationPluginTest)
#include "freerunoptimization_plugin_test.moc"
