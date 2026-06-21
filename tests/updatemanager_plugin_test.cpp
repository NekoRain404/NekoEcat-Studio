// TestUpdateManagerPlugin — Tests for Update Manager Plugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Available updates and history table structure
//   - Add/remove available updates
//   - Apply and rollback updates
//   - Clear history
//   - Export update log
//   - Signal emissions (available, applied, rollback)

#include <QApplication>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QtTest/QtTest>

#include "plugins/updatemanager/UpdateManagerPlugin.h"

class TestUpdateManagerPlugin : public QObject {
  Q_OBJECT
private slots:
  // Setup: create plugin instance
  // Set up plugin instance
  void initTestCase();
  // Clean up plugin instance
  void cleanupTestCase();
  // Verify plugin id, display names, and visibility
  void identity();
  // Widget is created successfully
  void widgetNotNull();
  // Available updates table has 5 columns
  void availableTable();
  // History table has 6 columns
  void historyTable();
  // Add and remove available updates, verify counts
  void addAndRemoveAvailable();
  // Apply an update and verify signal and history
  void applyUpdate();
  // Rollback an applied update and verify signal
  void rollbackUpdate();
  // Clear history and verify empty state
  void clearHistory();
  // Export update log to JSON file
  void exportLog();
  // Verify signals fire for available, applied, and rollback events
  void signalEmissions();

private:
  UpdateManagerPlugin *plugin_ = nullptr;
};

void TestUpdateManagerPlugin::initTestCase() {
  plugin_ = new UpdateManagerPlugin(this);
}

void TestUpdateManagerPlugin::cleanupTestCase() {
  delete plugin_;
  plugin_ = nullptr;
}

void TestUpdateManagerPlugin::identity() {
  QCOMPARE(plugin_->id(), QString("updatemanager"));
  QCOMPARE(plugin_->displayName(), QString("Update Manager"));
  QCOMPARE(plugin_->displayNameZh(), QString("更新管理器"));
  QVERIFY(plugin_->visible());
}

void TestUpdateManagerPlugin::widgetNotNull() {
  QVERIFY(plugin_->widget() != nullptr);
}

void TestUpdateManagerPlugin::availableTable() {
  QVERIFY(plugin_->availableTable() != nullptr);
  QCOMPARE(plugin_->availableTable()->columnCount(), 5);
}

void TestUpdateManagerPlugin::historyTable() {
  QVERIFY(plugin_->historyTable() != nullptr);
  QCOMPARE(plugin_->historyTable()->columnCount(), 6);
}

void TestUpdateManagerPlugin::addAndRemoveAvailable() {
  QCOMPARE(plugin_->availableCount(), 0);

  UpdateEntry e1;
  e1.name = "firmware-1";
  e1.currentVersion = "1.0.0";
  e1.availableVersion = "2.0.0";
  e1.description = "Major update";
  e1.status = "Available";
  plugin_->addAvailableUpdate(e1);
  QCOMPARE(plugin_->availableCount(), 1);

  UpdateEntry e2;
  e2.name = "firmware-2";
  e2.currentVersion = "1.5.0";
  e2.availableVersion = "2.0.0";
  e2.description = "Minor update";
  e2.status = "Available";
  plugin_->addAvailableUpdate(e2);
  QCOMPARE(plugin_->availableCount(), 2);

  QCOMPARE(plugin_->availableTable()->rowCount(), 2);

  plugin_->removeAvailableUpdate(0);
  QCOMPARE(plugin_->availableCount(), 1);

  plugin_->removeAvailableUpdate(0);
  QCOMPARE(plugin_->availableCount(), 0);
}

void TestUpdateManagerPlugin::applyUpdate() {
  UpdateEntry e;
  e.name = "test-firmware";
  e.currentVersion = "1.0.0";
  e.availableVersion = "2.0.0";
  e.description = "Test update";
  e.status = "Available";
  plugin_->addAvailableUpdate(e);

  QSignalSpy spy(plugin_, &UpdateManagerPlugin::updateApplied);
  plugin_->applyUpdate(0);
  QCOMPARE(spy.count(), 1);
  QCOMPARE(spy.at(0).at(1).toString(), QString("Success"));
  QCOMPARE(plugin_->historyCount(), 1);
  QCOMPARE(plugin_->historyTable()->rowCount(), 1);

  plugin_->removeAvailableUpdate(0);
  plugin_->clearHistory();
}

void TestUpdateManagerPlugin::rollbackUpdate() {
  UpdateEntry e;
  e.name = "rollback-test";
  e.currentVersion = "1.0.0";
  e.availableVersion = "2.0.0";
  e.description = "Rollback test";
  e.status = "Available";
  plugin_->addAvailableUpdate(e);
  plugin_->applyUpdate(0);

  QSignalSpy spy(plugin_, &UpdateManagerPlugin::rollbackRequested);
  plugin_->rollbackUpdate(0);
  QCOMPARE(spy.count(), 1);
  QCOMPARE(plugin_->historyCount(), 2);
  QCOMPARE(plugin_->historyTable()->rowCount(), 2);

  plugin_->removeAvailableUpdate(0);
  plugin_->clearHistory();
}

void TestUpdateManagerPlugin::clearHistory() {
  UpdateEntry e;
  e.name = "clear-test";
  e.currentVersion = "1.0.0";
  e.availableVersion = "2.0.0";
  e.description = "Clear test";
  e.status = "Available";
  plugin_->addAvailableUpdate(e);
  plugin_->applyUpdate(0);

  QCOMPARE(plugin_->historyCount(), 1);
  plugin_->clearHistory();
  QCOMPARE(plugin_->historyCount(), 0);
  QCOMPARE(plugin_->historyTable()->rowCount(), 0);

  plugin_->removeAvailableUpdate(0);
}

void TestUpdateManagerPlugin::exportLog() {
  UpdateEntry e;
  e.name = "export-test";
  e.currentVersion = "1.0.0";
  e.availableVersion = "2.0.0";
  e.description = "Export test";
  e.status = "Available";
  plugin_->addAvailableUpdate(e);
  plugin_->applyUpdate(0);

  QString tmpPath = QDir::tempPath() + "/update_log.json";
  QVERIFY(plugin_->exportUpdateLog(tmpPath));
  QVERIFY(QFile::exists(tmpPath));

  QFile::remove(tmpPath);
  plugin_->removeAvailableUpdate(0);
  plugin_->clearHistory();
}

void TestUpdateManagerPlugin::signalEmissions() {
  QSignalSpy availableSpy(plugin_, &UpdateManagerPlugin::updateAvailable);
  QSignalSpy appliedSpy(plugin_, &UpdateManagerPlugin::updateApplied);
  QSignalSpy rollbackSpy(plugin_, &UpdateManagerPlugin::rollbackRequested);

  UpdateEntry e;
  e.name = "signal-test";
  e.currentVersion = "1.0.0";
  e.availableVersion = "2.0.0";
  e.description = "Signal test";
  e.status = "Available";
  plugin_->addAvailableUpdate(e);
  QCOMPARE(availableSpy.count(), 1);

  plugin_->applyUpdate(0);
  QCOMPARE(appliedSpy.count(), 1);

  plugin_->rollbackUpdate(0);
  QCOMPARE(rollbackSpy.count(), 1);

  plugin_->removeAvailableUpdate(0);
  plugin_->clearHistory();
}

QTEST_MAIN(TestUpdateManagerPlugin)
#include "updatemanager_plugin_test.moc"
