#include "WorkflowMaintenanceService.h"

WorkflowMaintenanceService::WorkflowMaintenanceService(QObject *parent)
    : QObject(parent)
{
}

bool WorkflowMaintenanceService::scheduleMaintenance(const WfMaintenanceTask &task)
{
    if (task.description.isEmpty())
        return false;

    WfMaintenanceTask scheduled = task;
    if (scheduled.taskId == 0)
        scheduled.taskId = nextTaskId_++;
    else if (scheduled.taskId >= nextTaskId_)
        nextTaskId_ = scheduled.taskId + 1;

    tasks_.append(scheduled);
    emit maintenanceScheduled(scheduled);
    return true;
}

bool WorkflowMaintenanceService::executeMaintenance(int taskId)
{
    for (int i = 0; i < tasks_.size(); ++i) {
        if (tasks_[i].taskId == taskId) {
            return false;
        }
    }
    return false;
}

QVector<WfMaintenanceRecord> WorkflowMaintenanceService::maintenanceHistory() const
{
    return history_;
}

QVector<WfMaintenanceTask> WorkflowMaintenanceService::maintenanceSchedule() const
{
    return tasks_;
}
