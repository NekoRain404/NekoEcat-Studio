#include <QTest>
#include <QSignalSpy>
#include <QElapsedTimer>
#include "services/WorkflowAutomationService.h"

class WorkflowAutomationPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testTaskAutomationThroughput() {
    WorkflowAutomationService svc;
    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      AutoTaskConfig cfg;
      cfg.task = QStringLiteral("task_%1").arg(i);
      cfg.schedule = QStringLiteral("*/5 * * * *");
      cfg.priority = i % 10;
      svc.automateTask(cfg);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    auto s = svc.status(QStringLiteral("task"));
    QCOMPARE(s.result, AutomationResult::Success);
    qDebug() << "Task automation throughput:" << count << "tasks in" << elapsed << "ms";
  }

  void testTestAutomationThroughput() {
    WorkflowAutomationService svc;
    QElapsedTimer timer;
    timer.start();

    const int count = 500;
    for (int i = 0; i < count; i++) {
      TestConfig cfg;
      cfg.tests << QStringLiteral("test_%1").arg(i);
      cfg.environment = QStringLiteral("perf");
      cfg.failFast = true;
      svc.automateTest(cfg);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Test automation throughput:" << count << "tests in" << elapsed << "ms";
  }

  void testDeployAutomationThroughput() {
    WorkflowAutomationService svc;
    QElapsedTimer timer;
    timer.start();

    const int count = 200;
    for (int i = 0; i < count; i++) {
      DeployConfig cfg;
      cfg.target = QStringLiteral("target_%1").arg(i);
      cfg.version = QStringLiteral("1.0.%1").arg(i);
      cfg.dryRun = true;
      svc.automateDeploy(cfg);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Deploy automation throughput:" << count << "deploys in" << elapsed << "ms";
  }

  void testStatusQueryLatency() {
    WorkflowAutomationService svc;
    for (int i = 0; i < 100; i++) {
      AutoTaskConfig cfg;
      cfg.task = QStringLiteral("task_%1").arg(i);
      svc.automateTask(cfg);
    }

    QElapsedTimer timer;
    timer.start();

    const int iterations = 10000;
    for (int i = 0; i < iterations; i++) {
      svc.status(QStringLiteral("task_%1").arg(i % 100));
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "Status query latency:" << iterations << "queries in" << elapsed << "ms";
  }

  void testSignalThroughput() {
    WorkflowAutomationService svc;
    QSignalSpy startedSpy(&svc, &WorkflowAutomationService::automationStarted);
    QSignalSpy completedSpy(&svc, &WorkflowAutomationService::automationCompleted);

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      AutoTaskConfig cfg;
      cfg.task = QStringLiteral("sig_%1").arg(i);
      svc.automateTask(cfg);
    }

    qint64 elapsed = timer.elapsed();
    QCOMPARE(startedSpy.count(), count);
    QCOMPARE(completedSpy.count(), count);
    QVERIFY(elapsed < 5000);
    qDebug() << "Signal throughput:" << count << "automations in" << elapsed << "ms";
  }

  void testMemoryStability() {
    WorkflowAutomationService svc;

    for (int round = 0; round < 10; round++) {
      for (int i = 0; i < 25; i++) {
        AutoTaskConfig cfg;
        cfg.task = QStringLiteral("task_%1_%2").arg(round).arg(i);
        svc.automateTask(cfg);

        TestConfig testCfg;
        testCfg.tests << QStringLiteral("test_%1_%2").arg(round).arg(i);
        svc.automateTest(testCfg);

        DeployConfig deployCfg;
        deployCfg.target = QStringLiteral("target_%1_%2").arg(round).arg(i);
        svc.automateDeploy(deployCfg);

        MonitorConfig monCfg;
        monCfg.metrics << QStringLiteral("cpu");
        svc.automateMonitor(monCfg);
      }
    }

    QCOMPARE(svc.allStatuses().size(), 4);
    qDebug() << "Memory stability: 1000 automations across 4 types";
  }
};

QTEST_MAIN(WorkflowAutomationPerformanceTest)
#include "workflow_automation_performance_test.moc"
