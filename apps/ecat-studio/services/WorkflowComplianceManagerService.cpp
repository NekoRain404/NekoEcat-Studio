#include "WorkflowComplianceManagerService.h"

WorkflowComplianceManagerService::WorkflowComplianceManagerService(QObject *parent)
    : QObject(parent)
{
}

QString WorkflowComplianceManagerService::addRule(const QString &name,
                                                   const QString &category,
                                                   const QString &requirement)
{
    WfComplianceRule r;
    r.id = QStringLiteral("cr-%1").arg(nextId_++);
    r.name = name;
    r.category = category;
    r.requirement = requirement;
    r.active = true;
    r.createdAt = QDateTime::currentDateTime();
    rules_.append(r);
    emit ruleAdded(r.id);
    return r.id;
}

bool WorkflowComplianceManagerService::removeRule(const QString &ruleId)
{
    for (int i = 0; i < rules_.size(); ++i) {
        if (rules_[i].id == ruleId) {
            rules_.removeAt(i);
            emit ruleRemoved(ruleId);
            return true;
        }
    }
    return false;
}

bool WorkflowComplianceManagerService::activateRule(const QString &ruleId)
{
    for (auto &r : rules_) {
        if (r.id == ruleId) {
            r.active = true;
            emit ruleActivated(ruleId);
            return true;
        }
    }
    return false;
}

bool WorkflowComplianceManagerService::deactivateRule(const QString &ruleId)
{
    for (auto &r : rules_) {
        if (r.id == ruleId) {
            r.active = false;
            emit ruleDeactivated(ruleId);
            return true;
        }
    }
    return false;
}

WfComplianceRule WorkflowComplianceManagerService::rule(const QString &ruleId) const
{
    for (const auto &r : rules_) {
        if (r.id == ruleId)
            return r;
    }
    return {};
}

QVector<WfComplianceRule> WorkflowComplianceManagerService::allRules() const
{
    return rules_;
}

int WorkflowComplianceManagerService::ruleCount() const
{
    return rules_.size();
}

int WorkflowComplianceManagerService::activeRuleCount() const
{
    int count = 0;
    for (const auto &r : rules_) {
        if (r.active)
            ++count;
    }
    return count;
}

bool WorkflowComplianceManagerService::auditRule(const QString &ruleId)
{
    for (auto &r : rules_) {
        if (r.id == ruleId) {
            r.lastAudit = QDateTime::currentDateTime();
            emit ruleAudited(ruleId);
            return true;
        }
    }
    return false;
}

QVector<WfComplianceRule> WorkflowComplianceManagerService::rulesByCategory(
    const QString &category) const
{
    QVector<WfComplianceRule> result;
    for (const auto &r : rules_) {
        if (r.category == category)
            result.append(r);
    }
    return result;
}
