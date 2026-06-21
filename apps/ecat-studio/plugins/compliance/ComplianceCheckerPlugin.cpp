#include "ComplianceCheckerPlugin.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>

ComplianceCheckerPlugin::ComplianceCheckerPlugin(QObject *parent) {
  if (parent) setParent(parent);

  ComplianceRule r1;
  r1.id = "C001";
  r1.category = tr("Protocol");
  r1.description = tr("EtherCAT frame CRC must be valid");
  r1.severity = tr("Critical");
  rules_.append(r1);

  ComplianceRule r2;
  r2.id = "C002";
  r2.category = tr("Timing");
  r2.description = tr("Cycle time must not exceed configured maximum");
  r2.severity = tr("Warning");
  rules_.append(r2);

  ComplianceRule r3;
  r3.id = "C003";
  r3.category = tr("Safety");
  r3.description = tr("Emergency stop must be functional");
  r3.severity = tr("Critical");
  rules_.append(r3);

  buildUi();
}

QString ComplianceCheckerPlugin::id() const { return "compliance"; }
QString ComplianceCheckerPlugin::displayName() const { return "Compliance"; }
QString ComplianceCheckerPlugin::displayNameZh() const { return QStringLiteral("合规"); }
QIcon ComplianceCheckerPlugin::icon() const { return QIcon::fromTheme("dialog-ok-apply"); }
int ComplianceCheckerPlugin::defaultOrder() const { return 212; }
bool ComplianceCheckerPlugin::visible() const { return true; }

void ComplianceCheckerPlugin::activate() {}
void ComplianceCheckerPlugin::deactivate() {}

QWidget *ComplianceCheckerPlugin::widget() { return containerWidget_; }

QVector<ComplianceRule> ComplianceCheckerPlugin::rules() const { return rules_; }
void ComplianceCheckerPlugin::addRule(const ComplianceRule &rule) {
  rules_.append(rule);
  updateRulesTable();
  emit rulesChanged();
}
void ComplianceCheckerPlugin::clearRules() {
  rules_.clear();
  updateRulesTable();
}

QVector<ComplianceResult> ComplianceCheckerPlugin::results() const { return results_; }
void ComplianceCheckerPlugin::clearResults() {
  results_.clear();
  score_ = 0.0;
  updateResultsTable();
  updateScoreLabel();
}

double ComplianceCheckerPlugin::complianceScore() const { return score_; }

QTableWidget *ComplianceCheckerPlugin::rulesTable() const { return rulesTable_; }
QTableWidget *ComplianceCheckerPlugin::resultsTable() const { return resultsTable_; }
QLabel *ComplianceCheckerPlugin::scoreLabel() const { return scoreLabel_; }

void ComplianceCheckerPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  auto *toolbar = new QWidget;
  auto *toolbarLayout = new QHBoxLayout(toolbar);
  toolbarLayout->setContentsMargins(4, 2, 4, 2);

  runCheckBtn_ = new QPushButton(tr("Run Check"));
  addRuleBtn_ = new QPushButton(tr("Add Rule"));

  toolbarLayout->addWidget(runCheckBtn_);
  toolbarLayout->addWidget(addRuleBtn_);

  scoreLabel_ = new QLabel(tr("Score: N/A"));
  scoreLabel_->setStyleSheet("font-weight: bold; padding: 0 8px;");
  toolbarLayout->addWidget(scoreLabel_);
  toolbarLayout->addStretch();
  mainLayout->addWidget(toolbar);

  auto *splitter = new QSplitter(Qt::Horizontal);

  auto *leftPanel = new QWidget;
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(4, 4, 4, 4);
  leftLayout->addWidget(new QLabel(tr("Compliance Rules")));
  rulesTable_ = new QTableWidget;
  rulesTable_->setColumnCount(5);
  rulesTable_->setHorizontalHeaderLabels(
      {tr("ID"), tr("Category"), tr("Description"), tr("Severity"), tr("Enabled")});
  rulesTable_->horizontalHeader()->setStretchLastSection(true);
  rulesTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  updateRulesTable();
  leftLayout->addWidget(rulesTable_);
  splitter->addWidget(leftPanel);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);
  rightLayout->setContentsMargins(4, 4, 4, 4);
  rightLayout->addWidget(new QLabel(tr("Check Results")));
  resultsTable_ = new QTableWidget;
  resultsTable_->setColumnCount(4);
  resultsTable_->setHorizontalHeaderLabels(
      {tr("Rule ID"), tr("Passed"), tr("Details"), tr("Recommendation")});
  resultsTable_->horizontalHeader()->setStretchLastSection(true);
  resultsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  resultsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  rightLayout->addWidget(resultsTable_);
  splitter->addWidget(rightPanel);

  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 2);
  mainLayout->addWidget(splitter);

  connect(runCheckBtn_, &QPushButton::clicked, this, [this]() { runCheck(); });
  connect(addRuleBtn_, &QPushButton::clicked, this, [this]() { addRule(); });
}

void ComplianceCheckerPlugin::runCheck() {
  results_.clear();
  int passed = 0;
  for (const auto &rule : rules_) {
    if (!rule.enabled) continue;
    ComplianceResult result;
    result.ruleId = rule.id;
    result.passed = (QRandomGenerator::global()->bounded(3) != 0);
    result.details = result.passed ? tr("Check passed") : tr("Check failed");
    result.recommendation = result.passed ? tr("None") : tr("Review and fix");
    results_.append(result);
    if (result.passed) ++passed;
  }
  score_ = results_.isEmpty() ? 0.0 : (100.0 * passed / results_.size());
  updateResultsTable();
  updateScoreLabel();
  emit checkCompleted(score_);
}

void ComplianceCheckerPlugin::addRule() {
  ComplianceRule rule;
  rule.id = QStringLiteral("C%1").arg(rules_.size() + 1, 3, 10, QChar('0'));
  rule.category = tr("Custom");
  rule.description = tr("New compliance rule");
  rule.severity = tr("Info");
  rules_.append(rule);
  updateRulesTable();
  emit rulesChanged();
}

void ComplianceCheckerPlugin::updateRulesTable() {
  rulesTable_->setRowCount(rules_.size());
  for (int i = 0; i < rules_.size(); ++i) {
    const auto &r = rules_[i];
    rulesTable_->setItem(i, 0, new QTableWidgetItem(r.id));
    rulesTable_->setItem(i, 1, new QTableWidgetItem(r.category));
    rulesTable_->setItem(i, 2, new QTableWidgetItem(r.description));
    rulesTable_->setItem(i, 3, new QTableWidgetItem(r.severity));
    rulesTable_->setItem(i, 4, new QTableWidgetItem(r.enabled ? tr("Yes") : tr("No")));
  }
}

void ComplianceCheckerPlugin::updateResultsTable() {
  resultsTable_->setRowCount(results_.size());
  for (int i = 0; i < results_.size(); ++i) {
    const auto &r = results_[i];
    resultsTable_->setItem(i, 0, new QTableWidgetItem(r.ruleId));
    resultsTable_->setItem(i, 1, new QTableWidgetItem(r.passed ? tr("Pass") : tr("Fail")));
    resultsTable_->setItem(i, 2, new QTableWidgetItem(r.details));
    resultsTable_->setItem(i, 3, new QTableWidgetItem(r.recommendation));
  }
  resultsTable_->scrollToBottom();
}

void ComplianceCheckerPlugin::updateScoreLabel() {
  scoreLabel_->setText(tr("Score: %1%").arg(score_, 0, 'f', 1));
}
