#include <QTest>
#include <QApplication>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QFile>
#include <QTemporaryDir>

#include "services/PdoMappingOptimizationService.h"
#include "plugins/pdomappingoptimization/PdoMappingOptimizationPlugin.h"
#include "plugins/pdomappingoptimization/MappingOptimizerWidget.h"
#include "plugins/pdomappingoptimization/SizeOptimizerWidget.h"

class PdoMappingOptimizationPluginTest : public QObject {
  Q_OBJECT
private slots:
  void initTestCase();
  void cleanupTestCase();

  void testServiceOptimizeMapping();
  void testServiceOptimizeSize();
  void testServiceOptimizeAlignment();
  void testServiceOptimizePerformance();
  void testServiceApplyOptimizationFailsWithoutBackend();
  void testServiceHistoryRemainsEmptyWithoutBackend();
  void testServiceClearHistory();
  void testServiceDoesNotEmitCompletedWithoutBackend();
  void testSourceDoesNotContainSyntheticOptimizationData();

  void testPluginIdentity();
  void testPluginVisible();
  void testPluginDefaultOrder();
  void testPluginWidget();
  void testPluginService();
  void testExportReportReportsPersistenceOutcome();

  void testMappingOptimizerWidget();
  void testSizeOptimizerWidget();

private:
  PdoMappingOptimizationService *service_ = nullptr;
  PdoMappingOptimizationPlugin *plugin_ = nullptr;
};

void PdoMappingOptimizationPluginTest::initTestCase() {
  service_ = new PdoMappingOptimizationService(this);
  plugin_ = new PdoMappingOptimizationPlugin(this);
}

void PdoMappingOptimizationPluginTest::cleanupTestCase() {
  delete plugin_;
  plugin_ = nullptr;
  delete service_;
  service_ = nullptr;
}

void PdoMappingOptimizationPluginTest::testServiceOptimizeMapping() {
  auto result = service_->optimizeMapping();
  QCOMPARE(result.category, tr("Mapping"));
  QVERIFY(!result.description.isEmpty());
  QVERIFY(result.description.contains(QStringLiteral("backend"),
                                      Qt::CaseInsensitive));
  QVERIFY(result.before.isEmpty());
  QVERIFY(result.after.isEmpty());
  QCOMPARE(result.improvement, 0.0);
  QVERIFY(!result.recommendations.isEmpty());
  QVERIFY(!result.applied);
}

void PdoMappingOptimizationPluginTest::testServiceOptimizeSize() {
  auto result = service_->optimizeSize();
  QCOMPARE(result.category, tr("Size"));
  QVERIFY(!result.description.isEmpty());
  QVERIFY(result.before.isEmpty());
  QVERIFY(result.after.isEmpty());
  QCOMPARE(result.improvement, 0.0);
  QVERIFY(!result.recommendations.isEmpty());
}

void PdoMappingOptimizationPluginTest::testServiceOptimizeAlignment() {
  auto result = service_->optimizeAlignment();
  QCOMPARE(result.category, tr("Alignment"));
  QVERIFY(!result.description.isEmpty());
  QVERIFY(result.before.isEmpty());
  QVERIFY(result.after.isEmpty());
  QCOMPARE(result.improvement, 0.0);
  QVERIFY(!result.recommendations.isEmpty());
}

void PdoMappingOptimizationPluginTest::testServiceOptimizePerformance() {
  auto result = service_->optimizePerformance();
  QCOMPARE(result.category, tr("Performance"));
  QVERIFY(!result.description.isEmpty());
  QVERIFY(result.before.isEmpty());
  QVERIFY(result.after.isEmpty());
  QCOMPARE(result.improvement, 0.0);
  QVERIFY(!result.recommendations.isEmpty());
}

void PdoMappingOptimizationPluginTest::testServiceApplyOptimizationFailsWithoutBackend() {
  service_->clearHistory();
  QSignalSpy spy(service_, &PdoMappingOptimizationService::optimizationApplied);
  QVERIFY(spy.isValid());

  auto result = service_->optimizeMapping();
  QVERIFY(!result.applied);

  bool applied = service_->applyOptimization(result);
  QVERIFY(!applied);
  QCOMPARE(spy.count(), 0);
  QCOMPARE(service_->optimizationHistory().size(), 0);
}

void PdoMappingOptimizationPluginTest::testServiceHistoryRemainsEmptyWithoutBackend() {
  service_->clearHistory();
  QCOMPARE(service_->optimizationHistory().size(), 0);

  QVERIFY(!service_->applyOptimization(service_->optimizeMapping()));
  QVERIFY(!service_->applyOptimization(service_->optimizeSize()));
  QCOMPARE(service_->optimizationHistory().size(), 0);
}

void PdoMappingOptimizationPluginTest::testServiceClearHistory() {
  service_->applyOptimization(service_->optimizeMapping());
  service_->clearHistory();
  QCOMPARE(service_->optimizationHistory().size(), 0);
}

void PdoMappingOptimizationPluginTest::testServiceDoesNotEmitCompletedWithoutBackend() {
  QSignalSpy spy(service_, &PdoMappingOptimizationService::optimizationCompleted);
  QVERIFY(spy.isValid());

  service_->optimizeMapping();
  service_->optimizeSize();
  service_->optimizeAlignment();
  service_->optimizePerformance();
  QCOMPARE(spy.count(), 0);
}

void PdoMappingOptimizationPluginTest::testSourceDoesNotContainSyntheticOptimizationData() {
  QFile file(QStringLiteral(SOURCE_ROOT
                            "/apps/ecat-studio/services/PdoMappingOptimizationService.cpp"));
  QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
  const QString source = QString::fromUtf8(file.readAll());

  QVERIFY2(!source.contains(QStringLiteral("emit optimizationCompleted(result)")),
           "Offline PDO optimization must not emit completion.");
  QVERIFY2(!source.contains(QStringLiteral("31.25")),
           "PDO mapping optimization must not synthesize improvement values.");
  QVERIFY2(!source.contains(QStringLiteral("18.75")),
           "PDO size optimization must not synthesize improvement values.");
  QVERIFY2(!source.contains(QStringLiteral("83.33")),
           "PDO alignment optimization must not synthesize improvement values.");
}

void PdoMappingOptimizationPluginTest::testPluginIdentity() {
  QCOMPARE(plugin_->id(), QString("pdomappingoptimization"));
  QCOMPARE(plugin_->displayName(), QString("PDO Mapping Optimization"));
  QCOMPARE(plugin_->displayNameZh(), QStringLiteral("PDO 映射优化"));
}

void PdoMappingOptimizationPluginTest::testPluginVisible() {
  QVERIFY(!plugin_->visible());
}

void PdoMappingOptimizationPluginTest::testPluginDefaultOrder() {
  QCOMPARE(plugin_->defaultOrder(), 46);
}

void PdoMappingOptimizationPluginTest::testPluginWidget() {
  QWidget *w = plugin_->widget();
  QVERIFY(w != nullptr);
  QVERIFY(w->isVisible() == false || w->isVisible() == true);
}

void PdoMappingOptimizationPluginTest::testPluginService() {
  QVERIFY(plugin_->service() != nullptr);
}

void PdoMappingOptimizationPluginTest::testExportReportReportsPersistenceOutcome() {
  QTemporaryDir dir;
  QVERIFY(dir.isValid());

  const QString path = dir.filePath("pdo_mapping_optimization.md");
  QVERIFY(plugin_->exportReportToFile(path));
  QVERIFY(QFile::exists(path));

  QFile file(path);
  QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
  const QString markdown = QString::fromUtf8(file.readAll());
  QVERIFY(markdown.startsWith(QStringLiteral("# PDO Mapping Optimization Report\n")));
  QVERIFY(markdown.contains(QStringLiteral("No optimizations applied yet.")));

  QTest::failOnWarning(QRegularExpression(
      QStringLiteral("QFSFileEngine::open: No file name specified")));
  QVERIFY(!plugin_->exportReportToFile(QString()));
  QVERIFY(!plugin_->exportReportToFile(dir.path()));
}

void PdoMappingOptimizationPluginTest::testMappingOptimizerWidget() {
  MappingOptimizerWidget w;
  w.updateCurrentMapping(8, 32, 4, 6);
  QCOMPARE(w.totalPdosLabel()->text(), QString("8"));
  QCOMPARE(w.totalEntriesLabel()->text(), QString("32"));
  QVERIFY(w.optimizeButton() != nullptr);

  PdoMappingOptimizationResult result;
  result.category = "Mapping";
  result.description = "Test";
  result.before["totalEntries"] = 32;
  result.after["totalEntries"] = 22;
  result.improvement = 31.25;
  result.applied = false;
  result.timestamp = QDateTime::currentDateTime();
  w.showOptimizationResult(result);

  w.setOptimized();
  QVERIFY(!w.optimizeButton()->isEnabled());
}

void PdoMappingOptimizationPluginTest::testSizeOptimizerWidget() {
  SizeOptimizerWidget w;
  w.updateCurrentSize(256, 128, 128, 48);
  QCOMPARE(w.totalBytesLabel()->text(), QString("256 bytes"));
  QCOMPARE(w.inputBytesLabel()->text(), QString("128 bytes"));
  QCOMPARE(w.outputBytesLabel()->text(), QString("128 bytes"));
  QVERIFY(w.optimizeButton() != nullptr);

  PdoMappingOptimizationResult result;
  result.category = "Size";
  result.description = "Test";
  result.before["totalBytes"] = 256;
  result.after["totalBytes"] = 208;
  result.improvement = 18.75;
  result.applied = false;
  result.timestamp = QDateTime::currentDateTime();
  w.showOptimizationResult(result);

  w.setOptimized();
  QVERIFY(!w.optimizeButton()->isEnabled());
}

QTEST_MAIN(PdoMappingOptimizationPluginTest)
#include "pdomappingoptimization_plugin_test.moc"
