#include <QTest>
#include <QElapsedTimer>
#include "services/WorkflowSecurityManagerService.h"

class WorkflowSecurityManagerPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testAddPolicyThroughput() {
      WorkflowSecurityManagerService svc;
      QElapsedTimer timer;
      timer.start();
      for (int i = 0; i < 10000; i++) {
          svc.addPolicy(QStringLiteral("P%1").arg(i), QStringLiteral("Desc"));
      }
      qint64 elapsed = timer.elapsed();
      QVERIFY(elapsed < 1000);
      QCOMPARE(svc.policyCount(), 10000);
      qDebug() << "10000 addPolicy() calls:" << elapsed << "ms";
  }

  void testQueryLatency() {
      WorkflowSecurityManagerService svc;
      for (int i = 0; i < 1000; i++)
          svc.addPolicy(QStringLiteral("P%1").arg(i), QStringLiteral("D"));
      QElapsedTimer timer;
      timer.start();
      for (int i = 0; i < 10000; i++) {
          svc.policy(QStringLiteral("P500"));
      }
      qint64 elapsed = timer.elapsed();
      QVERIFY(elapsed < 500);
      qDebug() << "10000 policy() lookups:" << elapsed << "ms";
  }

  void testEnforceThroughput() {
      WorkflowSecurityManagerService svc;
      QVector<QString> ids;
      for (int i = 0; i < 1000; i++)
          ids.append(svc.addPolicy(QStringLiteral("P%1").arg(i), QStringLiteral("D")));
      QElapsedTimer timer;
      timer.start();
      for (int i = 0; i < 10000; i++) {
          svc.enforcePolicy(ids[i % ids.size()]);
      }
      qint64 elapsed = timer.elapsed();
      QVERIFY(elapsed < 1000);
      qDebug() << "10000 enforcePolicy() calls:" << elapsed << "ms";
  }

  void testRemoveThroughput() {
      WorkflowSecurityManagerService svc;
      QVector<QString> ids;
      for (int i = 0; i < 5000; i++)
          ids.append(svc.addPolicy(QStringLiteral("P%1").arg(i), QStringLiteral("D")));
      QElapsedTimer timer;
      timer.start();
      for (const auto &id : ids) {
          svc.removePolicy(id);
      }
      qint64 elapsed = timer.elapsed();
      QVERIFY(elapsed < 1000);
      QCOMPARE(svc.policyCount(), 0);
      qDebug() << "5000 removePolicy() calls:" << elapsed << "ms";
  }
};

QTEST_MAIN(WorkflowSecurityManagerPerformanceTest)
#include "workflow_security_manager_performance_test.moc"
