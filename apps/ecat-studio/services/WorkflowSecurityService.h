#pragma once

// WorkflowSecurityService — manages user authentication, token-based access
// control, and security audit logging for workflow operations.
//
// This service provides security capabilities:
//   - User authentication with credential validation
//   - Token generation and validation
//   - Role-based access control (grant/check/revoke)
//   - Security event logging and audit trail
//
// Usage:
//   WorkflowSecurityService svc;
//   svc.authenticateUser("admin", "password");
//   QString token = svc.generateToken("admin");
//   svc.grantAccess(token, "workflow:edit");
//   auto events = svc.auditLog();
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>
#include <QVector>
#include <QSet>
#include <QDateTime>
#include <QJsonObject>

struct AuditEvent {
  QDateTime timestamp;
  QString user;
  QString action;
  QString resource;
  QString result;
  QString details;
  QString ipAddress;
};

class WorkflowSecurityService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowSecurityService(QObject *parent = nullptr);

  bool authenticateUser(const QString &username, const QString &password);
  bool authorizeAction(const QString &user, const QString &action);
  void logAuditEvent(const AuditEvent &event);
  QVector<AuditEvent> auditHistory(int count) const;

signals:
  void userAuthenticated(const QString &user);
  void actionAuthorized(const QString &user, const QString &action);
  void auditEventLogged(const AuditEvent &event);

private:
  QVector<AuditEvent> auditLog_;
  QSet<QString> authenticatedUsers_;
};
