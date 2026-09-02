// EtherCATComplianceServiceTest — Tests for EtherCATComplianceService
//
// Test coverage:
//   - Default compliance rules (safety, timing, config, network)
//   - Rule management (add, remove)
//   - Compliance check execution and scoring
//   - Category-specific compliance checks

#include "services/EtherCATComplianceService.h"
#include <QFile>
#include <QSignalSpy>
#include <QTest>

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

    // Full compliance check must not synthesize passing evidence.
    void testRunComplianceCheckFailsClosedWithoutBackend() {
        EtherCATComplianceService svc;
        QSignalSpy spy(&svc, &EtherCATComplianceService::checkCompleted);
        ComplianceReport report = svc.runComplianceCheck();
        QCOMPARE(report.totalRules, 4);
        QCOMPARE(report.passedCount, 0);
        QCOMPARE(report.failedCount, 4);
        QCOMPARE(report.score, 0.0);
        QCOMPARE(report.results.size(), 4);
        for (const auto& result : report.results) {
            QVERIFY(!result.passed);
            QVERIFY(result.details.contains(QStringLiteral("requires a real compliance backend")));
        }
        QCOMPARE(spy.count(), 0);
    }

    // Category checks must not synthesize passing evidence.
    void testCheckCategoryFailsClosedWithoutBackend() {
        EtherCATComplianceService svc;
        ComplianceReport report = svc.checkCategory("Safety");
        QCOMPARE(report.totalRules, 1);
        QCOMPARE(report.passedCount, 0);
        QCOMPARE(report.failedCount, 1);
        QCOMPARE(report.score, 0.0);
        QCOMPARE(report.results.at(0).ruleId, QString("SAFETY-001"));
        QVERIFY(!report.results.at(0).passed);
    }

    // checkCompleted is not emitted for rejected offline checks.
    void testCheckCompletedSignalNotEmittedWithoutBackend() {
        EtherCATComplianceService svc;
        QSignalSpy spy(&svc, &EtherCATComplianceService::checkCompleted);
        svc.runComplianceCheck();
        QCOMPARE(spy.count(), 0);
    }

    // Compliance score is zero without evidence.
    void testComplianceScoreWithoutBackend() {
        EtherCATComplianceService svc;
        ComplianceReport report = svc.runComplianceCheck();
        QCOMPARE(report.score, 0.0);
    }

    // Implementation must not keep synthetic compliance pass paths.
    void testImplementationDoesNotContainSyntheticComplianceSuccessPath() {
        QFile source(QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/services/EtherCATComplianceService.cpp"));
        QVERIFY2(source.open(QIODevice::ReadOnly | QIODevice::Text), qPrintable(source.errorString()));
        const QString text = QString::fromUtf8(source.readAll());

        QVERIFY2(!text.contains(QStringLiteral("result.passed = true")),
                 "Compliance checks must not synthesize passing rule results.");
        QVERIFY2(!text.contains(QStringLiteral("score = report.totalRules > 0 ? 100.0")),
                 "Compliance checks must not synthesize a 100 score.");
        QVERIFY2(!text.contains(QStringLiteral("emit checkCompleted(report)")),
                 "Compliance checks must not emit completion for rejected offline checks.");
    }
};

QTEST_MAIN(EtherCATComplianceServiceTest)
#include "ethercat_compliance_service_test.moc"
