#include "SecurityManagerPlugin.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>
#include <QTime>
#include <QVBoxLayout>

SecurityManagerPlugin::SecurityManagerPlugin(QObject *parent) {
  if (parent) setParent(parent);
  policy_.level = tr("Standard");
  policy_.encryption = true;
  policy_.authentication = true;
  policy_.acl = tr("Default");
  buildUi();
}

QString SecurityManagerPlugin::id() const { return "security"; }
QString SecurityManagerPlugin::displayName() const { return "Security"; }
QString SecurityManagerPlugin::displayNameZh() const { return QStringLiteral("安全"); }
QIcon SecurityManagerPlugin::icon() const { return QIcon::fromTheme("security-high"); }
int SecurityManagerPlugin::defaultOrder() const { return 210; }
bool SecurityManagerPlugin::visible() const { return true; }

void SecurityManagerPlugin::activate() {}
void SecurityManagerPlugin::deactivate() {}

QWidget *SecurityManagerPlugin::widget() { return containerWidget_; }

SecurityManagerPlugin::AuditState SecurityManagerPlugin::auditState() const { return auditState_; }
bool SecurityManagerPlugin::isAuditing() const { return auditState_ == AuditState::Running; }

SecurityPolicy SecurityManagerPlugin::currentPolicy() const { return policy_; }
void SecurityManagerPlugin::setPolicy(const SecurityPolicy &policy) {
  policy_ = policy;
  populatePolicyTable();
  emit policyChanged(policy_);
}

QVector<SecurityAuditResult> SecurityManagerPlugin::auditResults() const { return auditResults_; }
void SecurityManagerPlugin::clearAuditResults() {
  auditResults_.clear();
  auditResultsTable_->setRowCount(0);
}

QTableWidget *SecurityManagerPlugin::policyTable() const { return policyTable_; }
QTableWidget *SecurityManagerPlugin::auditResultsTable() const { return auditResultsTable_; }
QTextEdit *SecurityManagerPlugin::auditLog() const { return auditLog_; }

void SecurityManagerPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  auto *toolbar = new QWidget;
  auto *toolbarLayout = new QHBoxLayout(toolbar);
  toolbarLayout->setContentsMargins(4, 2, 4, 2);

  runAuditBtn_ = new QPushButton(tr("Run Audit"));
  applyPolicyBtn_ = new QPushButton(tr("Apply Policy"));

  toolbarLayout->addWidget(runAuditBtn_);
  toolbarLayout->addWidget(applyPolicyBtn_);

  auditStateLabel_ = new QLabel(tr("Idle"));
  auditStateLabel_->setStyleSheet("font-weight: bold; padding: 0 8px;");
  toolbarLayout->addWidget(auditStateLabel_);
  toolbarLayout->addStretch();
  mainLayout->addWidget(toolbar);

  auto *splitter = new QSplitter(Qt::Horizontal);

  auto *leftPanel = new QWidget;
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(4, 4, 4, 4);
  leftLayout->addWidget(new QLabel(tr("Security Policy")));
  policyTable_ = new QTableWidget;
  policyTable_->setColumnCount(2);
  policyTable_->setHorizontalHeaderLabels({tr("Setting"), tr("Value")});
  policyTable_->horizontalHeader()->setStretchLastSection(true);
  policyTable_->setRowCount(4);
  policyTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  populatePolicyTable();
  leftLayout->addWidget(policyTable_);
  splitter->addWidget(leftPanel);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);
  rightLayout->setContentsMargins(4, 4, 4, 4);
  rightLayout->addWidget(new QLabel(tr("Audit Results")));
  auditResultsTable_ = new QTableWidget;
  auditResultsTable_->setColumnCount(5);
  auditResultsTable_->setHorizontalHeaderLabels(
      {tr("Time"), tr("Event"), tr("User"), tr("Description"), tr("Severity")});
  auditResultsTable_->horizontalHeader()->setStretchLastSection(true);
  auditResultsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  auditResultsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  rightLayout->addWidget(auditResultsTable_);
  splitter->addWidget(rightPanel);

  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 2);
  mainLayout->addWidget(splitter);

  auto *logPanel = new QWidget;
  auto *logLayout = new QVBoxLayout(logPanel);
  logLayout->setContentsMargins(4, 4, 4, 4);
  logLayout->addWidget(new QLabel(tr("Audit Log")));
  auditLog_ = new QTextEdit;
  auditLog_->setReadOnly(true);
  auditLog_->setMaximumHeight(150);
  logLayout->addWidget(auditLog_);
  mainLayout->addWidget(logPanel);

  connect(runAuditBtn_, &QPushButton::clicked, this, [this]() { runAudit(); });
  connect(applyPolicyBtn_, &QPushButton::clicked, this, [this]() { applyPolicy(); });
}

void SecurityManagerPlugin::populatePolicyTable() {
  policyTable_->setItem(0, 0, new QTableWidgetItem(tr("Security Level")));
  policyTable_->setItem(0, 1, new QTableWidgetItem(policy_.level));
  policyTable_->setItem(1, 0, new QTableWidgetItem(tr("Encryption")));
  policyTable_->setItem(1, 1, new QTableWidgetItem(policy_.encryption ? tr("Enabled") : tr("Disabled")));
  policyTable_->setItem(2, 0, new QTableWidgetItem(tr("Authentication")));
  policyTable_->setItem(2, 1, new QTableWidgetItem(policy_.authentication ? tr("Enabled") : tr("Disabled")));
  policyTable_->setItem(3, 0, new QTableWidgetItem(tr("Access Control")));
  policyTable_->setItem(3, 1, new QTableWidgetItem(policy_.acl));
}

void SecurityManagerPlugin::runAudit() {
  auditState_ = AuditState::Running;
  auditStateLabel_->setText(tr("Running"));
  runAuditBtn_->setEnabled(false);
  emit auditStateChanged(auditState_);

  SecurityAuditResult result;
  result.time = QTime::currentTime().toString("HH:mm:ss");
  result.event = tr("Access Check");
  result.user = tr("system");
  result.description = tr("Routine security audit completed");
  result.severity = tr("Info");
  auditResults_.append(result);
  updateAuditResultsTable();

  auditLog_->append(QStringLiteral("[%1] %2")
                        .arg(result.time)
                        .arg(tr("Audit completed: %1 events recorded").arg(auditResults_.size())));

  auditState_ = AuditState::Completed;
  auditStateLabel_->setText(tr("Completed"));
  runAuditBtn_->setEnabled(true);
  emit auditStateChanged(auditState_);
  emit auditCompleted();
}

void SecurityManagerPlugin::applyPolicy() {
  auditLog_->append(QStringLiteral("[%1] %2")
                        .arg(QTime::currentTime().toString("HH:mm:ss"))
                        .arg(tr("Policy applied: Level=%1, Encryption=%2, Auth=%3, ACL=%4")
                                 .arg(policy_.level)
                                 .arg(policy_.encryption ? tr("On") : tr("Off"))
                                 .arg(policy_.authentication ? tr("On") : tr("Off"))
                                 .arg(policy_.acl)));
  emit policyChanged(policy_);
}

void SecurityManagerPlugin::updateAuditResultsTable() {
  auditResultsTable_->setRowCount(auditResults_.size());
  for (int i = 0; i < auditResults_.size(); ++i) {
    const auto &r = auditResults_[i];
    auditResultsTable_->setItem(i, 0, new QTableWidgetItem(r.time));
    auditResultsTable_->setItem(i, 1, new QTableWidgetItem(r.event));
    auditResultsTable_->setItem(i, 2, new QTableWidgetItem(r.user));
    auditResultsTable_->setItem(i, 3, new QTableWidgetItem(r.description));
    auditResultsTable_->setItem(i, 4, new QTableWidgetItem(r.severity));
  }
  auditResultsTable_->scrollToBottom();
}
