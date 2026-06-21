#pragma once

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QPushButton;
class QTableWidget;

class ComplianceRule {
public:
  QString id;
  QString category;
  QString description;
  QString severity;
  bool enabled = true;
};

class ComplianceResult {
public:
  QString ruleId;
  bool passed = false;
  QString details;
  QString recommendation;
};

class ComplianceCheckerPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit ComplianceCheckerPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  QVector<ComplianceRule> rules() const;
  void addRule(const ComplianceRule &rule);
  void clearRules();

  QVector<ComplianceResult> results() const;
  void clearResults();

  double complianceScore() const;

  QTableWidget *rulesTable() const;
  QTableWidget *resultsTable() const;
  QLabel *scoreLabel() const;

signals:
  void checkCompleted(double score);
  void rulesChanged();

public slots:
  void runCheck();
  void addRule();

private:
  void buildUi();
  void updateRulesTable();
  void updateResultsTable();
  void updateScoreLabel();

  QWidget *containerWidget_ = nullptr;

  QTableWidget *rulesTable_ = nullptr;
  QTableWidget *resultsTable_ = nullptr;
  QLabel *scoreLabel_ = nullptr;
  QPushButton *runCheckBtn_ = nullptr;
  QPushButton *addRuleBtn_ = nullptr;

  QVector<ComplianceRule> rules_;
  QVector<ComplianceResult> results_;
  double score_ = 0.0;
};
