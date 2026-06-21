#pragma once

// ProjectPlanningService -- manages project planning for EtherCAT projects.
//
// Supports milestone planning, timeline planning, resource planning,
// and risk planning with assessments and reports.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QVector>
#include <QHash>
#include <QDateTime>

enum class MilestonePriority { Low, Medium, High, Critical };
enum class RiskLevel { Low, Medium, High, Critical };

struct MilestoneConfig {
  QString name;
  QString description;
  QDateTime deadline;
  QStringList deliverables;
  QStringList criteria;
  QStringList dependencies;
  MilestonePriority priority = MilestonePriority::Medium;
};

struct PlannedMilestone {
  int id = 0;
  QString name;
  QString description;
  QDateTime deadline;
  QStringList deliverables;
  QStringList criteria;
  QStringList dependencies;
  MilestonePriority priority = MilestonePriority::Medium;
  bool completed = false;
  QDateTime createdAt;
};

struct TimelineConfig {
  QString name;
  QDateTime startDate;
  QDateTime endDate;
  QVector<int> milestoneIds;
  QString description;
};

struct Timeline {
  int id = 0;
  QString name;
  QDateTime startDate;
  QDateTime endDate;
  QVector<int> milestoneIds;
  QString description;
  QDateTime createdAt;
};

struct ResourcePlan {
  int projectId = 0;
  QVector<QString> resourceNames;
  QVector<int> allocationPercent;
  QDateTime startDate;
  QDateTime endDate;
};

struct RiskItem {
  QString description;
  RiskLevel level = RiskLevel::Medium;
  double probability = 0.0;
  double impact = 0.0;
  QString mitigation;
};

struct RiskAssessment {
  int projectId = 0;
  QVector<RiskItem> risks;
  QString notes;
};

struct RiskReport {
  int projectId = 0;
  QVector<RiskItem> risks;
  double overallRiskScore = 0.0;
  int highRiskCount = 0;
  int mediumRiskCount = 0;
  int lowRiskCount = 0;
  QDateTime generatedAt;
};

class ProjectPlanningService : public QObject {
  Q_OBJECT
public:
  explicit ProjectPlanningService(QObject *parent = nullptr);

  PlannedMilestone createMilestone(const MilestoneConfig &config);
  Timeline createTimeline(const TimelineConfig &config);
  bool planResources(const ResourcePlan &plan);
  RiskReport assessRisks(const RiskAssessment &assessment);

  PlannedMilestone milestone(int milestoneId) const;
  QVector<PlannedMilestone> allMilestones() const;
  Timeline timeline(int timelineId) const;
  QVector<Timeline> allTimelines() const;
  int milestoneCount() const;
  int timelineCount() const;

  bool completeMilestone(int milestoneId);
  bool updateMilestonePriority(int milestoneId, MilestonePriority priority);

signals:
  void milestoneCreated(const PlannedMilestone &milestone);
  void timelineCreated(const Timeline &timeline);

private:
  QHash<int, PlannedMilestone> milestones_;
  QHash<int, Timeline> timelines_;
  QHash<int, ResourcePlan> resourcePlans_;
  int nextMilestoneId_ = 1;
  int nextTimelineId_ = 1;
  static constexpr int kMaxMilestones = 5000;
};
