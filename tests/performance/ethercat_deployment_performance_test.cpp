#include <QTest>
#include <QElapsedTimer>
#include "services/EtherCATDeploymentService.h"

class EtherCATDeploymentPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testDeployPerformance() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.deployConfiguration("target", "config");
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 500);
    qDebug() << "1000 deploy calls:" << elapsed << "ms";
  }

  void testListPerformance() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    for (int i = 0; i < 100; i++) {
      svc.deployConfiguration("target", "config");
    }
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.listDeployments();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 100);
    qDebug() << "1000 list calls:" << elapsed << "ms";
  }

  void testRollbackPerformance() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    QVector<DeploymentResult> deploys;
    for (int i = 0; i < 100; i++) {
      deploys.append(svc.deployConfiguration("target", "config"));
    }
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.rollbackDeployment(deploys[i % deploys.size()].id);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 500);
    qDebug() << "1000 rollback calls:" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATDeploymentPerformanceTest)
#include "ethercat_deployment_performance_test.moc"
