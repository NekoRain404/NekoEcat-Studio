#pragma once

// EtherCATSecurityService — security policy management, access control,
// and audit logging for EtherCAT networks.
//
// Provides policy configuration, access validation, and audit trail
// queries. Emits signals when policy changes or violations occur.
//
// Thread safety: main (GUI) thread only.

#include <QDateTime>
#include <QObject>
#include <QString>
#include <QVector>

struct SecurityPolicy {
    int level = 1;
    bool encryptionEnabled = false;
    bool authenticationRequired = false;
    QVector<QString> accessControlList;
};

struct SecurityAuditEntry {
    qint64 timestamp = 0;
    QString eventType;
    QString userId;
    QString description;
    int severity = 0;
};

struct SecurityAuditResult {
    QVector<SecurityAuditEntry> entries;
    int totalEvents = 0;
    int criticalCount = 0;
    int warningCount = 0;
};

class EtherCATSecurityService : public QObject {
    Q_OBJECT
public:
    explicit EtherCATSecurityService(QObject* parent = nullptr);

    void setSecurityPolicy(const SecurityPolicy& policy);
    SecurityPolicy currentPolicy() const;
    SecurityAuditResult runAudit();
    bool validateAccess(const QString& userId, const QString& resource);
    QVector<SecurityAuditEntry> recentEvents(int count);

signals:
    void policyChanged();
    void auditCompleted(const SecurityAuditResult& result);
    void accessViolation(const SecurityAuditEntry& entry);

private:
    SecurityPolicy policy_;
    QVector<SecurityAuditEntry> auditLog_;
};
