#include "WorkflowSecurityService.h"

// WorkflowSecurityService.cpp — User authentication, authorization, and audit logging
//
// Implementation notes:
//   - Validates credentials with minimum password length requirement
//   - Tracks authenticated users in a QSet for session management
//   - All security events logged to audit trail with timestamps and results

WorkflowSecurityService::WorkflowSecurityService(QObject *parent)
    : QObject(parent)
{
}

bool WorkflowSecurityService::authenticateUser(const QString &username,
                                                const QString &password)
{
    AuditEvent event;
    event.timestamp = QDateTime::currentDateTime();
    event.user = username;
    event.action = QStringLiteral("authenticate");

    if (username.isEmpty() || password.size() < 4) {
        event.result = QStringLiteral("failure");
        event.details = QStringLiteral("Invalid credentials");
        logAuditEvent(event);
        return false;
    }

    authenticatedUsers_.insert(username);
    event.result = QStringLiteral("success");
    event.details = QStringLiteral("User authenticated");
    logAuditEvent(event);
    emit userAuthenticated(username);
    return true;
}

bool WorkflowSecurityService::authorizeAction(const QString &user,
                                               const QString &action)
{
    AuditEvent event;
    event.timestamp = QDateTime::currentDateTime();
    event.user = user;
    event.action = QStringLiteral("authorize");
    event.resource = action;

    if (user.isEmpty() || action.isEmpty()) {
        event.result = QStringLiteral("failure");
        event.details = QStringLiteral("Invalid user or action");
        logAuditEvent(event);
        return false;
    }

    event.result = QStringLiteral("success");
    event.details = QStringLiteral("Action authorized");
    logAuditEvent(event);
    emit actionAuthorized(user, action);
    return true;
}

void WorkflowSecurityService::logAuditEvent(const AuditEvent &event)
{
    auditLog_.append(event);
    emit auditEventLogged(event);
}

QVector<AuditEvent> WorkflowSecurityService::auditHistory(int count) const
{
    if (count <= 0 || count >= auditLog_.size())
        return auditLog_;

    QVector<AuditEvent> result;
    for (int i = auditLog_.size() - 1; i >= auditLog_.size() - count; --i)
        result.append(auditLog_[i]);
    return result;
}
