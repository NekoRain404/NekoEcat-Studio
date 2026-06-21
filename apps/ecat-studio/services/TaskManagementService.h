#pragma once

// TaskManagementService -- manages tasks within EtherCAT projects.
//
// Supports creating, assigning, tracking, and reporting on tasks
// with priorities, dependencies, and tags.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QVector>
#include <QHash>
#include <QDateTime>

enum class TaskPriority { Low, Medium, High, Critical };
enum class TaskStatus { Open, InProgress, OnHold, Completed, Cancelled };

struct TaskConfig {
  QString title;
  QString description;
  TaskPriority priority = TaskPriority::Medium;
  QString assignee;
  QDateTime deadline;
  QVector<int> dependencies;
  QStringList tags;
};

struct Task {
  int id = 0;
  QString title;
  QString description;
  TaskPriority priority = TaskPriority::Medium;
  TaskStatus status = TaskStatus::Open;
  QString assignee;
  QDateTime deadline;
  QVector<int> dependencies;
  QStringList tags;
  QDateTime createdAt;
  QDateTime updatedAt;
  QDateTime completedAt;
};

struct TaskStatusInfo {
  int taskId = 0;
  TaskStatus status = TaskStatus::Open;
  QString assignee;
  bool overdue = false;
  QDateTime deadline;
  QDateTime lastUpdated;
};

struct TaskReport {
  int totalTasks = 0;
  int openTasks = 0;
  int inProgressTasks = 0;
  int completedTasks = 0;
  int overdueTasks = 0;
  QVector<Task> tasksByPriority;
  QVector<Task> tasksByAssignee;
  QDateTime generatedAt;
};

class TaskManagementService : public QObject {
  Q_OBJECT
public:
  explicit TaskManagementService(QObject *parent = nullptr);

  Task createTask(const TaskConfig &config);
  bool assignTask(int taskId, const QString &assignee);
  TaskStatusInfo trackTask(int taskId) const;
  TaskReport generateTaskReport() const;

  bool updateTaskStatus(int taskId, TaskStatus status);
  bool addDependency(int taskId, int dependencyId);
  bool addTag(int taskId, const QString &tag);
  bool removeTag(int taskId, const QString &tag);

  Task task(int taskId) const;
  QVector<Task> allTasks() const;
  QVector<Task> tasksByAssignee(const QString &assignee) const;
  QVector<Task> tasksByStatus(TaskStatus status) const;
  int taskCount() const;

signals:
  void taskCreated(const Task &task);
  void taskUpdated(const TaskStatusInfo &status);

private:
  QHash<int, Task> tasks_;
  int nextTaskId_ = 1;
  static constexpr int kMaxTasks = 10000;
};
