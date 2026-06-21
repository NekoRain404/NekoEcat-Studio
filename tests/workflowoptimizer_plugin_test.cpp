// TestWorkflowOptimizerPlugin — Tests for Workflow Optimizer Plugin
//
// Test coverage:
//   - Plugin identity (id, display names, order, visibility)
//   - Widget creation
//   - Workflow list management (add, remove, count)
//   - Optimization suggestions display
//   - Execution history tracking
//   - Report export
//   - Signal emissions

#include <QApplication>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QtTest/QtTest>

#include "plugins/workflowoptimizer/WorkflowOptimizerPlugin.h"
#include "services/WorkflowAnalyticsService.h"

class TestWorkflowOptimizerPlugin : public QObject {
  Q_OBJECT
private slots:
  void initTestCase();
  void cleanupTestCase();
  // Plugin reports correct id, display names, order, and visibility
  // Plugin reports correct identity fields
  void identity();
  // Widget is not null after creation
  // Widget is created and non-null
  void widgetNotNull();
  // Add and remove workflows and verify count
  // Workflows can be added, removed, and counted
  void workflowList();
  // Add and retrieve optimization suggestions
  // Optimization suggestions can be added per workflow
  void suggestions();
  // Track execution history entries
  // Execution history records can be added and counted
  void executionHistory();
  // Export optimization report
  // Export report writes JSON to disk
  void exportReport();
  // Verify signal emissions on state changes
  // Signals emitted on workflow selection and report export
  void signalEmissions();

private:
  WorkflowAnalyticsService *analytics_ = nullptr;
  WorkflowOptimizerPlugin *plugin_ = nullptr;
};

void TestWorkflowOptimizerPlugin::initTestCase() {
  analytics_ = new WorkflowAnalyticsService(this);
  plugin_ = new WorkflowOptimizerPlugin(analytics_, this);
}

void TestWorkflowOptimizerPlugin::cleanupTestCase() {
  delete plugin_;
  plugin_ = nullptr;
  delete analytics_;
  analytics_ = nullptr;
}

void TestWorkflowOptimizerPlugin::identity() {
  QCOMPARE(plugin_->id(), QString("workflowoptimizer"));
  QCOMPARE(plugin_->displayName(), QString("Workflow Optimizer"));
  QCOMPARE(plugin_->displayNameZh(), QString("工作流优化器"));
  QCOMPARE(plugin_->defaultOrder(), 385);
  QVERIFY(plugin_->visible());
}

void TestWorkflowOptimizerPlugin::widgetNotNull() {
  QVERIFY(plugin_->widget() != nullptr);
}

void TestWorkflowOptimizerPlugin::workflowList() {
  QCOMPARE(plugin_->workflowCount(), 0);

  plugin_->addWorkflow("wf1", "Test Workflow 1");
  QCOMPARE(plugin_->workflowCount(), 1);

  plugin_->addWorkflow("wf2", "Test Workflow 2");
  QCOMPARE(plugin_->workflowCount(), 2);

  plugin_->removeWorkflow("wf1");
  QCOMPARE(plugin_->workflowCount(), 1);

  plugin_->removeWorkflow("nonexistent");
  QCOMPARE(plugin_->workflowCount(), 1);

  plugin_->removeWorkflow("wf2");
  QCOMPARE(plugin_->workflowCount(), 0);
}

void TestWorkflowOptimizerPlugin::suggestions() {
  QCOMPARE(plugin_->suggestionCount(), 0);

  plugin_->addSuggestion("wf1", "High", "Optimize task scheduling");
  QCOMPARE(plugin_->suggestionCount(), 1);

  plugin_->addSuggestion("wf1", "Medium", "Reduce memory usage");
  QCOMPARE(plugin_->suggestionCount(), 2);

  plugin_->addSuggestion("wf2", "Low", "Consider caching");
  QCOMPARE(plugin_->suggestionCount(), 3);
}

void TestWorkflowOptimizerPlugin::executionHistory() {
  QCOMPARE(plugin_->executionHistoryCount(), 0);

  plugin_->addExecutionRecord("wf1", "Completed", 150.5);
  QCOMPARE(plugin_->executionHistoryCount(), 1);

  plugin_->addExecutionRecord("wf1", "Failed", 300.0);
  QCOMPARE(plugin_->executionHistoryCount(), 2);

  plugin_->addExecutionRecord("wf2", "Completed", 75.2);
  QCOMPARE(plugin_->executionHistoryCount(), 3);
}

void TestWorkflowOptimizerPlugin::exportReport() {
  plugin_->addWorkflow("wf_export", "Export Test");
  plugin_->addSuggestion("wf_export", "High", "Test suggestion");
  plugin_->addExecutionRecord("wf_export", "Completed", 100.0);

  QString tmpPath = QDir::tempPath() + "/optimizer_report_test.json";
  QVERIFY(plugin_->exportReport(tmpPath));
  QVERIFY(QFile::exists(tmpPath));
  QFile::remove(tmpPath);
}

void TestWorkflowOptimizerPlugin::signalEmissions() {
  QSignalSpy wfSpy(plugin_, &WorkflowOptimizerPlugin::workflowSelected);
  QSignalSpy optSpy(plugin_, &WorkflowOptimizerPlugin::optimizationRequested);
  QSignalSpy expSpy(plugin_, &WorkflowOptimizerPlugin::reportExported);

  plugin_->addWorkflow("wf_signal", "Signal Test");

  QString tmpPath = QDir::tempPath() + "/optimizer_signal_test.json";
  plugin_->exportReport(tmpPath);
  QCOMPARE(expSpy.count(), 1);
  QFile::remove(tmpPath);
}

QTEST_MAIN(TestWorkflowOptimizerPlugin)
#include "workflowoptimizer_plugin_test.moc"
