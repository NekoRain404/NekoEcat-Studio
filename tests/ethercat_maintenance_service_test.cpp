// EtherCATMaintenanceServiceTest — Tests for EtherCATMaintenanceService
//
// Test coverage:
//   - Task scheduling, cancellation, and listing
//   - Task execution and status queries
//   - Task type variety (Cleanup, Diagnostic, Backup, Calibration)
//   - Maintenance scheduling and execution with history

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATMaintenanceService.h"

class EtherCATMaintenanceServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Schedule a maintenance task
  // Schedule a task and verify type, schedule, and status fields
  void testScheduleTask() {
    EtherCATMaintenanceService svc(nullptr, nullptr);
    auto task = svc.scheduleTask("Cleanup", "daily");
    QCOMPARE(task.taskType, QStringLiteral("Cleanup"));
    QCOMPARE(task.schedule, QStringLiteral("daily"));
    QCOMPARE(task.status, QStringLiteral("Scheduled"));
    QVERIFY(!task.id.isEmpty());
  }

  // Cancel an existing task
  // Cancel a scheduled task successfully
  void testCancelTask() {
    EtherCATMaintenanceService svc(nullptr, nullptr);
    auto task = svc.scheduleTask("Diagnostic", "weekly");
    bool cancelled = svc.cancelTask(task.id);
    QVERIFY(cancelled);
  }

  // Cancel nonexistent task returns false
  // Cancel a nonexistent task returns false
  void testCancelNonexistent() {
    EtherCATMaintenanceService svc(nullptr, nullptr);
    bool cancelled = svc.cancelTask("nonexistent");
    QVERIFY(!cancelled);
  }

  // List all scheduled tasks
  // List multiple scheduled tasks in order
  void testListTasks() {
    EtherCATMaintenanceService svc(nullptr, nullptr);
    svc.scheduleTask("Cleanup", "daily");
    svc.scheduleTask("Backup", "weekly");
    auto list = svc.listTasks();
    QCOMPARE(list.size(), 2);
    QCOMPARE(list[0].taskType, QStringLiteral("Cleanup"));
    QCOMPARE(list[1].taskType, QStringLiteral("Backup"));
  }

  // Execute a scheduled task
  // Run a task and verify completion status
  void testRunTask() {
    EtherCATMaintenanceService svc(nullptr, nullptr);
    auto task = svc.scheduleTask("Calibration", "monthly");
    auto result = svc.runTask(task.id);
    QCOMPARE(result.status, QStringLiteral("Completed"));
    QVERIFY(!result.result.isEmpty());
  }

  // Get status of existing task
  // Query status of an existing task
  void testGetTaskStatus() {
    EtherCATMaintenanceService svc(nullptr, nullptr);
    auto task = svc.scheduleTask("Diagnostic", "daily");
    auto status = svc.getTaskStatus(task.id);
    QCOMPARE(status.status, QStringLiteral("Scheduled"));
    QCOMPARE(status.taskType, QStringLiteral("Diagnostic"));
  }

  // Get status of nonexistent task returns NotFound
  // Query status of a nonexistent task returns NotFound
  void testGetStatusNonexistent() {
    EtherCATMaintenanceService svc(nullptr, nullptr);
    auto status = svc.getTaskStatus("nonexistent");
    QCOMPARE(status.status, QStringLiteral("NotFound"));
  }

  // taskCompleted signal fires on execution
  // Verify taskCompleted signal is emitted on run
  void testTaskSignal() {
    EtherCATMaintenanceService svc(nullptr, nullptr);
    auto task = svc.scheduleTask("Cleanup", "daily");
    QSignalSpy spy(&svc, &EtherCATMaintenanceService::taskCompleted);
    svc.runTask(task.id);
    QCOMPARE(spy.count(), 1);
  }

  // All task types can be scheduled
  // Schedule all four task types and verify
  void testTaskTypes() {
    EtherCATMaintenanceService svc(nullptr, nullptr);
    auto t1 = svc.scheduleTask("Cleanup", "daily");
    auto t2 = svc.scheduleTask("Diagnostic", "weekly");
    auto t3 = svc.scheduleTask("Backup", "monthly");
    auto t4 = svc.scheduleTask("Calibration", "yearly");
    QCOMPARE(svc.listTasks().size(), 4);
    QCOMPARE(t1.taskType, QStringLiteral("Cleanup"));
    QCOMPARE(t2.taskType, QStringLiteral("Diagnostic"));
    QCOMPARE(t3.taskType, QStringLiteral("Backup"));
    QCOMPARE(t4.taskType, QStringLiteral("Calibration"));
  }

  // Schedule preventive maintenance with high priority
  // Schedule a maintenance task and verify in schedule list
  void testScheduleMaintenance() {
    EtherCATMaintenanceService svc(nullptr, nullptr);
    ScheduledMaintenanceTask task;
    task.description = "Test maintenance";
    task.type = MaintenanceType::Preventive;
    task.priority = MaintenancePriority::High;
    bool result = svc.scheduleMaintenance(task);
    QVERIFY(result);
    QCOMPARE(svc.maintenanceSchedule().size(), 1);
  }

  // Execute maintenance and verify history
  // Execute maintenance and verify history entry
  void testExecuteMaintenance() {
    EtherCATMaintenanceService svc(nullptr, nullptr);
    ScheduledMaintenanceTask task;
    task.description = "Execute test";
    task.type = MaintenanceType::Corrective;
    svc.scheduleMaintenance(task);
    bool result = svc.executeMaintenance(1);
    QVERIFY(result);
    QCOMPARE(svc.maintenanceHistory().size(), 1);
  }
};

QTEST_MAIN(EtherCATMaintenanceServiceTest)
#include "ethercat_maintenance_service_test.moc"
