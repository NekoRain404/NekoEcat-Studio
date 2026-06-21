#pragma once

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QPushButton;
class QTableWidget;
class QTextEdit;

class SecurityPolicy {
public:
  QString level;
  bool encryption = false;
  bool authentication = false;
  QString acl;
};

class SecurityAuditResult {
public:
  QString time;
  QString event;
  QString user;
  QString description;
  QString severity;
};

class SecurityManagerPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit SecurityManagerPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  enum class AuditState { Idle, Running, Completed };

  AuditState auditState() const;
  bool isAuditing() const;

  SecurityPolicy currentPolicy() const;
  void setPolicy(const SecurityPolicy &policy);

  QVector<SecurityAuditResult> auditResults() const;
  void clearAuditResults();

  QTableWidget *policyTable() const;
  QTableWidget *auditResultsTable() const;
  QTextEdit *auditLog() const;

signals:
  void auditStateChanged(SecurityManagerPlugin::AuditState state);
  void policyChanged(const SecurityPolicy &policy);
  void auditCompleted();

public slots:
  void runAudit();
  void applyPolicy();

private:
  void buildUi();
  void populatePolicyTable();
  void updateAuditResultsTable();

  QWidget *containerWidget_ = nullptr;
  AuditState auditState_ = AuditState::Idle;

  QTableWidget *policyTable_ = nullptr;
  QTableWidget *auditResultsTable_ = nullptr;
  QTextEdit *auditLog_ = nullptr;
  QPushButton *runAuditBtn_ = nullptr;
  QPushButton *applyPolicyBtn_ = nullptr;
  QLabel *auditStateLabel_ = nullptr;

  SecurityPolicy policy_;
  QVector<SecurityAuditResult> auditResults_;
};
