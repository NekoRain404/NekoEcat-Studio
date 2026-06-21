#include "CertificationManagerPlugin.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>

CertificationManagerPlugin::CertificationManagerPlugin(QObject *parent) {
  if (parent) setParent(parent);

  CertificationRequirement r1;
  r1.id = "R001";
  r1.category = tr("Safety");
  r1.description = tr("Emergency stop response time under 10ms");
  r1.mandatory = true;
  requirements_.append(r1);

  CertificationRequirement r2;
  r2.id = "R002";
  r2.category = tr("Performance");
  r2.description = tr("Jitter must not exceed 1us");
  r2.mandatory = true;
  requirements_.append(r2);

  CertificationRequirement r3;
  r3.id = "R003";
  r3.category = tr("Interoperability");
  r3.description = tr("Slave auto-discovery must succeed");
  r3.mandatory = false;
  requirements_.append(r3);

  buildUi();
}

QString CertificationManagerPlugin::id() const { return "certification"; }
QString CertificationManagerPlugin::displayName() const { return "Certification"; }
QString CertificationManagerPlugin::displayNameZh() const { return QStringLiteral("认证"); }
QIcon CertificationManagerPlugin::icon() const { return QIcon::fromTheme("emblem-checked"); }
int CertificationManagerPlugin::defaultOrder() const { return 214; }
bool CertificationManagerPlugin::visible() const { return true; }

void CertificationManagerPlugin::activate() {}
void CertificationManagerPlugin::deactivate() {}

QWidget *CertificationManagerPlugin::widget() { return containerWidget_; }

QVector<CertificationRequirement> CertificationManagerPlugin::requirements() const {
  return requirements_;
}
void CertificationManagerPlugin::addRequirement(const CertificationRequirement &req) {
  requirements_.append(req);
  updateRequirementsTable();
  emit requirementsChanged();
}
void CertificationManagerPlugin::clearRequirements() {
  requirements_.clear();
  updateRequirementsTable();
}

QVector<CertificationTestResult> CertificationManagerPlugin::testResults() const {
  return testResults_;
}
void CertificationManagerPlugin::clearTestResults() {
  testResults_.clear();
  updateResultsTable();
  updateStatusPanel();
}

int CertificationManagerPlugin::passedCount() const {
  int count = 0;
  for (const auto &r : testResults_) {
    if (r.status == tr("Pass")) ++count;
  }
  return count;
}

int CertificationManagerPlugin::failedCount() const {
  int count = 0;
  for (const auto &r : testResults_) {
    if (r.status == tr("Fail")) ++count;
  }
  return count;
}

bool CertificationManagerPlugin::allPassed() const {
  return !testResults_.isEmpty() && failedCount() == 0;
}

QTableWidget *CertificationManagerPlugin::requirementsTable() const { return requirementsTable_; }
QTableWidget *CertificationManagerPlugin::resultsTable() const { return resultsTable_; }
QLabel *CertificationManagerPlugin::statusLabel() const { return statusLabel_; }

void CertificationManagerPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  auto *toolbar = new QWidget;
  auto *toolbarLayout = new QHBoxLayout(toolbar);
  toolbarLayout->setContentsMargins(4, 2, 4, 2);

  runAllBtn_ = new QPushButton(tr("Run All"));
  testSelectedBtn_ = new QPushButton(tr("Test Selected"));

  toolbarLayout->addWidget(runAllBtn_);
  toolbarLayout->addWidget(testSelectedBtn_);

  statusLabel_ = new QLabel(tr("Status: Not Tested"));
  statusLabel_->setStyleSheet("font-weight: bold; padding: 0 8px;");
  toolbarLayout->addWidget(statusLabel_);

  passedCountLabel_ = new QLabel(tr("Passed: 0"));
  passedCountLabel_->setStyleSheet("color: green; padding: 0 8px;");
  toolbarLayout->addWidget(passedCountLabel_);

  failedCountLabel_ = new QLabel(tr("Failed: 0"));
  failedCountLabel_->setStyleSheet("color: red; padding: 0 8px;");
  toolbarLayout->addWidget(failedCountLabel_);

  toolbarLayout->addStretch();
  mainLayout->addWidget(toolbar);

  auto *splitter = new QSplitter(Qt::Horizontal);

  auto *leftPanel = new QWidget;
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(4, 4, 4, 4);
  leftLayout->addWidget(new QLabel(tr("Requirements")));
  requirementsTable_ = new QTableWidget;
  requirementsTable_->setColumnCount(4);
  requirementsTable_->setHorizontalHeaderLabels(
      {tr("ID"), tr("Category"), tr("Description"), tr("Mandatory")});
  requirementsTable_->horizontalHeader()->setStretchLastSection(true);
  requirementsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  requirementsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  updateRequirementsTable();
  leftLayout->addWidget(requirementsTable_);
  splitter->addWidget(leftPanel);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);
  rightLayout->setContentsMargins(4, 4, 4, 4);
  rightLayout->addWidget(new QLabel(tr("Test Results")));
  resultsTable_ = new QTableWidget;
  resultsTable_->setColumnCount(4);
  resultsTable_->setHorizontalHeaderLabels(
      {tr("Requirement ID"), tr("Status"), tr("Evidence"), tr("Notes")});
  resultsTable_->horizontalHeader()->setStretchLastSection(true);
  resultsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  resultsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  rightLayout->addWidget(resultsTable_);
  splitter->addWidget(rightPanel);

  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 2);
  mainLayout->addWidget(splitter);

  connect(runAllBtn_, &QPushButton::clicked, this, [this]() { runCertification(); });
  connect(testSelectedBtn_, &QPushButton::clicked, this, [this]() { testSelected(); });
}

void CertificationManagerPlugin::runCertification() {
  testResults_.clear();
  for (const auto &req : requirements_) {
    CertificationTestResult result;
    result.requirementId = req.id;
    result.status = (QRandomGenerator::global()->bounded(4) != 0) ? tr("Pass") : tr("Fail");
    result.evidence = result.status == tr("Pass") ? tr("Test passed") : tr("Test failed");
    result.notes = result.status == tr("Pass") ? tr("Compliant") : tr("Requires review");
    testResults_.append(result);
  }
  updateResultsTable();
  updateStatusPanel();
  emit certificationCompleted(allPassed());
}

void CertificationManagerPlugin::testSelected() {
  auto selected = requirementsTable_->selectionModel()->selectedRows();
  if (selected.isEmpty()) return;

  for (const auto &index : selected) {
    int row = index.row();
    if (row < 0 || row >= requirements_.size()) continue;
    const auto &req = requirements_[row];

    int existing = -1;
    for (int i = 0; i < testResults_.size(); ++i) {
      if (testResults_[i].requirementId == req.id) {
        existing = i;
        break;
      }
    }

    CertificationTestResult result;
    result.requirementId = req.id;
    result.status = (QRandomGenerator::global()->bounded(3) != 0) ? tr("Pass") : tr("Fail");
    result.evidence = result.status == tr("Pass") ? tr("Test passed") : tr("Test failed");
    result.notes = result.status == tr("Pass") ? tr("Compliant") : tr("Requires review");

    if (existing >= 0) {
      testResults_[existing] = result;
    } else {
      testResults_.append(result);
    }
  }
  updateResultsTable();
  updateStatusPanel();
  emit certificationCompleted(allPassed());
}

void CertificationManagerPlugin::updateRequirementsTable() {
  requirementsTable_->setRowCount(requirements_.size());
  for (int i = 0; i < requirements_.size(); ++i) {
    const auto &r = requirements_[i];
    requirementsTable_->setItem(i, 0, new QTableWidgetItem(r.id));
    requirementsTable_->setItem(i, 1, new QTableWidgetItem(r.category));
    requirementsTable_->setItem(i, 2, new QTableWidgetItem(r.description));
    requirementsTable_->setItem(i, 3, new QTableWidgetItem(r.mandatory ? tr("Yes") : tr("No")));
  }
}

void CertificationManagerPlugin::updateResultsTable() {
  resultsTable_->setRowCount(testResults_.size());
  for (int i = 0; i < testResults_.size(); ++i) {
    const auto &r = testResults_[i];
    resultsTable_->setItem(i, 0, new QTableWidgetItem(r.requirementId));
    resultsTable_->setItem(i, 1, new QTableWidgetItem(r.status));
    resultsTable_->setItem(i, 2, new QTableWidgetItem(r.evidence));
    resultsTable_->setItem(i, 3, new QTableWidgetItem(r.notes));
  }
  resultsTable_->scrollToBottom();
}

void CertificationManagerPlugin::updateStatusPanel() {
  int passed = passedCount();
  int failed = failedCount();
  passedCountLabel_->setText(tr("Passed: %1").arg(passed));
  failedCountLabel_->setText(tr("Failed: %1").arg(failed));

  if (testResults_.isEmpty()) {
    statusLabel_->setText(tr("Status: Not Tested"));
  } else if (failed == 0) {
    statusLabel_->setText(tr("Status: PASS"));
  } else {
    statusLabel_->setText(tr("Status: FAIL"));
  }
}
