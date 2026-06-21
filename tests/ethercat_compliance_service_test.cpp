// EtherCATComplianceServiceTest — Tests for EtherCATComplianceService
//
// Test coverage:
//   - Default compliance rules (safety, timing, config, network)
//   - Rule management (add, remove)
//   - Compliance check execution and scoring
//   - Category-specific compliance checks

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATComplianceService.h"

class EtherCATComplianceServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Default rules are safety, timing, config, network
  void testDefaultRules() {
    EtherCATComplianceService svc;
    QCOMPARE(svc.rules().size(), 4);
    QCOMPARE(svc.rules().at(0).ruleId, QString("SAFETY-001"));
    QCOMPARE(svc.rules().at(0).category, QString("Safety"));
    QCOMPARE(svc.rules().at(1).ruleId, QString("TIMING-001"));
    QCOMPARE(svc.rules().at(1).category, QString("Timing"));
    QCOMPARE(svc.rules().at(2).ruleId, QString("CONFIG-001"));
    QCOMPARE(svc.rules().at(2).category, QString("Configuration"));
    QCOMPARE(svc.rules().at(3).ruleId, QString("NET-001"));
    QCOMPARE(svc.rules().at(3).category, QString("Network"));
  }

  // Add custom compliance rule
  void testAddRule() {
    EtherCATComplianceService svc;
    ComplianceRule r;
    r.ruleId = "CUSTOM-001";
    r.category = "Custom";
    r.description = "Custom rule";
    r.severity = 1;
    svc.addRule(r);
    QCOMPARE(svc.rules().size(), 5);
    QCOMPARE(svc.rules().last().ruleId, QString("CUSTOM-001"));
  }

  // Remove existing rule
  void testRemoveRule() {
    EtherCATComplianceService svc;
    QVERIFY(svc.removeRule("SAFETY-001"));
    QCOMPARE(svc.rules().size(), 3);
  }

  // Remove nonexistent rule returns false
  void testRemoveNonexistentRule() {
    EtherCATComplianceService svc;
    QVERIFY(!svc.removeRule("NONEXISTENT"));
    QCOMPARE(svc.rules().size(), 4);
  }

  // Full compliance check passes all rules
  void testRunComplianceCheck() {
    EtherCATComplianceService svc;
    ComplianceReport report = svc.runComplianceCheck();
    QCOMPARE(report.totalRules, 4);
    QCOMPARE(report.passedCount, 4);
    QCOMPARE(report.failedCount, 0);
    QCOMPARE(report.score, 100.0);
    QCOMPARE(report.results.size(), 4);
  }

  // Check single category
  void testCheckCategory() {
    EtherCATComplianceService svc;
    ComplianceReport report = svc.checkCategory("Safety");
    QCOMPARE(report.totalRules, 1);
    QCOMPARE(report.passedCount, 1);
    QCOMPARE(report.results.at(0).ruleId, QString("SAFETY-001"));
  }

  // checkCompleted signal fires on check
  void testCheckCompletedSignal() {
    EtherCATComplianceService svc;
    QSignalSpy spy(&svc, &EtherCATComplianceService::checkCompleted);
    svc.runComplianceCheck();
    QCOMPARE(spy.count(), 1);
  }

  // Compliance score is 100 when all pass
  void testComplianceScore() {
    EtherCATComplianceService svc;
    ComplianceReport report = svc.runComplianceCheck();
    QVERIFY(report.score > 0.0);
    QCOMPARE(report.score, 100.0);
  }
};

QTEST_MAIN(EtherCATComplianceServiceTest)
#include "ethercat_compliance_service_test.moc"
