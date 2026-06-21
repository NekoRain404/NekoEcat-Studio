#include "ComplianceCheckerPlugin.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QSplitter>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

ComplianceCheckerPlugin::ComplianceCheckerPlugin(QObject *parent) {
  if (parent) setParent(parent);
  auto now = QDateTime::currentDateTime();
  checks_ = {
      {"c1", "EtherCAT Cable Redundancy", "Network",
       "Verify cable redundancy configuration", true, now},
      {"c2", "Distributed Clock Sync", "Timing",
       "Check DC synchronization accuracy", true, now},
      {"c3", "PDO Mapping Validation", "Communication",
       "Validate PDO mapping consistency", false, now},
      {"c4", "Slave State Machine Compliance", "Safety",
       "Verify slaves reach OP state", true, now},
      {"c5", "Watchdog Configuration", "Safety",
       "Check watchdog timeout settings", false, now},
      {"c6", "Firmware Version Check", "Maintenance",
       "Verify firmware versions match requirements", true, now},
  };
  violations_ = {
      {"v1", "c3", "warning", "PDO mapping mismatch on slave 2",
       "Update PDO mapping to match ESI file", now},
      {"v2", "c5", "critical", "Watchdog timeout too long (>100ms)",
       "Reduce watchdog timeout to 50ms", now},
  };
  recommendations_ = {
      {"r1", "high", "Enable Cable Redundancy",
       "Configure A/B cable paths for fault tolerance", "Network"},
      {"r2", "medium", "Update Firmware",
       "Upgrade all slaves to latest stable firmware", "Maintenance"},
      {"r3", "low", "Document Configuration",
       "Create configuration documentation for audit trail", "Compliance"},
  };
  buildUi();
}

QString ComplianceCheckerPlugin::id() const { return "compliancechecker"; }
QString ComplianceCheckerPlugin::displayName() const {
  return "Compliance Checker";
}
QString ComplianceCheckerPlugin::displayNameZh() const { return "合规检查器"; }
int ComplianceCheckerPlugin::defaultOrder() const { return 285; }
bool ComplianceCheckerPlugin::visible() const { return true; }

void ComplianceCheckerPlugin::activate() {}
void ComplianceCheckerPlugin::deactivate() {}

QWidget *ComplianceCheckerPlugin::widget() {
  if (!containerWidget_) buildUi();
  return containerWidget_;
}

void ComplianceCheckerPlugin::addCheck(const ComplianceCheck &check) {
  checks_.append(check);
  rebuildCheckTable();
}

void ComplianceCheckerPlugin::removeCheck(int index) {
  if (index >= 0 && index < checks_.size()) {
    checks_.removeAt(index);
    rebuildCheckTable();
  }
}

void ComplianceCheckerPlugin::runCheck(int index) {
  if (index >= 0 && index < checks_.size()) {
    checks_[index].checkedAt = QDateTime::currentDateTime();
    emit checkCompleted(index, checks_[index].passed);
    rebuildCheckTable();
    updateScore();
  }
}

int ComplianceCheckerPlugin::checkCount() const { return checks_.size(); }

void ComplianceCheckerPlugin::addViolation(const Violation &violation) {
  violations_.append(violation);
  rebuildViolationTable();
  emit violationDetected(violation.id);
  updateScore();
}

void ComplianceCheckerPlugin::removeViolation(int index) {
  if (index >= 0 && index < violations_.size()) {
    violations_.removeAt(index);
    rebuildViolationTable();
    updateScore();
  }
}

int ComplianceCheckerPlugin::violationCount() const {
  return violations_.size();
}

void ComplianceCheckerPlugin::addRecommendation(const Recommendation &rec) {
  recommendations_.append(rec);
  rebuildRecommendationTable();
}

void ComplianceCheckerPlugin::removeRecommendation(int index) {
  if (index >= 0 && index < recommendations_.size()) {
    recommendations_.removeAt(index);
    rebuildRecommendationTable();
  }
}

int ComplianceCheckerPlugin::recommendationCount() const {
  return recommendations_.size();
}

double ComplianceCheckerPlugin::complianceScore() const {
  if (checks_.isEmpty()) return 100.0;
  int passed = 0;
  for (const auto &c : checks_) {
    if (c.passed) ++passed;
  }
  return (100.0 * passed) / checks_.size();
}

ComplianceCheckerPlugin::ComplianceReport
ComplianceCheckerPlugin::generateReport() const {
  ComplianceReport report;
  report.generatedAt = QDateTime::currentDateTime();
  report.totalChecks = checks_.size();
  report.passedChecks = 0;
  report.failedChecks = 0;
  for (const auto &c : checks_) {
    if (c.passed)
      ++report.passedChecks;
    else
      ++report.failedChecks;
  }
  report.score = complianceScore();
  report.violations = violations_;
  return report;
}

void ComplianceCheckerPlugin::exportReport(const QString &path) {
  QFile f(path);
  if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&f);
    auto report = generateReport();
    out << "Compliance Report\n";
    out << "=================\n\n";
    out << "Score: " << QString::number(report.score, 'f', 1) << "%\n";
    out << "Total Checks: " << report.totalChecks << "\n";
    out << "Passed: " << report.passedChecks << "\n";
    out << "Failed: " << report.failedChecks << "\n\n";
    out << "--- Violations ---\n";
    for (const auto &v : violations_) {
      out << "[" << v.severity << "] " << v.description << "\n";
      out << "  Recommendation: " << v.recommendation << "\n";
    }
    out << "\n--- Recommendations ---\n";
    for (const auto &r : recommendations_) {
      out << "[" << r.priority << "] " << r.title << ": " << r.description
          << "\n";
    }
  }
}

QTableWidget *ComplianceCheckerPlugin::checkTable() const {
  return checkTable_;
}
QTableWidget *ComplianceCheckerPlugin::violationTable() const {
  return violationTable_;
}
QTableWidget *ComplianceCheckerPlugin::recommendationTable() const {
  return recommendationTable_;
}
QLabel *ComplianceCheckerPlugin::statusLabel() const { return statusLabel_; }
QLabel *ComplianceCheckerPlugin::scoreLabel() const { return scoreLabel_; }

void ComplianceCheckerPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);

  auto *scoreRow = new QWidget;
  auto *scoreLayout = new QHBoxLayout(scoreRow);
  scoreLabel_ = new QLabel("Compliance Score: 100%");
  scoreLayout->addWidget(scoreLabel_);
  mainLayout->addWidget(scoreRow);

  tabs_ = new QTabWidget;

  auto *checkTab = new QWidget;
  auto *checkLayout = new QVBoxLayout(checkTab);
  auto *checkFilterRow = new QWidget;
  auto *checkFilterLayout = new QHBoxLayout(checkFilterRow);
  categoryFilter_ = new QComboBox;
  categoryFilter_->addItems({"All", "Network", "Timing", "Communication",
                              "Safety", "Maintenance"});
  checkFilterLayout->addWidget(categoryFilter_);
  runAllBtn_ = new QPushButton("Run All");
  runSelectedBtn_ = new QPushButton("Run Selected");
  checkFilterLayout->addWidget(runAllBtn_);
  checkFilterLayout->addWidget(runSelectedBtn_);
  checkLayout->addWidget(checkFilterRow);

  checkTable_ = new QTableWidget;
  checkTable_->setColumnCount(5);
  checkTable_->setHorizontalHeaderLabels(
      {"ID", "Name", "Category", "Status", "Last Checked"});
  checkLayout->addWidget(checkTable_);
  tabs_->addTab(checkTab, "Checks");

  auto *violationTab = new QWidget;
  auto *violationLayout = new QVBoxLayout(violationTab);
  auto *violationFilterRow = new QWidget;
  auto *violationFilterLayout = new QHBoxLayout(violationFilterRow);
  severityFilter_ = new QComboBox;
  severityFilter_->addItems({"All", "critical", "warning", "info"});
  violationFilterLayout->addWidget(severityFilter_);
  removeViolationBtn_ = new QPushButton("Dismiss");
  violationFilterLayout->addWidget(removeViolationBtn_);
  violationLayout->addWidget(violationFilterRow);

  violationTable_ = new QTableWidget;
  violationTable_->setColumnCount(5);
  violationTable_->setHorizontalHeaderLabels(
      {"ID", "Severity", "Description", "Recommendation", "Detected"});
  violationLayout->addWidget(violationTable_);
  tabs_->addTab(violationTab, "Violations");

  auto *recTab = new QWidget;
  auto *recLayout = new QVBoxLayout(recTab);
  auto *recFilterRow = new QWidget;
  auto *recFilterLayout = new QHBoxLayout(recFilterRow);
  priorityFilter_ = new QComboBox;
  priorityFilter_->addItems({"All", "high", "medium", "low"});
  recFilterLayout->addWidget(priorityFilter_);
  addRecBtn_ = new QPushButton("Add");
  removeRecBtn_ = new QPushButton("Remove");
  recFilterLayout->addWidget(addRecBtn_);
  recFilterLayout->addWidget(removeRecBtn_);
  recLayout->addWidget(recFilterRow);

  recommendationTable_ = new QTableWidget;
  recommendationTable_->setColumnCount(4);
  recommendationTable_->setHorizontalHeaderLabels(
      {"ID", "Priority", "Title", "Description"});
  recLayout->addWidget(recommendationTable_);
  tabs_->addTab(recTab, "Recommendations");

  mainLayout->addWidget(tabs_);

  auto *bottomRow = new QWidget;
  auto *bottomLayout = new QHBoxLayout(bottomRow);
  exportReportBtn_ = new QPushButton("Export Report");
  bottomLayout->addWidget(exportReportBtn_);
  statusLabel_ = new QLabel("Ready");
  bottomLayout->addWidget(statusLabel_);
  mainLayout->addWidget(bottomRow);

  rebuildCheckTable();
  rebuildViolationTable();
  rebuildRecommendationTable();
  updateScore();

  connect(runAllBtn_, &QPushButton::clicked, this, [this]() {
    for (int i = 0; i < checks_.size(); ++i) runCheck(i);
  });
  connect(runSelectedBtn_, &QPushButton::clicked, this, [this]() {
    int row = checkTable_->currentRow();
    if (row >= 0) runCheck(row);
  });
  connect(removeViolationBtn_, &QPushButton::clicked, this, [this]() {
    int row = violationTable_->currentRow();
    if (row >= 0) removeViolation(row);
  });
  connect(addRecBtn_, &QPushButton::clicked, this, [this]() {
    Recommendation r;
    r.id = "r" + QString::number(recommendations_.size() + 1);
    r.priority = "medium";
    r.title = "New Recommendation";
    r.description = "";
    r.category = "General";
    addRecommendation(r);
  });
  connect(removeRecBtn_, &QPushButton::clicked, this, [this]() {
    int row = recommendationTable_->currentRow();
    if (row >= 0) removeRecommendation(row);
  });
  connect(exportReportBtn_, &QPushButton::clicked, this, [this]() {
    exportReport("/tmp/compliance_report.txt");
  });
}

void ComplianceCheckerPlugin::rebuildCheckTable() {
  if (!checkTable_) return;
  checkTable_->setRowCount(checks_.size());
  for (int i = 0; i < checks_.size(); ++i) {
    const auto &c = checks_[i];
    checkTable_->setItem(i, 0, new QTableWidgetItem(c.id));
    checkTable_->setItem(i, 1, new QTableWidgetItem(c.name));
    checkTable_->setItem(i, 2, new QTableWidgetItem(c.category));
    checkTable_->setItem(i, 3,
                        new QTableWidgetItem(c.passed ? "PASS" : "FAIL"));
    checkTable_->setItem(
        i, 4, new QTableWidgetItem(c.checkedAt.toString(Qt::ISODate)));
  }
}

void ComplianceCheckerPlugin::rebuildViolationTable() {
  if (!violationTable_) return;
  violationTable_->setRowCount(violations_.size());
  for (int i = 0; i < violations_.size(); ++i) {
    const auto &v = violations_[i];
    violationTable_->setItem(i, 0, new QTableWidgetItem(v.id));
    violationTable_->setItem(i, 1, new QTableWidgetItem(v.severity));
    violationTable_->setItem(i, 2, new QTableWidgetItem(v.description));
    violationTable_->setItem(i, 3, new QTableWidgetItem(v.recommendation));
    violationTable_->setItem(
        i, 4, new QTableWidgetItem(v.detectedAt.toString(Qt::ISODate)));
  }
}

void ComplianceCheckerPlugin::rebuildRecommendationTable() {
  if (!recommendationTable_) return;
  recommendationTable_->setRowCount(recommendations_.size());
  for (int i = 0; i < recommendations_.size(); ++i) {
    const auto &r = recommendations_[i];
    recommendationTable_->setItem(i, 0, new QTableWidgetItem(r.id));
    recommendationTable_->setItem(i, 1, new QTableWidgetItem(r.priority));
    recommendationTable_->setItem(i, 2, new QTableWidgetItem(r.title));
    recommendationTable_->setItem(i, 3, new QTableWidgetItem(r.description));
  }
}

void ComplianceCheckerPlugin::updateScore() {
  double score = complianceScore();
  if (scoreLabel_)
    scoreLabel_->setText(
        QString("Compliance Score: %1%").arg(QString::number(score, 'f', 1)));
  if (statusLabel_)
    statusLabel_->setText(
        QString("Checks: %1 | Violations: %2 | Score: %3%")
            .arg(checks_.size())
            .arg(violations_.size())
            .arg(QString::number(score, 'f', 1)));
  emit scoreChanged(score);
}
