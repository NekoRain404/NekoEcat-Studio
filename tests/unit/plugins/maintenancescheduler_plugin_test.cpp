// TestMaintenanceSchedulerPlugin — Tests for MaintenanceSchedulerPlugin
//
// Test coverage:
//   - Plugin identity (id, display names, visibility)
//   - UI widget and table creation (task, schedule, history tables)
//   - Task add/remove/update operations
//   - Schedule entry add/remove
//   - Maintenance record requests fail closed without a backend acknowledgement
//   - Report generation and export
//   - Signal emissions (taskAdded, taskUpdated, taskRemoved)
//   - Source guard against synthetic completion records

#include <QApplication>
#include <QFile>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QTemporaryDir>
#include <QRegularExpression>
#include <QtTest/QtTest>

#include "plugins/maintenancescheduler/MaintenanceSchedulerPlugin.h"

class TestMaintenanceSchedulerPlugin : public QObject {
  Q_OBJECT
private slots:
  void initTestCase();        // Create plugin instance
  void cleanupTestCase();     // Destroy plugin instance
  void identity();            // Verify plugin id, names, visibility
  void widgetNotNull();       // Check widget is created
  void taskTable();           // Check task table structure
  void scheduleTable();       // Check schedule table structure
  void historyTable();        // Check history table structure
  void addAndRemoveTasks();   // Test task CRUD operations
  void updateTask();          // Test task update with signal
  void addScheduleEntry();    // Test adding schedule entry
  void removeScheduleEntry(); // Test removing schedule entries
  void recordMaintenanceRequiresBackendAck(); // Test maintenance records fail closed
  void clearHistory();        // Test clearing maintenance history
  void generateReport();      // Test report generation
  void exportReport();        // Test report export to JSON
  void signalEmissions();     // Verify all plugin signals
  void noSyntheticCompletionInSource();

private:
  MaintenanceSchedulerPlugin *plugin_ = nullptr;
};

void TestMaintenanceSchedulerPlugin::initTestCase() {
  plugin_ = new MaintenanceSchedulerPlugin(this);
}

void TestMaintenanceSchedulerPlugin::cleanupTestCase() {
  delete plugin_;
  plugin_ = nullptr;
}

void TestMaintenanceSchedulerPlugin::identity() {
  QCOMPARE(plugin_->id(), QString("maintenancescheduler"));
  QCOMPARE(plugin_->displayName(), QString("Maintenance Scheduler"));
  QCOMPARE(plugin_->displayNameZh(), QString("维护调度器"));
  QVERIFY(!plugin_->visible());
}

void TestMaintenanceSchedulerPlugin::widgetNotNull() {
  QVERIFY(plugin_->widget() != nullptr);
}

void TestMaintenanceSchedulerPlugin::taskTable() {
  QVERIFY(plugin_->taskTable() != nullptr);
  QCOMPARE(plugin_->taskTable()->columnCount(), 5);
}

void TestMaintenanceSchedulerPlugin::scheduleTable() {
  QVERIFY(plugin_->scheduleTable() != nullptr);
  QCOMPARE(plugin_->scheduleTable()->columnCount(), 2);
}

void TestMaintenanceSchedulerPlugin::historyTable() {
  QVERIFY(plugin_->historyTable() != nullptr);
  QCOMPARE(plugin_->historyTable()->columnCount(), 4);
}

void TestMaintenanceSchedulerPlugin::addAndRemoveTasks() {
  QCOMPARE(plugin_->taskCount(), 0);

  MaintenanceTask t1;
  t1.id = "task_1";
  t1.name = "Lubrication";
  t1.description = "Lubricate bearings";
  t1.schedule = "Monthly";
  t1.priority = "High";
  t1.status = "Pending";
  plugin_->addTask(t1);
  QCOMPARE(plugin_->taskCount(), 1);

  MaintenanceTask t2;
  t2.id = "task_2";
  t2.name = "Calibration";
  t2.description = "Calibrate sensors";
  t2.schedule = "Quarterly";
  t2.priority = "Medium";
  t2.status = "Pending";
  plugin_->addTask(t2);
  QCOMPARE(plugin_->taskCount(), 2);

  QCOMPARE(plugin_->taskTable()->rowCount(), 2);

  plugin_->removeTask(0);
  QCOMPARE(plugin_->taskCount(), 1);

  plugin_->removeTask(0);
  QCOMPARE(plugin_->taskCount(), 0);
}

void TestMaintenanceSchedulerPlugin::updateTask() {
  MaintenanceTask t;
  t.id = "task_1";
  t.name = "Original Task";
  t.description = "Original description";
  t.schedule = "Weekly";
  t.priority = "Low";
  t.status = "Pending";
  plugin_->addTask(t);

  QSignalSpy spy(plugin_, &MaintenanceSchedulerPlugin::taskUpdated);
  t.name = "Updated Task";
  t.status = "In Progress";
  plugin_->updateTask(0, t);
  QCOMPARE(spy.count(), 1);
  QCOMPARE(plugin_->taskTable()->item(0, 0)->text(), QString("Updated Task"));
  QCOMPARE(plugin_->taskTable()->item(0, 4)->text(), QString("In Progress"));

  plugin_->removeTask(0);
}

void TestMaintenanceSchedulerPlugin::addScheduleEntry() {
  QCOMPARE(plugin_->scheduleCount(), 0);
  plugin_->addScheduleEntry("Lubrication", "2025-06-01 09:00");
  QCOMPARE(plugin_->scheduleCount(), 1);
  QCOMPARE(plugin_->scheduleTable()->rowCount(), 1);
  QCOMPARE(plugin_->scheduleTable()->item(0, 0)->text(), QString("Lubrication"));
  QCOMPARE(plugin_->scheduleTable()->item(0, 1)->text(), QString("2025-06-01 09:00"));
  plugin_->removeScheduleEntry(0);
}

void TestMaintenanceSchedulerPlugin::removeScheduleEntry() {
  plugin_->addScheduleEntry("Task1", "2025-06-01");
  plugin_->addScheduleEntry("Task2", "2025-06-02");
  QCOMPARE(plugin_->scheduleCount(), 2);
  plugin_->removeScheduleEntry(0);
  QCOMPARE(plugin_->scheduleCount(), 1);
  plugin_->removeScheduleEntry(0);
  QCOMPARE(plugin_->scheduleCount(), 0);
}

void TestMaintenanceSchedulerPlugin::recordMaintenanceRequiresBackendAck() {
  QCOMPARE(plugin_->historyCount(), 0);

  QSignalSpy recordedSpy(plugin_, &MaintenanceSchedulerPlugin::maintenanceRecorded);
  MaintenanceRecord rec;
  rec.id = "rec_1";
  rec.taskName = "Lubrication";
  rec.status = "Completed";
  rec.timestamp = "2025-01-01T00:00:00";
  rec.notes = "All bearings lubricated";
  QVERIFY(!plugin_->recordMaintenance(rec));
  QCOMPARE(plugin_->historyCount(), 0);
  QCOMPARE(plugin_->historyTable()->rowCount(), 0);
  QCOMPARE(recordedSpy.count(), 0);
  QVERIFY(plugin_->statusLabel()->text().contains("backend", Qt::CaseInsensitive));
}

void TestMaintenanceSchedulerPlugin::clearHistory() {
  plugin_->clearHistory();
  QCOMPARE(plugin_->historyCount(), 0);

  MaintenanceRecord rec;
  rec.id = "rec_1";
  rec.taskName = "Test";
  rec.status = "Completed";
  rec.timestamp = "2025-01-01T00:00:00";
  rec.notes = "Test note";
  QVERIFY(!plugin_->recordMaintenance(rec));
  QCOMPARE(plugin_->historyCount(), 0);
  plugin_->clearHistory();
  QCOMPARE(plugin_->historyCount(), 0);
  QCOMPARE(plugin_->historyTable()->rowCount(), 0);
}

void TestMaintenanceSchedulerPlugin::generateReport() {
  MaintenanceTask t;
  t.id = "task_1";
  t.name = "Report Task";
  t.description = "For report";
  t.schedule = "Weekly";
  t.priority = "High";
  t.status = "Active";
  plugin_->addTask(t);

  plugin_->generateReport();
  QVERIFY(!plugin_->reportPanel()->toPlainText().isEmpty());
  QVERIFY(plugin_->reportPanel()->toPlainText().contains("Report Task"));

  plugin_->removeTask(0);
}

void TestMaintenanceSchedulerPlugin::exportReport() {
  MaintenanceTask t;
  t.id = "task_1";
  t.name = "Export Task";
  t.description = "For export";
  t.schedule = "Monthly";
  t.priority = "Medium";
  t.status = "Active";
  plugin_->addTask(t);

  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  const QString tmpPath = dir.filePath("maintenance_report.json");
  QVERIFY(plugin_->exportReport(tmpPath));
  QVERIFY(QFile::exists(tmpPath));

  QTest::failOnWarning(QRegularExpression(
      QStringLiteral("QFSFileEngine::open: No file name specified")));
  QVERIFY(!plugin_->exportReport(QString()));
  QVERIFY(!plugin_->exportReport(dir.path()));

  plugin_->removeTask(0);
  plugin_->clearHistory();
}

void TestMaintenanceSchedulerPlugin::signalEmissions() {
  QSignalSpy taskAddedSpy(plugin_, &MaintenanceSchedulerPlugin::taskAdded);
  QSignalSpy taskUpdatedSpy(plugin_, &MaintenanceSchedulerPlugin::taskUpdated);
  QSignalSpy taskRemovedSpy(plugin_, &MaintenanceSchedulerPlugin::taskRemoved);

  MaintenanceTask t;
  t.id = "task_1";
  t.name = "Signal Task";
  t.description = "Signal test";
  t.schedule = "Daily";
  t.priority = "Low";
  t.status = "Pending";
  plugin_->addTask(t);
  QCOMPARE(taskAddedSpy.count(), 1);

  t.name = "Updated Signal Task";
  plugin_->updateTask(0, t);
  QCOMPARE(taskUpdatedSpy.count(), 1);

  plugin_->removeTask(0);
  QCOMPARE(taskRemovedSpy.count(), 1);

  plugin_->clearHistory();
}

void TestMaintenanceSchedulerPlugin::noSyntheticCompletionInSource() {
  QFile source(QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/plugins/maintenancescheduler/MaintenanceSchedulerPlugin.cpp"));
  QVERIFY2(source.open(QIODevice::ReadOnly | QIODevice::Text),
           qPrintable(QStringLiteral("Unable to open %1").arg(source.fileName())));
  const QString text = QString::fromUtf8(source.readAll());
  QVERIFY2(!text.contains(QStringLiteral("rec.status = \"Completed\"")),
           "Maintenance scheduler must not synthesize a Completed record from the UI button.");
  QVERIFY2(!text.contains(QStringLiteral("Completed maintenances: %1")),
           "Maintenance reports must not count unacknowledged local history as completed maintenance.");
}

QTEST_MAIN(TestMaintenanceSchedulerPlugin)
#include "maintenancescheduler_plugin_test.moc"
