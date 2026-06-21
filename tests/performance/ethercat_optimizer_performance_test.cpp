#include <QTest>
#include <QElapsedTimer>
#include "services/EtherCATOptimizerService.h"

class EtherCATOptimizerPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testOptimizeConfigurationPerformance() {
    EtherCATOptimizerService svc(nullptr, nullptr);
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      svc.optimizeConfiguration();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "10000 optimizeConfiguration() calls:" << elapsed << "ms";
  }

  void testOptimizeTimingPerformance() {
    EtherCATOptimizerService svc(nullptr, nullptr);
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      svc.optimizeTiming();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "10000 optimizeTiming() calls:" << elapsed << "ms";
  }

  void testOptimizeBuffersPerformance() {
    EtherCATOptimizerService svc(nullptr, nullptr);
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      svc.optimizeBuffers();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "10000 optimizeBuffers() calls:" << elapsed << "ms";
  }

  void testOptimizePrioritiesPerformance() {
    EtherCATOptimizerService svc(nullptr, nullptr);
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      svc.optimizePriorities();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "10000 optimizePriorities() calls:" << elapsed << "ms";
  }

  void testMixedOptimizationPerformance() {
    EtherCATOptimizerService svc(nullptr, nullptr);
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 2500; i++) {
      svc.optimizeConfiguration();
      svc.optimizeTiming();
      svc.optimizeBuffers();
      svc.optimizePriorities();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "10000 mixed optimizations:" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATOptimizerPerformanceTest)
#include "ethercat_optimizer_performance_test.moc"
