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
            WfMaintenanceRecord record;
            record.taskId = tasks_[i].taskId;
            record.type = tasks_[i].type;
            record.description = tasks_[i].description;
            record.startTime = QDateTime::currentDateTime();
            record.endTime = QDateTime::currentDateTime();
            record.success = true;
            history_.append(record);
            emit maintenanceCompleted(record);
            return true;
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
