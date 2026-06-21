#pragma once

// ProjectTrackingService -- manages project tracking for EtherCAT projects.
//
// Supports progress tracking, time tracking, cost tracking,
// and quality tracking with detailed reports.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QVector>
#include <QHash>
#include <QDateTime>

enum class ProjectTrackStatus { OnTrack, AtRisk, Behind, Ahead, Completed };

struct ProgressStatus {
  int projectId = 0;
  int progress = 0;
  int completedTasks = 0;
  int totalTasks = 0;
  double percentage = 0.0;
  ProjectTrackStatus status = ProjectTrackStatus::OnTrack;
  QDateTime lastUpdated;
};

struct TimeEntry {
  int taskId = 0;
  QString description;
  double hours = 0.0;
  QDate date;
  QString assignee;
};

struct TimeReport {
  int projectId = 0;
  double totalHours = 0.0;
  double budgetedHours = 0.0;
  double remainingHours = 0.0;
  QVector<TimeEntry> entries;
  QDateTime generatedAt;
};

struct CostEntry {
  QString category;
  double amount = 0.0;
  QDate date;
  QString description;
};

struct CostReport {
  int projectId = 0;
  double totalCost = 0.0;
  double budgetedCost = 0.0;
  double remainingBudget = 0.0;
  QVector<CostEntry> entries;
  QDateTime generatedAt;
};

struct QualityMetric {
  QString name;
  double value = 0.0;
  double target = 0.0;
  bool meetsTarget = false;
};

struct QualityReport {
  int projectId = 0;
  double overallScore = 0.0;
  QVector<QualityMetric> metrics;
  QDateTime generatedAt;
};

struct ProjectTrackData {
  int projectId = 0;
  int completedTasks = 0;
  int totalTasks = 0;
  double budgetedHours = 0.0;
  double budgetedCost = 0.0;
  QVector<TimeEntry> timeEntries;
  QVector<CostEntry> costEntries;
  QVector<QualityMetric> qualityMetrics;
};

class ProjectTrackingService : public QObject {
  Q_OBJECT
public:
  explicit ProjectTrackingService(QObject *parent = nullptr);

  bool addProject(const ProjectTrackData &data);
  ProgressStatus trackProgress(int projectId) const;
  TimeReport trackTime(int projectId) const;
  CostReport trackCost(int projectId) const;
  QualityReport trackQuality(int projectId) const;

  bool logTime(int projectId, const TimeEntry &entry);
  bool logCost(int projectId, const CostEntry &entry);
  bool updateTaskCompletion(int projectId, int completedTasks);
  bool updateQualityMetric(int projectId, const QualityMetric &metric);

  int projectCount() const;

signals:
  void progressUpdated(const ProgressStatus &status);
  void timeUpdated(const TimeReport &report);

private:
  QHash<int, ProjectTrackData> projects_;
  static constexpr int kMaxProjects = 5000;
};
