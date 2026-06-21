#include "TaskManagementService.h"

// TaskManagementService.cpp — CRUD and lifecycle management for project tasks
//
// Implementation notes:
//   - Tasks carry priority, assignee, deadline, dependencies, and tags
//   - Status progression: Open → InProgress → Completed/Blocked
//   - Filtering by status, assignee, and tag-based queries

TaskManagementService::TaskManagementService(QObject *parent)
    : QObject(parent)
{
}

Task TaskManagementService::createTask(const TaskConfig &config)
{
    Task t;
    t.id = nextTaskId_++;
    t.title = config.title;
    t.description = config.description;
    t.priority = config.priority;
    t.assignee = config.assignee;
    t.deadline = config.deadline;
    t.dependencies = config.dependencies;
    t.tags = config.tags;
    t.status = TaskStatus::Open;
    t.createdAt = QDateTime::currentDateTime();
    t.updatedAt = t.createdAt;

    tasks_.insert(t.id, t);
    emit taskCreated(t);
    return t;
}

bool TaskManagementService::assignTask(int taskId, const QString &assignee)
{
    auto it = tasks_.find(taskId);
    if (it == tasks_.end())
        return false;

    it->assignee = assignee;
    it->updatedAt = QDateTime::currentDateTime();
    emit taskUpdated(trackTask(taskId));
    return true;
}

TaskStatusInfo TaskManagementService::trackTask(int taskId) const
{
    TaskStatusInfo info;
    auto it = tasks_.find(taskId);
    if (it == tasks_.end())
        return info;

    const Task &t = *it;
    info.taskId = t.id;
    info.status = t.status;
    info.assignee = t.assignee;
    info.deadline = t.deadline;
    info.lastUpdated = t.updatedAt;
    info.overdue = (t.deadline.isValid() && QDateTime::currentDateTime() > t.deadline
                    && t.status != TaskStatus::Completed && t.status != TaskStatus::Cancelled);
    return info;
}

TaskReport TaskManagementService::generateTaskReport() const
{
    TaskReport report;
    report.generatedAt = QDateTime::currentDateTime();
    report.totalTasks = tasks_.size();

    for (const auto &t : tasks_) {
        switch (t.status) {
        case TaskStatus::Open: report.openTasks++; break;
        case TaskStatus::InProgress: report.inProgressTasks++; break;
        case TaskStatus::Completed: report.completedTasks++; break;
        default: break;
        }

        if (t.deadline.isValid() && QDateTime::currentDateTime() > t.deadline
            && t.status != TaskStatus::Completed && t.status != TaskStatus::Cancelled)
            report.overdueTasks++;

        report.tasksByPriority.append(t);
    }

    std::sort(report.tasksByPriority.begin(), report.tasksByPriority.end(),
              [](const Task &a, const Task &b) { return a.priority > b.priority; });

    report.tasksByAssignee = report.tasksByPriority;
    std::sort(report.tasksByAssignee.begin(), report.tasksByAssignee.end(),
              [](const Task &a, const Task &b) { return a.assignee < b.assignee; });

    return report;
}

bool TaskManagementService::updateTaskStatus(int taskId, TaskStatus status)
{
    auto it = tasks_.find(taskId);
    if (it == tasks_.end())
        return false;

    it->status = status;
    it->updatedAt = QDateTime::currentDateTime();
    if (status == TaskStatus::Completed)
        it->completedAt = QDateTime::currentDateTime();

    emit taskUpdated(trackTask(taskId));
    return true;
}

bool TaskManagementService::addDependency(int taskId, int dependencyId)
{
    auto it = tasks_.find(taskId);
    if (it == tasks_.end())
        return false;

    if (!it->dependencies.contains(dependencyId))
        it->dependencies.append(dependencyId);

    it->updatedAt = QDateTime::currentDateTime();
    return true;
}

bool TaskManagementService::addTag(int taskId, const QString &tag)
{
    auto it = tasks_.find(taskId);
    if (it == tasks_.end())
        return false;

    if (!it->tags.contains(tag))
        it->tags.append(tag);

    it->updatedAt = QDateTime::currentDateTime();
    return true;
}

bool TaskManagementService::removeTag(int taskId, const QString &tag)
{
    auto it = tasks_.find(taskId);
    if (it == tasks_.end())
        return false;

    it->tags.removeAll(tag);
    it->updatedAt = QDateTime::currentDateTime();
    return true;
}

Task TaskManagementService::task(int taskId) const
{
    auto it = tasks_.find(taskId);
    if (it != tasks_.end())
        return *it;
    return {};
}

QVector<Task> TaskManagementService::allTasks() const
{
    QVector<Task> result;
    result.reserve(tasks_.size());
    for (const auto &t : tasks_)
        result.append(t);
    return result;
}

QVector<Task> TaskManagementService::tasksByAssignee(const QString &assignee) const
{
    QVector<Task> result;
    for (const auto &t : tasks_) {
        if (t.assignee == assignee)
            result.append(t);
    }
    return result;
}

QVector<Task> TaskManagementService::tasksByStatus(TaskStatus status) const
{
    QVector<Task> result;
    for (const auto &t : tasks_) {
        if (t.status == status)
            result.append(t);
    }
    return result;
}

int TaskManagementService::taskCount() const
{
    return tasks_.size();
}
