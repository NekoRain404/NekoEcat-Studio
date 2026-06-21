// TestCloudManagerPlugin — Tests for CloudManagerPlugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget and sub-widget creation
//   - Cloud connections CRUD
//   - Sync progress tracking
//   - Backup history management
//   - Monitoring view
//   - Export cloud report
//   - Signal emissions

#include <QApplication>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QTreeWidget>
#include <QtTest/QtTest>

#include "plugins/cloudmanager/CloudManagerPlugin.h"

class TestCloudManagerPlugin : public QObject {
  Q_OBJECT
private slots:
  void initTestCase();
  void cleanupTestCase();
  // Verify plugin id, display names, and default order
  void identity();
  // Verify widget is not null
  void widgetNotNull();
  // Verify connection table widget exists
  void connectionTable();
  // Verify sync progress widget exists
  void syncProgress();
  // Verify backup history widget exists
  void backupHistory();
  // Verify monitoring view widget exists
  void monitoringView();
  // Add and remove cloud connections
  void addAndRemoveConnections();
  // Clear all connections
  void clearConnections();
  // Verify sync progress value updates
  void syncProgressValue();
  // Add and clear backup entries
  void addAndClearBackupEntries();
  // Verify monitoring text content
  void monitoringText();
  // Verify cloud report export
  void exportCloudReport();
  // Verify plugin signals are emitted correctly
  void signalEmissions();

private:
  CloudManagerPlugin *plugin_ = nullptr;
};

void TestCloudManagerPlugin::initTestCase() {
  plugin_ = new CloudManagerPlugin(this);
}

void TestCloudManagerPlugin::cleanupTestCase() {
  delete plugin_;
  plugin_ = nullptr;
}

void TestCloudManagerPlugin::identity() {
  QCOMPARE(plugin_->id(), QString("cloudmanager"));
  QCOMPARE(plugin_->displayName(), QString("Cloud Manager"));
  QCOMPARE(plugin_->displayNameZh(), QString("云管理器"));
  QCOMPARE(plugin_->defaultOrder(), 310);
  QVERIFY(plugin_->visible());
}

void TestCloudManagerPlugin::widgetNotNull() {
  QVERIFY(plugin_->widget() != nullptr);
}

void TestCloudManagerPlugin::connectionTable() {
  QVERIFY(plugin_->connectionTable() != nullptr);
}

void TestCloudManagerPlugin::syncProgress() {
  QVERIFY(plugin_->syncProgress() != nullptr);
}

void TestCloudManagerPlugin::backupHistory() {
  QVERIFY(plugin_->backupHistory() != nullptr);
}

void TestCloudManagerPlugin::monitoringView() {
  QVERIFY(plugin_->monitoringView() != nullptr);
}

void TestCloudManagerPlugin::addAndRemoveConnections() {
  plugin_->clearConnections();
  QCOMPARE(plugin_->connectionCount(), 0);

  plugin_->addConnection("AWS", "aws.cloud.io");
  QCOMPARE(plugin_->connectionCount(), 1);

  plugin_->addConnection("Azure", "azure.cloud.io");
  QCOMPARE(plugin_->connectionCount(), 2);

  plugin_->removeConnection("AWS");
  QCOMPARE(plugin_->connectionCount(), 1);

  plugin_->removeConnection("NonExistent");
  QCOMPARE(plugin_->connectionCount(), 1);

  plugin_->clearConnections();
}

void TestCloudManagerPlugin::clearConnections() {
  plugin_->addConnection("A", "a.com");
  plugin_->addConnection("B", "b.com");
  QCOMPARE(plugin_->connectionCount(), 2);

  plugin_->clearConnections();
  QCOMPARE(plugin_->connectionCount(), 0);
}

void TestCloudManagerPlugin::syncProgressValue() {
  plugin_->setSyncProgress(0);
  QCOMPARE(plugin_->syncProgressValue(), 0);

  plugin_->setSyncProgress(50);
  QCOMPARE(plugin_->syncProgressValue(), 50);

  plugin_->setSyncProgress(100);
  QCOMPARE(plugin_->syncProgressValue(), 100);
}

void TestCloudManagerPlugin::addAndClearBackupEntries() {
  plugin_->clearBackupHistory();
  QCOMPARE(plugin_->backupCount(), 0);

  plugin_->addBackupEntry("2025-01-01T00:00:00", "Completed");
  QCOMPARE(plugin_->backupCount(), 1);

  plugin_->addBackupEntry("2025-01-02T00:00:00", "Failed");
  QCOMPARE(plugin_->backupCount(), 2);

  plugin_->clearBackupHistory();
  QCOMPARE(plugin_->backupCount(), 0);
}

void TestCloudManagerPlugin::monitoringText() {
  plugin_->setMonitoringText("CPU: 45%, Memory: 2GB");
  QCOMPARE(plugin_->monitoringText(), QString("CPU: 45%, Memory: 2GB"));

  plugin_->setMonitoringText("");
  QCOMPARE(plugin_->monitoringText(), QString(""));
}

void TestCloudManagerPlugin::exportCloudReport() {
  QString tmpPath = QDir::tempPath() + "/cloud_manager_test_export.json";
  QVERIFY(plugin_->exportCloudReport(tmpPath, "JSON"));
  QFile::remove(tmpPath);
}

void TestCloudManagerPlugin::signalEmissions() {
  QSignalSpy connAddSpy(plugin_, &CloudManagerPlugin::connectionAdded);
  QSignalSpy connRemoveSpy(plugin_, &CloudManagerPlugin::connectionRemoved);
  QSignalSpy syncSpy(plugin_, &CloudManagerPlugin::syncProgressChanged);
  QSignalSpy backupSpy(plugin_, &CloudManagerPlugin::backupAdded);

  plugin_->addConnection("SignalTest", "signal.test.io");
  QCOMPARE(connAddSpy.count(), 1);
  QCOMPARE(connAddSpy.at(0).at(0).toString(), QString("SignalTest"));

  plugin_->removeConnection("SignalTest");
  QCOMPARE(connRemoveSpy.count(), 1);

  plugin_->setSyncProgress(50);
  QCOMPARE(syncSpy.count(), 1);
  QCOMPARE(syncSpy.at(0).at(0).toInt(), 50);

  plugin_->addBackupEntry("2025-06-01T12:00:00", "Completed");
  QCOMPARE(backupSpy.count(), 1);

  plugin_->clearConnections();
  plugin_->clearBackupHistory();
}

QTEST_MAIN(TestCloudManagerPlugin)
#include "cloudmanager_plugin_test.moc"
