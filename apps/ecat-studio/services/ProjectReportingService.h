#pragma once

// ProjectReportingService -- manages project reporting for EtherCAT projects.
//
// Supports generating status reports, progress reports, performance reports,
// and financial reports with charts, tables, and recommendations.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QVector>
#include <QHash>
#include <QDateTime>

enum class ProjectReportFormat { Text, Html, Pdf, Json };

struct ProjectReportSection {
  QString title;
  QString content;
  QStringList items;
};

struct ProjectReportChart {
  QString title;
  QString type;
  QHash<QString, double> data;
};

struct ProjectReportTable {
  QString title;
  QStringList headers;
  QVector<QStringList> rows;
};

struct ProjectDocReport {
  QString title;
  QString summary;
  QVector<ProjectReportSection> sections;
  QVector<ProjectReportChart> charts;
  QVector<ProjectReportTable> tables;
  QStringList recommendations;
  ProjectReportFormat format = ProjectReportFormat::Text;
  QDateTime timestamp;
};

struct ReportProjectData {
  int projectId = 0;
  QString projectName;
  int completedTasks = 0;
  int totalTasks = 0;
  double totalCost = 0.0;
  double budgetedCost = 0.0;
  double totalHours = 0.0;
  double budgetedHours = 0.0;
  double qualityScore = 0.0;
  QVector<QString> statusNotes;
};

class ProjectReportingService : public QObject {
  Q_OBJECT
public:
  explicit ProjectReportingService(QObject *parent = nullptr);

  bool addProject(const ReportProjectData &data);
  ProjectDocReport generateStatusReport(int projectId);
  ProjectDocReport generateProgressReport(int projectId);
  ProjectDocReport generatePerformanceReport(int projectId);
  ProjectDocReport generateFinancialReport(int projectId);

  QVector<ProjectDocReport> allReports() const;
  int reportCount() const;

signals:
  void reportGenerated(const ProjectDocReport &report);

private:
  ProjectDocReport buildBaseReport(const ReportProjectData &d, const QString &type);
  QHash<int, ReportProjectData> projects_;
  QVector<ProjectDocReport> reports_;
  static constexpr int kMaxReports = 1000;
};
