// TaskManagementServiceTest — Tests for Task Management Service
//
// Test coverage:
//   - Task creation with full configuration
//   - Task assignment to users
//   - Task tracking and status monitoring
//   - Status updates
//   - Report generation
//   - Task dependencies
//   - Tag management (add/remove)
//   - Nonexistent task handling
//   - Filtering by assignee and status
#include <QTest>
#include <QSignalSpy>
#include "services/TaskManagementService.h"

class TaskManagementServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Test creating a task with full config emits signal
  void testCreateTask() {
    TaskManagementService svc;
    QSignalSpy spy(&svc, &TaskManagementService::taskCreated);

    TaskConfig cfg;
    cfg.title = "Test Task";
    cfg.description = "A test task";
    cfg.priority = TaskPriority::High;
    cfg.assignee = "user1";
    cfg.tags = {"ethercat", "urgent"};

    Task t = svc.createTask(cfg);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(t.title, QString("Test Task"));
    QCOMPARE(t.priority, TaskPriority::High);
    QCOMPARE(t.status, TaskStatus::Open);
    QCOMPARE(t.assignee, QString("user1"));
    QCOMPARE(t.tags.size(), 2);
  }

  void testRejectEmptyTaskTitle() {
    TaskManagementService svc;
    QSignalSpy spy(&svc, &TaskManagementService::taskCreated);

    TaskConfig cfg;
    cfg.title = "   ";
    Task t = svc.createTask(cfg);

    QCOMPARE(t.id, 0);
    QCOMPARE(svc.taskCount(), 0);
    QCOMPARE(spy.count(), 0);
  }

  // Test assigning a task updates assignee and emits signal
  void testAssignTask() {
    TaskManagementService svc;
    TaskConfig cfg;
    cfg.title = "Assign Test";
    Task t = svc.createTask(cfg);

    QSignalSpy spy(&svc, &TaskManagementService::taskUpdated);
    QVERIFY(svc.assignTask(t.id, "user2"));
    QCOMPARE(spy.count(), 1);

    TaskStatusInfo info = svc.trackTask(t.id);
    QCOMPARE(info.assignee, QString("user2"));
  }

  // Test tracking returns correct task ID and status
  void testTrackTask() {
    TaskManagementService svc;
    TaskConfig cfg;
    cfg.title = "Track Test";
    Task t = svc.createTask(cfg);

    TaskStatusInfo info = svc.trackTask(t.id);
    QCOMPARE(info.taskId, t.id);
    QCOMPARE(info.status, TaskStatus::Open);
    QVERIFY(!info.overdue);
  }

  // Test updating task status
  void testUpdateStatus() {
    TaskManagementService svc;
    TaskConfig cfg;
    cfg.title = "Status Test";
    Task t = svc.createTask(cfg);

    QVERIFY(svc.updateTaskStatus(t.id, TaskStatus::InProgress));
    TaskStatusInfo info = svc.trackTask(t.id);
    QCOMPARE(info.status, TaskStatus::InProgress);
  }

  // Test generating task report with correct counts
  void testGenerateReport() {
    TaskManagementService svc;
    TaskConfig cfg;
    cfg.title = "T1";
    svc.createTask(cfg);
    cfg.title = "T2";
    cfg.priority = TaskPriority::Critical;
    svc.createTask(cfg);

    TaskReport report = svc.generateTaskReport();
    QCOMPARE(report.totalTasks, 2);
    QCOMPARE(report.openTasks, 2);
    QVERIFY(report.generatedAt.isValid());
  }

  // Test task dependencies are stored correctly
  void testDependencies() {
    TaskManagementService svc;
    TaskConfig cfg;
    cfg.title = "Task A";
    Task a = svc.createTask(cfg);
    cfg.title = "Task B";
    cfg.dependencies = {a.id};
    Task b = svc.createTask(cfg);

    Task fetched = svc.task(b.id);
    QCOMPARE(fetched.dependencies.size(), 1);
    QCOMPARE(fetched.dependencies[0], a.id);
  }

  void testRejectInvalidDependencies() {
    TaskManagementService svc;
    TaskConfig cfg;
    cfg.title = "Task A";
    Task a = svc.createTask(cfg);

    QVERIFY(!svc.addDependency(a.id, a.id));
    QVERIFY(!svc.addDependency(a.id, 999));

    Task fetched = svc.task(a.id);
    QCOMPARE(fetched.dependencies.size(), 0);
  }

  // Test adding and removing tags
  void testTags() {
    TaskManagementService svc;
    TaskConfig cfg;
    cfg.title = "Tag Test";
    Task t = svc.createTask(cfg);

    QVERIFY(svc.addTag(t.id, "tag1"));
    QVERIFY(svc.addTag(t.id, "tag2"));
    Task fetched = svc.task(t.id);
    QCOMPARE(fetched.tags.size(), 2);

    QVERIFY(svc.removeTag(t.id, "tag1"));
    fetched = svc.task(t.id);
    QCOMPARE(fetched.tags.size(), 1);
  }

  // Test nonexistent task returns empty info
  void testNonexistentTask() {
    TaskManagementService svc;
    TaskStatusInfo info = svc.trackTask(999);
    QCOMPARE(info.taskId, 0);
    QVERIFY(!svc.assignTask(999, "user"));
    QCOMPARE(svc.taskCount(), 0);
  }

  // Test filtering tasks by assignee
  void testFilterByAssignee() {
    TaskManagementService svc;
    TaskConfig cfg;
    cfg.title = "T1";
    cfg.assignee = "alice";
    svc.createTask(cfg);
    cfg.title = "T2";
    cfg.assignee = "bob";
    svc.createTask(cfg);
    cfg.title = "T3";
    cfg.assignee = "alice";
    svc.createTask(cfg);

    QCOMPARE(svc.tasksByAssignee("alice").size(), 2);
    QCOMPARE(svc.tasksByAssignee("bob").size(), 1);
  }

  // Test filtering tasks by status
  void testFilterByStatus() {
    TaskManagementService svc;
    TaskConfig cfg;
    cfg.title = "T1";
    Task t = svc.createTask(cfg);
    cfg.title = "T2";
    svc.createTask(cfg);

    svc.updateTaskStatus(t.id, TaskStatus::Completed);
    QCOMPARE(svc.tasksByStatus(TaskStatus::Completed).size(), 1);
    QCOMPARE(svc.tasksByStatus(TaskStatus::Open).size(), 1);
  }
};

QTEST_MAIN(TaskManagementServiceTest)
#include "task_management_service_test.moc"
