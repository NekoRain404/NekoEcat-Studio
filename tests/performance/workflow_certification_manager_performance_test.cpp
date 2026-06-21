#include <QTest>
#include <QElapsedTimer>
#include "services/WorkflowCertificationManagerService.h"

class WorkflowCertificationManagerPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testAddRequirementThroughput() {
      WorkflowCertificationManagerService svc;
      QElapsedTimer timer;
      timer.start();
      for (int i = 0; i < 10000; i++) {
          svc.addRequirement(QStringLiteral("R%1").arg(i), QStringLiteral("ISO"));
      }
      qint64 elapsed = timer.elapsed();
      QVERIFY(elapsed < 1000);
      QCOMPARE(svc.requirementCount(), 10000);
      qDebug() << "10000 addRequirement() calls:" << elapsed << "ms";
  }

  void testQueryLatency() {
      WorkflowCertificationManagerService svc;
      for (int i = 0; i < 1000; i++)
          svc.addRequirement(QStringLiteral("R%1").arg(i), QStringLiteral("ISO"));
      QElapsedTimer timer;
      timer.start();
      for (int i = 0; i < 10000; i++) {
          svc.requirement(QStringLiteral("R500"));
      }
      qint64 elapsed = timer.elapsed();
      QVERIFY(elapsed < 500);
      qDebug() << "10000 requirement() lookups:" << elapsed << "ms";
  }

  void testStatusUpdateThroughput() {
      WorkflowCertificationManagerService svc;
      QVector<QString> ids;
      for (int i = 0; i < 1000; i++)
          ids.append(svc.addRequirement(QStringLiteral("R%1").arg(i),
                                        QStringLiteral("ISO")));
      QElapsedTimer timer;
      timer.start();
      for (int i = 0; i < 10000; i++) {
          svc.updateStatus(ids[i % ids.size()], QStringLiteral("approved"));
      }
      qint64 elapsed = timer.elapsed();
      QVERIFY(elapsed < 1000);
      qDebug() << "10000 updateStatus() calls:" << elapsed << "ms";
  }

  void testStandardFilterThroughput() {
      WorkflowCertificationManagerService svc;
      for (int i = 0; i < 5000; i++)
          svc.addRequirement(QStringLiteral("R%1").arg(i),
                             i % 2 == 0 ? QStringLiteral("ISO") : QStringLiteral("CE"));
      QElapsedTimer timer;
      timer.start();
      for (int i = 0; i < 10000; i++) {
          svc.requirementsByStandard(QStringLiteral("ISO"));
      }
      qint64 elapsed = timer.elapsed();
      QVERIFY(elapsed < 10000);
      qDebug() << "10000 requirementsByStandard() calls:" << elapsed << "ms";
  }
};

QTEST_MAIN(WorkflowCertificationManagerPerformanceTest)
#include "workflow_certification_manager_performance_test.moc"
