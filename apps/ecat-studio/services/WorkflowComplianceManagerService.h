#pragma once

// WorkflowComplianceManagerService — high-level compliance rule management
// for workflow operations. Builds on WorkflowComplianceService to provide
// rule CRUD, audit scheduling, and compliance tracking.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>
#include <QVector>
#include <QDateTime>

struct WfComplianceRule {
  QString id;
  QString name;
  QString category;
  QString requirement;
  bool active = true;
  QDateTime createdAt;
  QDateTime lastAudit;
};

class WorkflowComplianceManagerService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowComplianceManagerService(QObject *parent = nullptr);

  QString addRule(const QString &name, const QString &category,
                  const QString &requirement);
  bool removeRule(const QString &ruleId);
  bool activateRule(const QString &ruleId);
  bool deactivateRule(const QString &ruleId);
  WfComplianceRule rule(const QString &ruleId) const;
  QVector<WfComplianceRule> allRules() const;
  int ruleCount() const;
  int activeRuleCount() const;
  bool auditRule(const QString &ruleId);
  QVector<WfComplianceRule> rulesByCategory(const QString &category) const;

signals:
  void ruleAdded(const QString &ruleId);
  void ruleRemoved(const QString &ruleId);
  void ruleActivated(const QString &ruleId);
  void ruleDeactivated(const QString &ruleId);
  void ruleAudited(const QString &ruleId);

private:
  QVector<WfComplianceRule> rules_;
  int nextId_ = 1;
};
