// TestAIAssistantPlugin — Tests for AIAssistantPlugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation and sub-widgets
//   - Predictions, anomalies, optimizations, and patterns CRUD
//   - Export AI report
//   - Signal emissions

#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QTreeWidget>
#include <QtTest/QtTest>

#include "plugins/aiassistant/AIAssistantPlugin.h"

class TestAIAssistantPlugin : public QObject {
  Q_OBJECT
private slots:
  void initTestCase();
  void cleanupTestCase();
  // Verify plugin id, display names, and default order
  void identity();
  // Verify widget is not null
  void widgetNotNull();
  // Verify predictions table widget exists
  void predictionsTable();
  // Verify anomalies tree widget exists
  void anomaliesTree();
  // Verify optimizations view widget exists
  void optimizationsView();
  // Verify patterns table widget exists
  void patternsTable();
  // Add predictions and verify clear resets count
  void addAndClearPredictions();
  // Add anomalies and verify clear resets count
  void addAndClearAnomalies();
  // Verify optimizations text content
  void optimizationsText();
  // Add patterns and verify clear resets count
  void addAndClearPatterns();
  // Verify AI report export functionality
  void exportAIReport();
  // Verify plugin signals are emitted correctly
  void signalEmissions();

private:
  AIAssistantPlugin *plugin_ = nullptr;
};

void TestAIAssistantPlugin::initTestCase() {
  plugin_ = new AIAssistantPlugin(this);
}

void TestAIAssistantPlugin::cleanupTestCase() {
  delete plugin_;
  plugin_ = nullptr;
}

void TestAIAssistantPlugin::identity() {
  QCOMPARE(plugin_->id(), QString("aiassistant"));
  QCOMPARE(plugin_->displayName(), QString("AI Assistant"));
  QCOMPARE(plugin_->displayNameZh(), QString("AI助手"));
  QCOMPARE(plugin_->defaultOrder(), 330);
  QVERIFY(plugin_->visible());
}

void TestAIAssistantPlugin::widgetNotNull() {
  QVERIFY(plugin_->widget() != nullptr);
}

void TestAIAssistantPlugin::predictionsTable() {
  QVERIFY(plugin_->predictionsTable() != nullptr);
}

void TestAIAssistantPlugin::anomaliesTree() {
  QVERIFY(plugin_->anomaliesTree() != nullptr);
}

void TestAIAssistantPlugin::optimizationsView() {
  QVERIFY(plugin_->optimizationsView() != nullptr);
}

void TestAIAssistantPlugin::patternsTable() {
  QVERIFY(plugin_->patternsTable() != nullptr);
}

void TestAIAssistantPlugin::addAndClearPredictions() {
  plugin_->clearPredictions();
  QCOMPARE(plugin_->predictionCount(), 0);

  plugin_->addPrediction("Cycle Time", 0.95, "1.2ms");
  QCOMPARE(plugin_->predictionCount(), 1);

  plugin_->addPrediction("Jitter", 0.87, "50us");
  QCOMPARE(plugin_->predictionCount(), 2);

  plugin_->clearPredictions();
  QCOMPARE(plugin_->predictionCount(), 0);
}

void TestAIAssistantPlugin::addAndClearAnomalies() {
  plugin_->clearAnomalies();
  QCOMPARE(plugin_->anomalyCount(), 0);

  plugin_->addAnomaly("Slave-3", "High", "Unexpected latency spike");
  QCOMPARE(plugin_->anomalyCount(), 1);

  plugin_->addAnomaly("Master", "Low", "Minor jitter increase");
  QCOMPARE(plugin_->anomalyCount(), 2);

  plugin_->clearAnomalies();
  QCOMPARE(plugin_->anomalyCount(), 0);
}

void TestAIAssistantPlugin::optimizationsText() {
  plugin_->setOptimizationsText("Reduce polling interval to 100us");
  QCOMPARE(plugin_->optimizationsText(), QString("Reduce polling interval to 100us"));

  plugin_->setOptimizationsText("");
  QCOMPARE(plugin_->optimizationsText(), QString(""));
}

void TestAIAssistantPlugin::addAndClearPatterns() {
  plugin_->clearPatterns();
  QCOMPARE(plugin_->patternCount(), 0);

  plugin_->addPattern("Spike", "10ms", "0.95");
  QCOMPARE(plugin_->patternCount(), 1);

  plugin_->addPattern("Drift", "1s", "0.80");
  QCOMPARE(plugin_->patternCount(), 2);

  plugin_->clearPatterns();
  QCOMPARE(plugin_->patternCount(), 0);
}

void TestAIAssistantPlugin::exportAIReport() {
  QString tmpPath = QDir::tempPath() + "/ai_assistant_test_export.json";
  QVERIFY(plugin_->exportAIReport(tmpPath, "JSON"));
  QFile::remove(tmpPath);
}

void TestAIAssistantPlugin::signalEmissions() {
  QSignalSpy predSpy(plugin_, &AIAssistantPlugin::predictionAdded);
  QSignalSpy anomalySpy(plugin_, &AIAssistantPlugin::anomalyDetected);
  QSignalSpy optSpy(plugin_, &AIAssistantPlugin::optimizationsUpdated);
  QSignalSpy patternSpy(plugin_, &AIAssistantPlugin::patternRecognized);

  plugin_->addPrediction("Signal Metric", 0.99, "test value");
  QCOMPARE(predSpy.count(), 1);
  QCOMPARE(predSpy.at(0).at(0).toString(), QString("Signal Metric"));

  plugin_->addAnomaly("Signal Source", "Critical", "Test anomaly");
  QCOMPARE(anomalySpy.count(), 1);
  QCOMPARE(anomalySpy.at(0).at(0).toString(), QString("Signal Source"));

  plugin_->setOptimizationsText("test optimization");
  QCOMPARE(optSpy.count(), 1);

  plugin_->addPattern("test pattern", "1ms", "0.9");
  QCOMPARE(patternSpy.count(), 1);

  plugin_->clearPredictions();
  plugin_->clearAnomalies();
  plugin_->clearPatterns();
}

QTEST_MAIN(TestAIAssistantPlugin)
#include "aiassistant_plugin_test.moc"
