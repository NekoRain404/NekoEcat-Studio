#include "EtherCATSecurityService.h"
#include <QDateTime>

// EtherCATSecurityService.cpp — Security policy management and audit logging
//
// Implementation notes:
//   - Initializes default policy with encryption, authentication, and ACL
//   - Maintains audit log with timestamped entries and severity levels
//   - Audit aggregates critical/warning counts from log entries

EtherCATSecurityService::EtherCATSecurityService(QObject* parent) : QObject(parent) {
    policy_.level = 2;
    policy_.encryptionEnabled = true;
    policy_.authenticationRequired = true;
    policy_.accessControlList.append(QStringLiteral("admin"));
    policy_.accessControlList.append(QStringLiteral("operator"));

    SecurityAuditEntry entry;
    entry.timestamp = QDateTime::currentMSecsSinceEpoch();
    entry.eventType = QStringLiteral("system_start");
    entry.userId = QStringLiteral("system");
    entry.description = QStringLiteral("Security service initialized.");
    entry.severity = 0;
    auditLog_.append(entry);
}

void EtherCATSecurityService::setSecurityPolicy(const SecurityPolicy& policy) {
    policy_ = policy;
    emit policyChanged();
}

SecurityPolicy EtherCATSecurityService::currentPolicy() const {
    return policy_;
}

SecurityAuditResult EtherCATSecurityService::runAudit() {
    SecurityAuditResult result;
    result.entries = auditLog_;
    result.totalEvents = auditLog_.size();
    result.criticalCount = 0;
    result.warningCount = 0;
    for (const auto& e : auditLog_) {
        if (e.severity >= 3)
            result.criticalCount++;
        else if (e.severity >= 2)
            result.warningCount++;
    }
    emit auditCompleted(result);
    return result;
}

bool EtherCATSecurityService::validateAccess(const QString& userId, const QString& resource) {
    Q_UNUSED(resource);
    if (!policy_.authenticationRequired)
        return true;
    if (policy_.accessControlList.contains(userId))
        return true;

    SecurityAuditEntry violation;
    violation.timestamp = QDateTime::currentMSecsSinceEpoch();
    violation.eventType = QStringLiteral("access_denied");
    violation.userId = userId;
    violation.description = QStringLiteral("Access denied for user: ") + userId;
    violation.severity = 3;
    auditLog_.append(violation);
    emit accessViolation(violation);
    return false;
}

QVector<SecurityAuditEntry> EtherCATSecurityService::recentEvents(int count) {
    if (count >= auditLog_.size())
        return auditLog_;
    return auditLog_.mid(auditLog_.size() - count);
}
