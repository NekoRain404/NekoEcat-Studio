#pragma once

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QPushButton;
class QTableWidget;

class CertificationRequirement {
public:
  QString id;
  QString category;
  QString description;
  bool mandatory = true;
};

class CertificationTestResult {
public:
  QString requirementId;
  QString status;
  QString evidence;
  QString notes;
};

class CertificationManagerPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit CertificationManagerPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  QVector<CertificationRequirement> requirements() const;
  void addRequirement(const CertificationRequirement &req);
  void clearRequirements();

  QVector<CertificationTestResult> testResults() const;
  void clearTestResults();

  int passedCount() const;
  int failedCount() const;
  bool allPassed() const;

  QTableWidget *requirementsTable() const;
  QTableWidget *resultsTable() const;
  QLabel *statusLabel() const;

signals:
  void certificationCompleted(bool passed);
  void requirementsChanged();

public slots:
  void runCertification();
  void testSelected();

private:
  void buildUi();
  void updateRequirementsTable();
  void updateResultsTable();
  void updateStatusPanel();

  QWidget *containerWidget_ = nullptr;

  QTableWidget *requirementsTable_ = nullptr;
  QTableWidget *resultsTable_ = nullptr;
  QLabel *statusLabel_ = nullptr;
  QLabel *passedCountLabel_ = nullptr;
  QLabel *failedCountLabel_ = nullptr;
  QPushButton *runAllBtn_ = nullptr;
  QPushButton *testSelectedBtn_ = nullptr;

  QVector<CertificationRequirement> requirements_;
  QVector<CertificationTestResult> testResults_;
};
