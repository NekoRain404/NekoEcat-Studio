#include <QTest>
#include <QElapsedTimer>
#include "services/EtherCATCertificationService.h"

class EtherCATCertificationPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testAddRequirementPerformance() {
    EtherCATCertificationService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      CertificationRequirement r;
      r.requirementId = QStringLiteral("PERF-%1").arg(i);
      r.category = "Performance";
      r.description = "Performance test requirement";
      r.mandatory = true;
      svc.addRequirement(r);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "10000 addRequirement() calls:" << elapsed << "ms";
  }

  void testRunCertificationPerformance() {
    EtherCATCertificationService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      CertificationReport report = svc.runCertification();
      QVERIFY(!report.overallPass);
      QCOMPARE(report.notTestedCount, report.totalRequirements);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "10000 runCertification() calls:" << elapsed << "ms";
  }

  void testTestRequirementPerformance() {
    EtherCATCertificationService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      CertificationTestResult result = svc.testRequirement("CONF-001");
      QCOMPARE(result.status, CertificationTestStatus::NotTested);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "10000 testRequirement() calls:" << elapsed << "ms";
  }

  void testRequirementsPerformance() {
    EtherCATCertificationService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      svc.requirements();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "10000 requirements() calls:" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATCertificationPerformanceTest)
#include "ethercat_certification_performance_test.moc"
