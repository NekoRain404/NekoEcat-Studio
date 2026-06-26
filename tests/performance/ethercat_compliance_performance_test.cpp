#include <QTest>
#include <QElapsedTimer>
#include "services/EtherCATComplianceService.h"

class EtherCATCompliancePerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testAddRulePerformance() {
    EtherCATComplianceService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      ComplianceRule r;
      r.ruleId = QStringLiteral("PERF-%1").arg(i);
      r.category = "Performance";
      r.description = "Performance test rule";
      r.severity = 1;
      svc.addRule(r);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "10000 addRule() calls:" << elapsed << "ms";
  }

  void testRunComplianceCheckPerformance() {
    EtherCATComplianceService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      ComplianceReport report = svc.runComplianceCheck();
      QCOMPARE(report.passedCount, 0);
      QCOMPARE(report.score, 0.0);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "10000 runComplianceCheck() calls:" << elapsed << "ms";
  }

  void testCheckCategoryPerformance() {
    EtherCATComplianceService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      ComplianceReport report = svc.checkCategory("Safety");
      QCOMPARE(report.passedCount, 0);
      QCOMPARE(report.score, 0.0);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "10000 checkCategory() calls:" << elapsed << "ms";
  }

  void testRulesPerformance() {
    EtherCATComplianceService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      svc.rules();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "10000 rules() calls:" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATCompliancePerformanceTest)
#include "ethercat_compliance_performance_test.moc"
