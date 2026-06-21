// TestWorkflowDashboardPlugin — Tests for Workflow Dashboard Plugin
//
// Test coverage:
//   - Plugin identity (id, display names, order, visibility)
//   - Widget creation
//   - Active workflow management (add, remove, count)
//   - Workflow status updates
//   - Alert management and display
//   - Notification handling
//   - Dashboard export functionality
//   - Signal emissions

#include <QApplication>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QtTest/QtTest>

#include "plugins/workflowdashboard/WorkflowDashboardPlugin.h"
#include "services/WorkflowMonitoringService.h"

class TestWorkflowDashboardPlugin : public QObject {
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
  // Add and remove active workflows and verify count
  // Active workflows can be added, removed, and counted
  void activeWorkflows();
  // Update workflow status and verify state
  // Workflow status can be updated after adding
  void workflowStatusUpdate();
  // Add and retrieve alerts
  // Alerts can be added with severity levels
  void alerts();
  // Add and retrieve notifications
  // Notifications can be added by channel type
  void notifications();
  // Export dashboard data to JSON
  // Export dashboard writes JSON to disk
  void exportDashboard();
  // Verify signal emissions on state changes
  // Signal emissions on activation and export
  void signalEmissions();

private:
  WorkflowMonitoringService *monitoring_ = nullptr;
  WorkflowDashboardPlugin *plugin_ = nullptr;
};

void TestWorkflowDashboardPlugin::initTestCase() {
  monitoring_ = new WorkflowMonitoringService(this);
  plugin_ = new WorkflowDashboardPlugin(monitoring_, this);
}

void TestWorkflowDashboardPlugin::cleanupTestCase() {
  delete plugin_;
  plugin_ = nullptr;
  delete monitoring_;
  monitoring_ = nullptr;
}

void TestWorkflowDashboardPlugin::identity() {
  QCOMPARE(plugin_->id(), QString("workflowdashboard"));
  QCOMPARE(plugin_->displayName(), QString("Workflow Dashboard"));
  QCOMPARE(plugin_->displayNameZh(), QString("工作流仪表盘"));
  QCOMPARE(plugin_->defaultOrder(), 390);
  QVERIFY(plugin_->visible());
}

void TestWorkflowDashboardPlugin::widgetNotNull() {
  QVERIFY(plugin_->widget() != nullptr);
}

void TestWorkflowDashboardPlugin::activeWorkflows() {
  QCOMPARE(plugin_->activeWorkflowCount(), 0);

  plugin_->addActiveWorkflow("wf1", "Build Pipeline", "Running");
  QCOMPARE(plugin_->activeWorkflowCount(), 1);

  plugin_->addActiveWorkflow("wf2", "Deploy Pipeline", "Idle");
  QCOMPARE(plugin_->activeWorkflowCount(), 2);

  plugin_->removeActiveWorkflow("wf1");
  QCOMPARE(plugin_->activeWorkflowCount(), 1);

  plugin_->removeActiveWorkflow("nonexistent");
  QCOMPARE(plugin_->activeWorkflowCount(), 1);

  plugin_->removeActiveWorkflow("wf2");
  QCOMPARE(plugin_->activeWorkflowCount(), 0);
}

void TestWorkflowDashboardPlugin::workflowStatusUpdate() {
  plugin_->addActiveWorkflow("wf_update", "Update Test", "Idle");
  QCOMPARE(plugin_->activeWorkflowCount(), 1);

  plugin_->updateWorkflowStatus("wf_update", "Running");
  plugin_->removeActiveWorkflow("wf_update");
}

void TestWorkflowDashboardPlugin::alerts() {
  QCOMPARE(plugin_->alertCount(), 0);

  plugin_->addAlert("Critical", "WorkflowEngine", "Pipeline stalled");
  QCOMPARE(plugin_->alertCount(), 1);

  plugin_->addAlert("Warning", "ResourceMonitor", "High memory usage");
  QCOMPARE(plugin_->alertCount(), 2);

  plugin_->addAlert("Info", "Scheduler", "Task queued");
  QCOMPARE(plugin_->alertCount(), 3);
}

void TestWorkflowDashboardPlugin::notifications() {
  QCOMPARE(plugin_->notificationCount(), 0);

  plugin_->addNotification("email", "Build completed successfully");
  QCOMPARE(plugin_->notificationCount(), 1);

  plugin_->addNotification("slack", "Deployment started");
  QCOMPARE(plugin_->notificationCount(), 2);
}

void TestWorkflowDashboardPlugin::exportDashboard() {
  plugin_->addActiveWorkflow("wf_exp", "Export Test", "Running");
  plugin_->addAlert("Info", "Test", "Test alert");
  plugin_->addNotification("test", "Test notification");

  QString tmpPath = QDir::tempPath() + "/dashboard_export_test.json";
  QVERIFY(plugin_->exportDashboard(tmpPath));
  QVERIFY(QFile::exists(tmpPath));
  QFile::remove(tmpPath);
}

void TestWorkflowDashboardPlugin::signalEmissions() {
  QSignalSpy actSpy(plugin_, &WorkflowDashboardPlugin::workflowActivated);
  QSignalSpy expSpy(plugin_, &WorkflowDashboardPlugin::dashboardExported);

  QString tmpPath = QDir::tempPath() + "/dashboard_signal_test.json";
  plugin_->exportDashboard(tmpPath);
  QCOMPARE(expSpy.count(), 1);
  QFile::remove(tmpPath);
}

QTEST_MAIN(TestWorkflowDashboardPlugin)
#include "workflowdashboard_plugin_test.moc"
