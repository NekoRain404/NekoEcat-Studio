// WorkflowAutomationServiceTest — Tests for Workflow Automation Service
//
// Test coverage:
//   - Task automation with config and signals
//   - Test automation with fail-fast
//   - Deploy automation with dry-run
//   - Monitor automation
//   - Empty config rejection for all types
//   - Cancel running and nonexistent tasks
//   - All statuses aggregation
//   - Automation progress signal
#include <QTest>
#include <QSignalSpy>
#include <QFile>
#include "services/WorkflowAutomationService.h"

class WorkflowAutomationServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Automating a task emits started but waits for an execution backend
  // Automate a task with schedule and triggers
  void testAutomateTask() {
      WorkflowAutomationService svc;
      QSignalSpy startedSpy(&svc, &WorkflowAutomationService::automationStarted);
      QSignalSpy completedSpy(&svc, &WorkflowAutomationService::automationCompleted);

      AutoTaskConfig cfg;
      cfg.task = QStringLiteral("build");
      cfg.schedule = QStringLiteral("0 * * * *");
      cfg.triggers << QStringLiteral("push") << QStringLiteral("manual");
      cfg.priority = 5;

      QVERIFY(svc.automateTask(cfg));
      QCOMPARE(startedSpy.count(), 1);
      QCOMPARE(completedSpy.count(), 0);

      auto s = svc.status(QStringLiteral("task"));
      QCOMPARE(s.type, QString("task"));
      QCOMPARE(s.result, AutomationResult::Running);
      QCOMPARE(s.progress, 0.0);
  }

  // Test automation with environment config and failFast
  // Automate tests with fail-fast mode
  void testAutomateTest() {
      WorkflowAutomationService svc;
      QSignalSpy spy(&svc, &WorkflowAutomationService::automationCompleted);

      TestConfig cfg;
      cfg.tests << QStringLiteral("unit") << QStringLiteral("integration");
      cfg.environment = QStringLiteral("staging");
      cfg.failFast = true;

      QVERIFY(svc.automateTest(cfg));
      QCOMPARE(spy.count(), 0);

      auto s = svc.status(QStringLiteral("test"));
      QCOMPARE(s.result, AutomationResult::Running);
  }

  // Deploy automation with rollback steps and dry run
  // Automate deploy with dry-run and rollback steps
  void testAutomateDeploy() {
      WorkflowAutomationService svc;
      QSignalSpy spy(&svc, &WorkflowAutomationService::automationStarted);

      DeployConfig cfg;
      cfg.target = QStringLiteral("production");
      cfg.version = QStringLiteral("1.2.3");
      cfg.rollbackSteps << QStringLiteral("stop") << QStringLiteral("restore");
      cfg.dryRun = true;

      QVERIFY(svc.automateDeploy(cfg));
      QCOMPARE(spy.count(), 1);
      QCOMPARE(spy.at(0).at(0).toString(), QString("deploy"));
  }

  // Monitor automation with metrics and alert config
  // Automate monitoring with metrics and alerts
  void testAutomateMonitor() {
      WorkflowAutomationService svc;

      MonitorConfig cfg;
      cfg.metrics << QStringLiteral("cpu") << QStringLiteral("memory");
      cfg.alerts << QStringLiteral("high_cpu");
      cfg.notifications << QStringLiteral("email");
      cfg.intervalMs = 1000;

      QVERIFY(svc.automateMonitor(cfg));
      auto s = svc.status(QStringLiteral("monitor"));
      QCOMPARE(s.result, AutomationResult::Running);
  }

  // Empty task config returns false
  // Empty task config returns false
  void testEmptyTaskReturnsFalse() {
      WorkflowAutomationService svc;
      AutoTaskConfig cfg;
      QVERIFY(!svc.automateTask(cfg));
  }

  // Empty test config returns false
  // Empty test config returns false
  void testEmptyTestReturnsFalse() {
      WorkflowAutomationService svc;
      TestConfig cfg;
      QVERIFY(!svc.automateTest(cfg));
  }

  // Empty deploy config returns false
  // Empty deploy config returns false
  void testEmptyDeployReturnsFalse() {
      WorkflowAutomationService svc;
      DeployConfig cfg;
      QVERIFY(!svc.automateDeploy(cfg));
  }

  // Empty monitor config returns false
  // Empty monitor config returns false
  void testEmptyMonitorReturnsFalse() {
      WorkflowAutomationService svc;
      MonitorConfig cfg;
      QVERIFY(!svc.automateMonitor(cfg));
  }

  // Cancel on a backend-pending task marks it cancelled
  // Cancel running task emits a failed completion
  void testCancel() {
      WorkflowAutomationService svc;
      AutoTaskConfig cfg;
      cfg.task = QStringLiteral("long_task");
      svc.automateTask(cfg);

      auto s = svc.status(QStringLiteral("task"));
      QCOMPARE(s.result, AutomationResult::Running);
      QSignalSpy completedSpy(&svc, &WorkflowAutomationService::automationCompleted);
      QVERIFY(svc.cancel(QStringLiteral("task")));
      QCOMPARE(completedSpy.count(), 1);
      QCOMPARE(completedSpy.at(0).at(1).toBool(), false);
      QCOMPARE(svc.status(QStringLiteral("task")).result, AutomationResult::Cancelled);
  }

  // Cancel on nonexistent task returns false
  // Cancel nonexistent task returns false
  void testCancelNonexistent() {
      WorkflowAutomationService svc;
      QVERIFY(!svc.cancel(QStringLiteral("nope")));
  }

  // allStatuses returns all registered automations
  // Aggregate statuses from multiple automations
  void testAllStatuses() {
      WorkflowAutomationService svc;

      AutoTaskConfig tcfg;
      tcfg.task = QStringLiteral("t1");
      svc.automateTask(tcfg);

      TestConfig testCfg;
      testCfg.tests << QStringLiteral("t1");
      svc.automateTest(testCfg);

      QCOMPARE(svc.allStatuses().size(), 2);
  }

  // Progress signal must not report 100% without an execution backend
  void testSignalProgressNotSynthesized() {
      WorkflowAutomationService svc;
      QSignalSpy spy(&svc, &WorkflowAutomationService::automationProgress);

      AutoTaskConfig cfg;
      cfg.task = QStringLiteral("progress_task");
      svc.automateTask(cfg);

      QCOMPARE(spy.count(), 0);
  }

  void testSourceDoesNotKeepSyntheticSuccessPath() {
      QFile file(QStringLiteral(SOURCE_ROOT
                                "/apps/ecat-studio/services/WorkflowAutomationService.cpp"));
      QVERIFY2(file.open(QIODevice::ReadOnly | QIODevice::Text),
               qPrintable(file.errorString()));
      const QString source = QString::fromUtf8(file.readAll());

      QVERIFY2(!source.contains(QStringLiteral("s.result = AutomationResult::Success")),
               "Workflow automation must not report success without an execution backend");
      QVERIFY2(!source.contains(QStringLiteral("s.progress = 100.0")),
               "Workflow automation must not report 100% progress without an execution backend");
      QVERIFY2(!source.contains(QStringLiteral("emit automationCompleted(type, true)")),
               "Workflow automation must not emit synthetic successful completion");
  }
};

QTEST_MAIN(WorkflowAutomationServiceTest)
#include "workflow_automation_service_test.moc"
