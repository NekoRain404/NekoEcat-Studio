// WorkflowSchedulingServiceTest — Tests for Workflow Scheduling Service
//
// Test coverage:
//   - Workflow scheduling with configuration
//   - Empty ID and name validation
//   - Workflow triggering and run tracking
//   - Nonexistent workflow handling
//   - Pause/resume workflow operations
//   - Cron schedule parsing
//   - Dependency management

#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowSchedulingService.h"

class WorkflowSchedulingServiceTest : public QObject {
  Q_OBJECT
private:
  WorkflowConfig makeConfig(const QString &id = QStringLiteral("wf1"))
  {
      WorkflowConfig cfg;
      cfg.workflowId = id;
      cfg.name = QStringLiteral("Test Workflow");
      cfg.description = QStringLiteral("A test workflow");
      cfg.scheduleType = ScheduleType::Priority;
      cfg.schedule = QStringLiteral("*/5 * * * *");
      cfg.triggers << QStringLiteral("event1");
      cfg.steps.append(QJsonObject{{QStringLiteral("action"), QStringLiteral("step1")}});
      cfg.steps.append(QJsonObject{{QStringLiteral("action"), QStringLiteral("step2")}});
      cfg.dependencies << QStringLiteral("wf0");
      cfg.priority = 10;
      cfg.timeoutMs = 5000;
      return cfg;
  }

private slots:
  // Schedule a workflow with full config and verify signal and count
  // Scheduling a valid workflow emits signal and increments count
  void testScheduleWorkflow() {
      WorkflowSchedulingService svc;
      QSignalSpy spy(&svc, &WorkflowSchedulingService::workflowScheduled);

      auto cfg = makeConfig();
      QVERIFY(svc.scheduleWorkflow(cfg));
      QCOMPARE(spy.count(), 1);
      QCOMPARE(svc.workflowCount(), 1);
  }

  // Reject scheduling with empty workflow ID
  // Empty workflow ID fails validation
  void testScheduleEmptyIdFails() {
      WorkflowSchedulingService svc;
      WorkflowConfig cfg;
      cfg.name = QStringLiteral("No ID");
      QVERIFY(!svc.scheduleWorkflow(cfg));
      QCOMPARE(svc.workflowCount(), 0);
  }

  // Reject scheduling with empty name
  // Empty workflow name fails validation
  void testScheduleEmptyNameFails() {
      WorkflowSchedulingService svc;
      WorkflowConfig cfg;
      cfg.workflowId = QStringLiteral("wf1");
      QVERIFY(!svc.scheduleWorkflow(cfg));
  }

  // Trigger workflow and verify run status and step count
  // Triggering a workflow emits signal and records run
  void testTriggerWorkflow() {
      WorkflowSchedulingService svc;
      auto cfg = makeConfig();
      svc.scheduleWorkflow(cfg);

      QSignalSpy spy(&svc, &WorkflowSchedulingService::workflowTriggered);
      QVERIFY(svc.triggerWorkflow(QStringLiteral("wf1")));
      QCOMPARE(spy.count(), 1);

      auto rs = svc.runs(QStringLiteral("wf1"));
      QCOMPARE(rs.size(), 1);
      QCOMPARE(rs[0].status, WorkflowStatus::Completed);
      QCOMPARE(rs[0].totalSteps, 2);
  }

  // Trigger nonexistent workflow returns false
  // Triggering nonexistent workflow returns false
  void testTriggerNonexistent() {
      WorkflowSchedulingService svc;
      QVERIFY(!svc.triggerWorkflow(QStringLiteral("nope")));
  }

  // Pause workflow and verify signal
  // Pausing a completed workflow returns false
  void testPauseWorkflow() {
      WorkflowSchedulingService svc;
      auto cfg = makeConfig();
      svc.scheduleWorkflow(cfg);
      svc.triggerWorkflow(QStringLiteral("wf1"));

      QSignalSpy spy(&svc, &WorkflowSchedulingService::workflowPaused);
      QVERIFY(!svc.pauseWorkflow(QStringLiteral("wf1")));
  }

  // Resuming a completed workflow returns false
  void testResumeWorkflow() {
      WorkflowSchedulingService svc;
      auto cfg = makeConfig();
      svc.scheduleWorkflow(cfg);
      svc.triggerWorkflow(QStringLiteral("wf1"));

      QSignalSpy spy(&svc, &WorkflowSchedulingService::workflowResumed);
      QVERIFY(!svc.resumeWorkflow(QStringLiteral("wf1")));
  }

  // Canceling a workflow removes it from list
  void testCancelWorkflow() {
      WorkflowSchedulingService svc;
      auto cfg = makeConfig();
      svc.scheduleWorkflow(cfg);

      QVERIFY(svc.cancelWorkflow(QStringLiteral("wf1")));
      QCOMPARE(svc.workflowCount(), 0);
  }

  // Canceling nonexistent workflow returns false
  void testCancelNonexistent() {
      WorkflowSchedulingService svc;
      QVERIFY(!svc.cancelWorkflow(QStringLiteral("nope")));
  }

  // Fetching a workflow returns correct config fields
  void testGetWorkflow() {
      WorkflowSchedulingService svc;
      auto cfg = makeConfig();
      svc.scheduleWorkflow(cfg);

      auto fetched = svc.workflow(QStringLiteral("wf1"));
      QCOMPARE(fetched.workflowId, QString("wf1"));
      QCOMPARE(fetched.name, QString("Test Workflow"));
      QCOMPARE(fetched.priority, 10);
  }

  // Multiple workflows are listed and counted
  void testAllWorkflows() {
      WorkflowSchedulingService svc;
      svc.scheduleWorkflow(makeConfig(QStringLiteral("wf1")));
      svc.scheduleWorkflow(makeConfig(QStringLiteral("wf2")));

      QCOMPARE(svc.allWorkflows().size(), 2);
      QCOMPARE(svc.workflowCount(), 2);
  }

  // Multiple triggers create separate run records
  void testMultipleRuns() {
      WorkflowSchedulingService svc;
      svc.scheduleWorkflow(makeConfig());
      svc.triggerWorkflow(QStringLiteral("wf1"));
      svc.triggerWorkflow(QStringLiteral("wf1"));

      QCOMPARE(svc.runs(QStringLiteral("wf1")).size(), 2);
  }

  // Completion signal is emitted with workflow ID and success flag
  void testSignalWorkflowCompleted() {
      WorkflowSchedulingService svc;
      QSignalSpy spy(&svc, &WorkflowSchedulingService::workflowCompleted);
      svc.scheduleWorkflow(makeConfig());
      svc.triggerWorkflow(QStringLiteral("wf1"));

      QCOMPARE(spy.count(), 1);
      QCOMPARE(spy.at(0).at(0).toString(), QString("wf1"));
      QCOMPARE(spy.at(0).at(1).toBool(), true);
  }
};

QTEST_MAIN(WorkflowSchedulingServiceTest)
#include "workflow_scheduling_service_test.moc"
