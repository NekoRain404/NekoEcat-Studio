#include "WorkflowSchedulingService.h"

// WorkflowSchedulingService.cpp — Schedules, triggers, pauses, and cancels workflow runs
//
// Implementation notes:
//   - Workflows registered by ID with step definitions and cron-like schedules
//   - Run tracking with status progression: Scheduled → Running → Completed/Cancelled
//   - Auto-incrementing run IDs per workflow

WorkflowSchedulingService::WorkflowSchedulingService(QObject *parent)
    : QObject(parent)
{
}

bool WorkflowSchedulingService::scheduleWorkflow(const WorkflowConfig &config)
{
    if (config.workflowId.isEmpty() || config.name.isEmpty())
        return false;

    workflows_[config.workflowId] = config;
    runs_[config.workflowId] = QVector<WorkflowRun>();
    emit workflowScheduled(config);
    return true;
}

bool WorkflowSchedulingService::triggerWorkflow(const QString &workflowId)
{
    if (!workflows_.contains(workflowId))
        return false;

    const auto &cfg = workflows_[workflowId];

    WorkflowRun run;
    run.workflowId = workflowId;
    run.runId = nextRunId();
    run.status = WorkflowStatus::Running;
    run.scheduledAt = QDateTime::currentDateTime();
    run.startedAt = QDateTime::currentDateTime();
    run.totalSteps = cfg.steps.size();
    run.currentStep = 0;
    runs_[workflowId].append(run);

    emit workflowTriggered(workflowId);

    auto &r = runs_[workflowId].last();
    r.status = WorkflowStatus::Completed;
    r.completedAt = QDateTime::currentDateTime();
    r.currentStep = r.totalSteps;
    emit workflowCompleted(workflowId, true);
    return true;
}

bool WorkflowSchedulingService::pauseWorkflow(const QString &workflowId)
{
    if (!workflows_.contains(workflowId))
        return false;

    auto &rs = runs_[workflowId];
    for (auto it = rs.begin(); it != rs.end(); ++it) {
        if (it->status == WorkflowStatus::Running) {
            it->status = WorkflowStatus::Paused;
            emit workflowPaused(workflowId);
            return true;
        }
    }
    return false;
}

bool WorkflowSchedulingService::resumeWorkflow(const QString &workflowId)
{
    if (!workflows_.contains(workflowId))
        return false;

    auto &rs = runs_[workflowId];
    for (auto it = rs.begin(); it != rs.end(); ++it) {
        if (it->status == WorkflowStatus::Paused) {
            it->status = WorkflowStatus::Running;
            emit workflowResumed(workflowId);
            return true;
        }
    }
    return false;
}

bool WorkflowSchedulingService::cancelWorkflow(const QString &workflowId)
{
    if (!workflows_.contains(workflowId))
        return false;

    workflows_.remove(workflowId);
    auto rs = runs_.take(workflowId);
    for (auto &r : rs) {
        if (r.status == WorkflowStatus::Running || r.status == WorkflowStatus::Paused) {
            r.status = WorkflowStatus::Cancelled;
            r.completedAt = QDateTime::currentDateTime();
        }
    }
    return true;
}

WorkflowConfig WorkflowSchedulingService::workflow(const QString &workflowId) const
{
    return workflows_.value(workflowId);
}

QVector<WorkflowConfig> WorkflowSchedulingService::allWorkflows() const
{
    QVector<WorkflowConfig> result;
    for (auto it = workflows_.begin(); it != workflows_.end(); ++it)
        result.append(it.value());
    return result;
}

QVector<WorkflowRun> WorkflowSchedulingService::runs(const QString &workflowId) const
{
    return runs_.value(workflowId);
}

int WorkflowSchedulingService::workflowCount() const
{
    return workflows_.size();
}

QString WorkflowSchedulingService::nextRunId()
{
    return QStringLiteral("run-%1").arg(++runCounter_);
}
