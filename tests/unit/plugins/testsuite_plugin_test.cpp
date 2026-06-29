// TestTestSuitePlugin — Tests for TestSuitePlugin (full)
//
// Test coverage:
//   - Plugin identity, widget, and sub-widgets
//   - Test list management (add, remove, clear)
//   - Test execution (single, all, stop)
//   - Test result updates and counts
//   - Report generation and export
//   - Signal emissions

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QTreeWidget>
#include <QTemporaryDir>
#include <QRegularExpression>
#include <QtTest/QtTest>

#include "plugins/testsuite/TestSuitePlugin.h"

class TestTestSuitePlugin : public QObject {
  Q_OBJECT
private slots:
  // Setup: create plugin instance
  void initTestCase();
  // Teardown: destroy plugin instance
  void cleanupTestCase();
  // Plugin id, name, Chinese name, order, and visibility
  void identity();
  // Widget pointer is non-null
  void widgetNotNull();
  // Test list tree widget exists and has items
  void testList();
  // Results table has 4 columns
  void testResults();
  // Report text widget is read-only
  void testReport();
  // Runner status label shows "Ready"
  void runnerStatus();
  // Add and remove tests updates count and table rows
  void addAndRemoveTests();
  // Clear all tests resets count and table
  void clearTests();
  // Run single test fails closed without a test backend
  void runSingleTest();
  // Run all tests fails closed without a test backend
  void runAllTests();
  // Stop tests updates runner status
  void stopTests();
  // Update result populates table columns
  void updateTestResult();
  // Passed/failed/skip counts reflect result statuses
  void testCounts();
  // Generate report produces non-empty text with results
  void reportGeneration();
  // Export report writes JSON file to disk
  void exportReport();
  // All plugin signals fire on corresponding actions
  void signalEmissions();
  // Source must not mint passed test results without a backend
  void sourceDoesNotMintSyntheticPassedResults();

private:
  TestSuitePlugin *plugin_ = nullptr;
};

void TestTestSuitePlugin::initTestCase() {
  plugin_ = new TestSuitePlugin(this);
}

void TestTestSuitePlugin::cleanupTestCase() {
  delete plugin_;
  plugin_ = nullptr;
}

void TestTestSuitePlugin::identity() {
  QCOMPARE(plugin_->id(), QString("testsuite"));
  QCOMPARE(plugin_->displayName(), QString("Test Suite"));
  QCOMPARE(plugin_->displayNameZh(), QString("测试套件"));
  QCOMPARE(plugin_->defaultOrder(), 255);
  QVERIFY(!plugin_->visible());
}

void TestTestSuitePlugin::widgetNotNull() {
  QVERIFY(plugin_->widget() != nullptr);
}

void TestTestSuitePlugin::testList() {
  QVERIFY(plugin_->testList() != nullptr);
  QVERIFY(plugin_->testList()->topLevelItemCount() > 0);
}

void TestTestSuitePlugin::testResults() {
  QVERIFY(plugin_->testResults() != nullptr);
  QCOMPARE(plugin_->testResults()->columnCount(), 4);
}

void TestTestSuitePlugin::testReport() {
  QVERIFY(plugin_->testReport() != nullptr);
  QVERIFY(plugin_->testReport()->isReadOnly());
}

void TestTestSuitePlugin::runnerStatus() {
  QVERIFY(plugin_->runnerStatus() != nullptr);
  QVERIFY(plugin_->runnerStatus()->text().contains("Ready"));
}

void TestTestSuitePlugin::addAndRemoveTests() {
  plugin_->clearTests();
  QCOMPARE(plugin_->testCount(), 0);

  plugin_->addTest("EtherCAT", "Connection Test", "Test EtherCAT connection");
  QCOMPARE(plugin_->testCount(), 1);

  plugin_->addTest("System", "Startup Test");
  QCOMPARE(plugin_->testCount(), 2);

  QCOMPARE(plugin_->testResults()->rowCount(), 2);

  plugin_->removeTest("test_1");
  QCOMPARE(plugin_->testCount(), 1);

  plugin_->removeTest("nonexistent");
  QCOMPARE(plugin_->testCount(), 1);

  plugin_->clearTests();
}

void TestTestSuitePlugin::clearTests() {
  plugin_->addTest("A", "TestA");
  plugin_->addTest("B", "TestB");
  QCOMPARE(plugin_->testCount(), 2);

  plugin_->clearTests();
  QCOMPARE(plugin_->testCount(), 0);
  QCOMPARE(plugin_->testResults()->rowCount(), 0);
}

void TestTestSuitePlugin::runSingleTest() {
  plugin_->clearTests();
  plugin_->addTest("Test", "SingleTest");

  QSignalSpy startSpy(plugin_, &TestSuitePlugin::testStarted);
  QSignalSpy finishSpy(plugin_, &TestSuitePlugin::testFinished);

  plugin_->runTest("test_1");
  QCOMPARE(startSpy.count(), 1);
  QCOMPARE(finishSpy.count(), 0);

  QCOMPARE(plugin_->testResults()->item(0, 1)->text(), QString("Blocked"));
  QVERIFY(plugin_->testResults()->item(0, 2)->text().contains("backend"));

  plugin_->clearTests();
}

void TestTestSuitePlugin::runAllTests() {
  plugin_->clearTests();
  plugin_->addTest("Cat1", "Test1");
  plugin_->addTest("Cat2", "Test2");

  QSignalSpy allFinishedSpy(plugin_, &TestSuitePlugin::allTestsFinished);
  plugin_->runAllTests();
  QCOMPARE(allFinishedSpy.count(), 1);
  QCOMPARE(plugin_->passedCount(), 0);
  QCOMPARE(plugin_->failedCount(), 0);
  QCOMPARE(plugin_->skippedCount(), 2);
  QCOMPARE(plugin_->runnerStatus()->text().contains("Finished"), true);

  plugin_->clearTests();
}

void TestTestSuitePlugin::stopTests() {
  plugin_->clearTests();
  plugin_->addTest("Test", "StopTest");

  plugin_->stopTests();
  QVERIFY(plugin_->runnerStatus()->text().contains("Stopped"));

  plugin_->clearTests();
}

void TestTestSuitePlugin::updateTestResult() {
  plugin_->clearTests();
  plugin_->addTest("Test", "UpdateTest");

  plugin_->updateTestResult("test_1", "Failed", "Error occurred", 500);
  QCOMPARE(plugin_->testResults()->item(0, 1)->text(), QString("Failed"));
  QCOMPARE(plugin_->testResults()->item(0, 2)->text(), QString("Error occurred"));
  QCOMPARE(plugin_->testResults()->item(0, 3)->text(), QString("500 ms"));

  plugin_->clearTests();
}

void TestTestSuitePlugin::testCounts() {
  plugin_->clearTests();
  plugin_->addTest("A", "Test1");
  plugin_->addTest("B", "Test2");
  plugin_->addTest("C", "Test3");

  plugin_->updateTestResult("test_1", "Passed", "OK");
  plugin_->updateTestResult("test_2", "Failed", "Error");
  plugin_->updateTestResult("test_3", "Skipped", "N/A");

  QCOMPARE(plugin_->passedCount(), 1);
  QCOMPARE(plugin_->failedCount(), 1);
  QCOMPARE(plugin_->skippedCount(), 1);

  plugin_->clearTests();
}

void TestTestSuitePlugin::reportGeneration() {
  plugin_->clearTests();
  plugin_->addTest("Test", "ReportTest");
  plugin_->updateTestResult("test_1", "Passed", "OK");

  QSignalSpy reportSpy(plugin_, &TestSuitePlugin::reportGenerated);
  plugin_->generateReport();
  QCOMPARE(reportSpy.count(), 1);

  QVERIFY(!plugin_->testReport()->toPlainText().isEmpty());
  QVERIFY(plugin_->testReport()->toPlainText().contains("Passed"));

  plugin_->clearTests();
}

void TestTestSuitePlugin::exportReport() {
  plugin_->clearTests();
  plugin_->addTest("Test", "ExportTest");
  plugin_->updateTestResult("test_1", "Passed", "OK");

  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  const QString tmpPath = dir.filePath("testsuite_report.json");
  QVERIFY(plugin_->exportReport(tmpPath));
  QVERIFY(QFile::exists(tmpPath));

  QTest::failOnWarning(QRegularExpression(
      QStringLiteral("QFSFileEngine::open: No file name specified")));
  QVERIFY(!plugin_->exportReport(QString()));
  QVERIFY(!plugin_->exportReport(dir.path()));

  plugin_->clearTests();
}

void TestTestSuitePlugin::signalEmissions() {
  QSignalSpy addSpy(plugin_, &TestSuitePlugin::testAdded);
  QSignalSpy removeSpy(plugin_, &TestSuitePlugin::testRemoved);
  QSignalSpy startSpy(plugin_, &TestSuitePlugin::testStarted);
  QSignalSpy finishSpy(plugin_, &TestSuitePlugin::testFinished);

  plugin_->addTest("Test", "SignalTest");
  QCOMPARE(addSpy.count(), 1);
  QCOMPARE(addSpy.at(0).at(1).toString(), QString("SignalTest"));

  plugin_->runTest("test_1");
  QCOMPARE(startSpy.count(), 1);
  QCOMPARE(finishSpy.count(), 0);

  plugin_->removeTest("test_1");
  QCOMPARE(removeSpy.count(), 1);

  plugin_->clearTests();
}

void TestTestSuitePlugin::sourceDoesNotMintSyntheticPassedResults() {
  QFile file(QStringLiteral(SOURCE_ROOT
                            "/apps/ecat-studio/plugins/testsuite/TestSuitePlugin.cpp"));
  QVERIFY2(file.open(QIODevice::ReadOnly | QIODevice::Text),
           qPrintable(file.errorString()));
  const QString source = QString::fromUtf8(file.readAll());

  QVERIFY2(!source.contains(QStringLiteral("tests_[i].status = \"Passed\"")),
           "Test suite UI must not mark tests passed without a test backend");
  QVERIFY2(!source.contains(QStringLiteral("tests_[i].result = \"OK\"")),
           "Test suite UI must not synthesize OK results without a test backend");
  QVERIFY2(!source.contains(QStringLiteral("emit testFinished(testId, \"Passed\")")),
           "Test suite UI must not emit Passed without a test backend");
}

QTEST_MAIN(TestTestSuitePlugin)
#include "testsuite_plugin_test.moc"
