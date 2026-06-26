#pragma once

#include "plugins/WorkspacePlugin.h"

#include <QDateTime>
#include <QVector>

class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class QSplitter;
class QTabWidget;
class QTableWidget;
class QTextEdit;

class ComplianceCheckerPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit ComplianceCheckerPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  struct ComplianceCheck {
    QString id;
    QString name;
    QString category;
    QString description;
    bool passed;
    QDateTime checkedAt;
  };

  struct Violation {
    QString id;
    QString checkId;
    QString severity;
    QString description;
    QString recommendation;
    QDateTime detectedAt;
  };

  struct Recommendation {
    QString id;
    QString priority;
    QString title;
    QString description;
    QString category;
  };

  struct ComplianceReport {
    QDateTime generatedAt;
    int totalChecks;
    int passedChecks;
    int failedChecks;
    double score;
    QVector<Violation> violations;
  };

  void addCheck(const ComplianceCheck &check);
  void removeCheck(int index);
  void runCheck(int index);
  int checkCount() const;

  void addViolation(const Violation &violation);
  void removeViolation(int index);
  int violationCount() const;

  void addRecommendation(const Recommendation &rec);
  void removeRecommendation(int index);
  int recommendationCount() const;

  double complianceScore() const;
  ComplianceReport generateReport() const;
  bool exportReport(const QString &path);

  QTableWidget *checkTable() const;
  QTableWidget *violationTable() const;
  QTableWidget *recommendationTable() const;
  QLabel *statusLabel() const;
  QLabel *scoreLabel() const;

signals:
  void checkCompleted(int index, bool passed);
  void violationDetected(const QString &violationId);
  void scoreChanged(double score);

private:
  void buildUi();
  void rebuildCheckTable();
  void rebuildViolationTable();
  void rebuildRecommendationTable();
  void updateScore();

  QWidget *containerWidget_ = nullptr;
  QTabWidget *tabs_ = nullptr;

  QTableWidget *checkTable_ = nullptr;
  QComboBox *categoryFilter_ = nullptr;
  QPushButton *runAllBtn_ = nullptr;
  QPushButton *runSelectedBtn_ = nullptr;

  QTableWidget *violationTable_ = nullptr;
  QComboBox *severityFilter_ = nullptr;
  QPushButton *removeViolationBtn_ = nullptr;

  QTableWidget *recommendationTable_ = nullptr;
  QComboBox *priorityFilter_ = nullptr;
  QPushButton *addRecBtn_ = nullptr;
  QPushButton *removeRecBtn_ = nullptr;

  QPushButton *exportReportBtn_ = nullptr;
  QLabel *statusLabel_ = nullptr;
  QLabel *scoreLabel_ = nullptr;

  QVector<ComplianceCheck> checks_;
  QVector<Violation> violations_;
  QVector<Recommendation> recommendations_;
};
