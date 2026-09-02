#include "WorkflowSecurityManagerService.h"

WorkflowSecurityManagerService::WorkflowSecurityManagerService(QObject* parent) : QObject(parent) {}

QString WorkflowSecurityManagerService::addPolicy(const QString& name, const QString& description,
                                                  const QString& severity) {
    WfSecurityPolicy p;
    p.id = QStringLiteral("sp-%1").arg(nextId_++);
    p.name = name;
    p.description = description;
    p.severity = severity;
    p.enabled = true;
    p.createdAt = QDateTime::currentDateTime();
    policies_.append(p);
    emit policyAdded(p.id);
    return p.id;
}

bool WorkflowSecurityManagerService::removePolicy(const QString& policyId) {
    for (int i = 0; i < policies_.size(); ++i) {
        if (policies_[i].id == policyId) {
            policies_.removeAt(i);
            emit policyRemoved(policyId);
            return true;
        }
    }
    return false;
}

bool WorkflowSecurityManagerService::enablePolicy(const QString& policyId) {
    for (auto& p : policies_) {
        if (p.id == policyId) {
            p.enabled = true;
            emit policyEnabled(policyId);
            return true;
        }
    }
    return false;
}

bool WorkflowSecurityManagerService::disablePolicy(const QString& policyId) {
    for (auto& p : policies_) {
        if (p.id == policyId) {
            p.enabled = false;
            emit policyDisabled(policyId);
            return true;
        }
    }
    return false;
}

WfSecurityPolicy WorkflowSecurityManagerService::policy(const QString& policyId) const {
    for (const auto& p : policies_) {
        if (p.id == policyId)
            return p;
    }
    return {};
}

QVector<WfSecurityPolicy> WorkflowSecurityManagerService::allPolicies() const {
    return policies_;
}

int WorkflowSecurityManagerService::policyCount() const {
    return policies_.size();
}

bool WorkflowSecurityManagerService::enforcePolicy(const QString& policyId) {
    for (const auto& p : policies_) {
        if (p.id == policyId && p.enabled) {
            emit policyEnforced(policyId);
            return true;
        }
    }
    return false;
}

int WorkflowSecurityManagerService::enabledPolicyCount() const {
    int count = 0;
    for (const auto& p : policies_) {
        if (p.enabled)
            ++count;
    }
    return count;
}
