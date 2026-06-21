// TestEdgeComputingPlugin — Tests for EdgeComputingPlugin
//
// Test coverage:
//   - Plugin identity and ordering
//   - Device table, processing jobs, analytics view, storage table
//   - Add/remove/clear devices
//   - Add/remove/clear processing jobs
//   - Analytics text get/set
//   - Storage entries management
//   - Export edge report
//   - Signal emissions

#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QTreeWidget>
#include <QtTest/QtTest>

#include "plugins/edgecomputing/EdgeComputingPlugin.h"

class TestEdgeComputingPlugin : public QObject {
  Q_OBJECT
private slots:
  void initTestCase();
  void cleanupTestCase();
  // Verify plugin id, display names, order, and visibility
  void identity();
  // Verify main widget is created
  void widgetNotNull();
  // Verify device table widget exists
  void deviceTable();
  // Verify processing jobs widget exists
  void processingJobs();
  // Verify analytics view widget exists
  void analyticsView();
  // Verify storage table widget exists
  void storageTable();
  // Verify adding and removing devices updates count correctly
  void addAndRemoveDevices();
  // Verify clearDevices resets count to zero
  void clearDevices();
  // Verify adding and removing processing jobs updates count correctly
  void addAndRemoveProcessingJobs();
  // Verify clearProcessingJobs resets count to zero
  void clearProcessingJobs();
  // Verify analytics text get/set
  void analyticsText();
  // Verify storage entries add/clear operations
  void storageEntries();
  // Verify export creates edge report file
  void exportEdgeReport();
  // Verify all expected signals are emitted on operations
  void signalEmissions();

private:
  EdgeComputingPlugin *plugin_ = nullptr;
};

void TestEdgeComputingPlugin::initTestCase() {
  plugin_ = new EdgeComputingPlugin(this);
}

void TestEdgeComputingPlugin::cleanupTestCase() {
  delete plugin_;
  plugin_ = nullptr;
}

// Verify plugin id, display names, order, and visibility
void TestEdgeComputingPlugin::identity() {
  QCOMPARE(plugin_->id(), QString("edgecomputing"));
  QCOMPARE(plugin_->displayName(), QString("Edge Computing"));
  QCOMPARE(plugin_->displayNameZh(), QString("边缘计算"));
  QCOMPARE(plugin_->defaultOrder(), 320);
  QVERIFY(plugin_->visible());
}

// Verify main widget is created
void TestEdgeComputingPlugin::widgetNotNull() {
  QVERIFY(plugin_->widget() != nullptr);
}

// Verify device table widget is created
void TestEdgeComputingPlugin::deviceTable() {
  QVERIFY(plugin_->deviceTable() != nullptr);
}

// Verify processing jobs widget is created
void TestEdgeComputingPlugin::processingJobs() {
  QVERIFY(plugin_->processingJobs() != nullptr);
}

// Verify analytics view widget is created
void TestEdgeComputingPlugin::analyticsView() {
  QVERIFY(plugin_->analyticsView() != nullptr);
}

// Verify storage table widget is created
void TestEdgeComputingPlugin::storageTable() {
  QVERIFY(plugin_->storageTable() != nullptr);
}

// Verify add, remove, and clear device operations
void TestEdgeComputingPlugin::addAndRemoveDevices() {
  plugin_->clearDevices();
  QCOMPARE(plugin_->deviceCount(), 0);

  plugin_->addDevice("Sensor-1", "Temperature", "Online");
  QCOMPARE(plugin_->deviceCount(), 1);

  plugin_->addDevice("Sensor-2", "Pressure", "Online");
  QCOMPARE(plugin_->deviceCount(), 2);

  plugin_->removeDevice("Sensor-1");
  QCOMPARE(plugin_->deviceCount(), 1);

  plugin_->removeDevice("NonExistent");
  QCOMPARE(plugin_->deviceCount(), 1);

  plugin_->clearDevices();
}

// Verify clearDevices resets count to zero
void TestEdgeComputingPlugin::clearDevices() {
  plugin_->addDevice("A", "Type1", "OK");
  plugin_->addDevice("B", "Type2", "OK");
  QCOMPARE(plugin_->deviceCount(), 2);

  plugin_->clearDevices();
  QCOMPARE(plugin_->deviceCount(), 0);
}

// Verify add, remove, and clear processing job operations
void TestEdgeComputingPlugin::addAndRemoveProcessingJobs() {
  plugin_->clearProcessingJobs();
  QCOMPARE(plugin_->processingJobCount(), 0);

  plugin_->addProcessingJob("job-1", "Aggregate sensor data");
  QCOMPARE(plugin_->processingJobCount(), 1);

  plugin_->addProcessingJob("job-2", "Filter noise");
  QCOMPARE(plugin_->processingJobCount(), 2);

  plugin_->removeProcessingJob("job-1");
  QCOMPARE(plugin_->processingJobCount(), 1);

  plugin_->removeProcessingJob("NonExistent");
  QCOMPARE(plugin_->processingJobCount(), 1);

  plugin_->clearProcessingJobs();
}

// Verify clearProcessingJobs resets count to zero
void TestEdgeComputingPlugin::clearProcessingJobs() {
  plugin_->addProcessingJob("A", "job1");
  plugin_->addProcessingJob("B", "job2");
  QCOMPARE(plugin_->processingJobCount(), 2);

  plugin_->clearProcessingJobs();
  QCOMPARE(plugin_->processingJobCount(), 0);
}

// Verify analytics text get/set
void TestEdgeComputingPlugin::analyticsText() {
  plugin_->setAnalyticsText("Trend: increasing, slope: 0.5");
  QCOMPARE(plugin_->analyticsText(), QString("Trend: increasing, slope: 0.5"));

  plugin_->setAnalyticsText("");
  QCOMPARE(plugin_->analyticsText(), QString(""));
}

// Verify storage entry add and clear operations
void TestEdgeComputingPlugin::storageEntries() {
  plugin_->clearStorageEntries();
  QCOMPARE(plugin_->storageEntryCount(), 0);

  plugin_->addStorageEntry("log.dat", "10MB", "75%");
  QCOMPARE(plugin_->storageEntryCount(), 1);

  plugin_->addStorageEntry("metrics.db", "50MB", "40%");
  QCOMPARE(plugin_->storageEntryCount(), 2);

  plugin_->clearStorageEntries();
  QCOMPARE(plugin_->storageEntryCount(), 0);
}

// Verify export edge report to JSON
void TestEdgeComputingPlugin::exportEdgeReport() {
  QString tmpPath = QDir::tempPath() + "/edge_computing_test_export.json";
  QVERIFY(plugin_->exportEdgeReport(tmpPath, "JSON"));
  QFile::remove(tmpPath);
}

// Verify device, job, and analytics signals are emitted
void TestEdgeComputingPlugin::signalEmissions() {
  QSignalSpy deviceAddSpy(plugin_, &EdgeComputingPlugin::deviceAdded);
  QSignalSpy deviceRemoveSpy(plugin_, &EdgeComputingPlugin::deviceRemoved);
  QSignalSpy jobSpy(plugin_, &EdgeComputingPlugin::processingJobAdded);
  QSignalSpy analyticsSpy(plugin_, &EdgeComputingPlugin::analyticsUpdated);

  plugin_->addDevice("SignalDevice", "Sensor", "Online");
  QCOMPARE(deviceAddSpy.count(), 1);
  QCOMPARE(deviceAddSpy.at(0).at(0).toString(), QString("SignalDevice"));

  plugin_->removeDevice("SignalDevice");
  QCOMPARE(deviceRemoveSpy.count(), 1);

  plugin_->addProcessingJob("signal-job", "test job");
  QCOMPARE(jobSpy.count(), 1);
  QCOMPARE(jobSpy.at(0).at(0).toString(), QString("signal-job"));

  plugin_->setAnalyticsText("test analytics");
  QCOMPARE(analyticsSpy.count(), 1);

  plugin_->clearDevices();
  plugin_->clearProcessingJobs();
}

QTEST_MAIN(TestEdgeComputingPlugin)
#include "edgecomputing_plugin_test.moc"
