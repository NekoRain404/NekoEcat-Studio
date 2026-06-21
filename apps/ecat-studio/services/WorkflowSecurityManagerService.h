#pragma once

// WorkflowSecurityManagerService — high-level security policy management
// for workflow operations. Builds on WorkflowSecurityService to provide
// policy CRUD, enforcement, and compliance status tracking.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>
#include <QVector>
#include <QDateTime>

struct WfSecurityPolicy {
  QString id;
  QString name;
  QString description;
  QString severity;
  bool enabled = true;
  QDateTime createdAt;
};

class WorkflowSecurityManagerService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowSecurityManagerService(QObject *parent = nullptr);

  QString addPolicy(const QString &name, const QString &description,
                    const QString &severity = QStringLiteral("medium"));
  bool removePolicy(const QString &policyId);
  bool enablePolicy(const QString &policyId);
  bool disablePolicy(const QString &policyId);
  WfSecurityPolicy policy(const QString &policyId) const;
  QVector<WfSecurityPolicy> allPolicies() const;
  int policyCount() const;
  bool enforcePolicy(const QString &policyId);
  int enabledPolicyCount() const;

signals:
  void policyAdded(const QString &policyId);
  void policyRemoved(const QString &policyId);
  void policyEnabled(const QString &policyId);
  void policyDisabled(const QString &policyId);
  void policyEnforced(const QString &policyId);

private:
  QVector<WfSecurityPolicy> policies_;
  int nextId_ = 1;
};
