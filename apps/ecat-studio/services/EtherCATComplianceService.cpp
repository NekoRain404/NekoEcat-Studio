#include "EtherCATComplianceService.h"

// EtherCATComplianceService.cpp — Compliance rule management and request facade
//
// Implementation notes:
//   - Pre-defines safety, timing, configuration, and network compliance rules
//   - Rules have severity levels (1-3) for prioritization
//   - Checks fail closed until a real evidence-producing backend exists

EtherCATComplianceService::EtherCATComplianceService(QObject *parent)
    : QObject(parent)
{
    ComplianceRule r1;
    r1.ruleId = QStringLiteral("SAFETY-001");
    r1.category = QStringLiteral("Safety");
    r1.description = QStringLiteral("Emergency stop must be configured.");
    r1.severity = 3;
    rules_.append(r1);

    ComplianceRule r2;
    r2.ruleId = QStringLiteral("TIMING-001");
    r2.category = QStringLiteral("Timing");
    r2.description = QStringLiteral("DC sync cycle time must not exceed 1ms.");
    r2.severity = 2;
    rules_.append(r2);

    ComplianceRule r3;
    r3.ruleId = QStringLiteral("CONFIG-001");
    r3.category = QStringLiteral("Configuration");
    r3.description = QStringLiteral("All slaves must have valid ESI files.");
    r3.severity = 2;
    rules_.append(r3);

    ComplianceRule r4;
    r4.ruleId = QStringLiteral("NET-001");
    r4.category = QStringLiteral("Network");
    r4.description = QStringLiteral("Network redundancy should be enabled.");
    r4.severity = 1;
    rules_.append(r4);
}

void EtherCATComplianceService::addRule(const ComplianceRule &rule)
{
    rules_.append(rule);
    emit ruleAdded(rule);
}

bool EtherCATComplianceService::removeRule(const QString &ruleId)
{
    for (int i = 0; i < rules_.size(); ++i) {
        if (rules_[i].ruleId == ruleId) {
            rules_.removeAt(i);
            emit ruleRemoved(ruleId);
            return true;
        }
    }
    return false;
}

QVector<ComplianceRule> EtherCATComplianceService::rules() const
{
    return rules_;
}

ComplianceReport EtherCATComplianceService::runComplianceCheck()
{
    ComplianceReport report;
    for (const auto &rule : rules_) {
        if (!rule.enabled)
            continue;
        ComplianceCheckResult result;
        result.ruleId = rule.ruleId;
        result.passed = false;
        result.details = rule.description + QStringLiteral(" — requires a real compliance backend.");
        result.recommendation = QStringLiteral("Run this rule against a live compliance backend before claiming compliance.");
        report.results.append(result);
    }
    report.totalRules = report.results.size();
    report.passedCount = 0;
    report.failedCount = report.results.size();
    report.score = 0.0;
    return report;
}

ComplianceReport EtherCATComplianceService::checkCategory(const QString &category)
{
    ComplianceReport report;
    for (const auto &rule : rules_) {
        if (!rule.enabled || rule.category != category)
            continue;
        ComplianceCheckResult result;
        result.ruleId = rule.ruleId;
        result.passed = false;
        result.details = rule.description + QStringLiteral(" — requires a real compliance backend.");
        result.recommendation = QStringLiteral("Run this rule against a live compliance backend before claiming compliance.");
        report.results.append(result);
    }
    report.totalRules = report.results.size();
    report.passedCount = 0;
    report.failedCount = report.results.size();
    report.score = 0.0;
    return report;
}
