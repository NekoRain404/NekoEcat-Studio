#include "EtherCATMaintenanceService.h"
#include <QDateTime>

// EtherCATMaintenanceService.cpp — Maintenance task planning facade
//
// Implementation notes:
//   - Uses EventBus and EcatClient for task orchestration
//   - Generates unique task IDs via nextId_ counter
//   - Rejects offline execution instead of synthesizing completed maintenance

EtherCATMaintenanceService::EtherCATMaintenanceService(EventBus* bus, EcatClient* client, QObject* parent)
    : QObject(parent), bus_(bus), client_(client) {}

MaintenanceTaskInfo EtherCATMaintenanceService::makeTask(const QString& id, const QString& taskType,
                                                         const QString& schedule, const QString& status,
                                                         const QString& result) {
    MaintenanceTaskInfo t;
    t.id = id;
    t.taskType = taskType;
    t.schedule = schedule;
    t.status = status;
    t.lastRun = QDateTime::currentDateTime().toString(Qt::ISODate);
    t.nextRun = QDateTime::currentDateTime().addSecs(3600).toString(Qt::ISODate);
    t.result = result;
    return t;
}

MaintenanceTaskInfo EtherCATMaintenanceService::scheduleTask(const QString& taskType, const QString& schedule) {
    QString id = QStringLiteral("task_%1").arg(nextId_++);
    auto task = makeTask(id, taskType, schedule, QStringLiteral("Scheduled"),
                         QStringLiteral("Task '%1' scheduled: %2").arg(taskType, schedule));
    tasks_.append(task);
    return task;
}

bool EtherCATMaintenanceService::cancelTask(const QString& taskId) {
    for (auto& t : tasks_) {
        if (t.id == taskId) {
            t.status = QStringLiteral("Cancelled");
            return true;
        }
    }
    return false;
}

QVector<MaintenanceTaskInfo> EtherCATMaintenanceService::listTasks() {
    return tasks_;
}

MaintenanceTaskInfo EtherCATMaintenanceService::runTask(const QString& taskId) {
    for (auto& t : tasks_) {
        if (t.id == taskId) {
            auto rejected = t;
            rejected.status = QStringLiteral("Rejected");
            rejected.result =
                QStringLiteral("Task '%1' requires a connected EtherCAT maintenance backend").arg(t.taskType);
            return rejected;
        }
    }
    return makeTask(taskId, QString(), QString(), QStringLiteral("Failed"),
                    QStringLiteral("Task '%1' not found").arg(taskId));
}

MaintenanceTaskInfo EtherCATMaintenanceService::getTaskStatus(const QString& taskId) {
    for (const auto& t : tasks_) {
        if (t.id == taskId)
            return t;
    }
    return makeTask(taskId, QString(), QString(), QStringLiteral("NotFound"),
                    QStringLiteral("Task '%1' not found").arg(taskId));
}

bool EtherCATMaintenanceService::scheduleMaintenance(const ScheduledMaintenanceTask& task) {
    if (task.description.isEmpty())
        return false;

    ScheduledMaintenanceTask t = task;
    t.taskId = nextTaskId_++;
    schedule_.append(t);
    emit maintenanceScheduled(t);
    return true;
}

bool EtherCATMaintenanceService::executeMaintenance(int taskId) {
    for (int i = 0; i < schedule_.size(); ++i) {
        if (schedule_.at(i).taskId == taskId) {
            return false;
        }
    }
    return false;
}

QVector<MaintenanceExecutionRecord> EtherCATMaintenanceService::maintenanceHistory() const {
    return history_;
}

QVector<ScheduledMaintenanceTask> EtherCATMaintenanceService::maintenanceSchedule() const {
    return schedule_;
}
