#pragma once

// ProjectManagementService -- manages EtherCAT projects.
//
// Supports creating, tracking, reporting, and collaborating on projects
// with milestones, deliverables, and resource planning.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QVector>
#include <QHash>
#include <QDateTime>

enum class ProjectStatus { NotStarted, InProgress, OnHold, Completed, Cancelled };
enum class MilestoneStatus { Pending, InProgress, Completed, Delayed };

struct Milestone {
  int id = 0;
  QString name;
  QString description;
  QDateTime dueDate;
  MilestoneStatus status = MilestoneStatus::Pending;
  bool completed = false;
};

struct Deliverable {
  int id = 0;
  QString name;
  QString description;
  QDateTime deadline;
  bool delivered = false;
};

struct ResourceAllocation {
  QString resourceName;
  int allocationPercent = 0;
  double costPerHour = 0.0;
};

struct ProjectConfig {
  QString name;
  QString description;
  QString scope;
  QDateTime startDate;
  QDateTime endDate;
  QVector<ResourceAllocation> resources;
  QVector<Milestone> milestones;
  QVector<Deliverable> deliverables;
};

struct Project {
  int id = 0;
  QString name;
  QString description;
  QString scope;
  QDateTime startDate;
  QDateTime endDate;
  ProjectStatus status = ProjectStatus::NotStarted;
  QVector<ResourceAllocation> resources;
  QVector<Milestone> milestones;
  QVector<Deliverable> deliverables;
  QDateTime createdAt;
  QDateTime updatedAt;
};

struct ProjectStatusInfo {
  int projectId = 0;
  ProjectStatus status = ProjectStatus::NotStarted;
  int completionPercent = 0;
  int completedMilestones = 0;
  int totalMilestones = 0;
  int deliveredItems = 0;
  int totalDeliverables = 0;
  QDateTime lastUpdated;
};

struct CollaborationEntry {
  QString user;
  QString action;
  QDateTime timestamp;
  QString details;
};

struct ProjectReport {
  int projectId = 0;
  QString projectName;
  ProjectStatus status = ProjectStatus::NotStarted;
  int completionPercent = 0;
  QVector<Milestone> milestones;
  QVector<Deliverable> deliverables;
  QVector<CollaborationEntry> recentActivity;
  QDateTime generatedAt;
};

class ProjectManagementService : public QObject {
  Q_OBJECT
public:
  explicit ProjectManagementService(QObject *parent = nullptr);

  Project createProject(const ProjectConfig &config);
  ProjectStatusInfo trackProject(int projectId) const;
  ProjectReport generateReport(int projectId) const;
  bool collaborate(int projectId, const CollaborationEntry &collab);

  bool updateProjectStatus(int projectId, ProjectStatus status);
  bool addMilestone(int projectId, const Milestone &milestone);
  bool completeMilestone(int projectId, int milestoneId);
  bool deliverItem(int projectId, int deliverableId);

  Project project(int projectId) const;
  QVector<Project> allProjects() const;
  int projectCount() const;
  QVector<CollaborationEntry> projectHistory(int projectId) const;

signals:
  void projectCreated(const Project &project);
  void projectUpdated(const ProjectStatusInfo &status);

private:
  QHash<int, Project> projects_;
  QHash<int, QVector<CollaborationEntry>> history_;
  int nextProjectId_ = 1;
  int nextMilestoneId_ = 1;
  int nextDeliverableId_ = 1;
  static constexpr int kMaxHistory = 500;
};
