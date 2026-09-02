#pragma once

// WorkflowSchedulingService -- manages cron-based, event-based, dependency-based,
// and priority-based workflow scheduling.
//
// Thread safety: main (GUI) thread only.

#include <QDateTime>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

enum class ScheduleType { Cron, Event, Dependency, Priority };
enum class WorkflowStatus { Scheduled, Running, Paused, Completed, Failed, Cancelled };

struct WorkflowConfig {
    QString workflowId;
    QString name;
    QString description;
    ScheduleType scheduleType = ScheduleType::Priority;
    QString schedule;
    QStringList triggers;
    QVector<QJsonObject> steps;
    QStringList dependencies;
    int priority = 0;
    int timeoutMs = 60000;
    QJsonObject metadata;
};

struct WorkflowRun {
    QString workflowId;
    QString runId;
    WorkflowStatus status = WorkflowStatus::Scheduled;
    QDateTime scheduledAt;
    QDateTime startedAt;
    QDateTime completedAt;
    int currentStep = 0;
    int totalSteps = 0;
    QString error;
};

class WorkflowSchedulingService : public QObject {
    Q_OBJECT
public:
    explicit WorkflowSchedulingService(QObject* parent = nullptr);

    bool scheduleWorkflow(const WorkflowConfig& config);
    bool triggerWorkflow(const QString& workflowId);
    bool pauseWorkflow(const QString& workflowId);
    bool resumeWorkflow(const QString& workflowId);
    bool cancelWorkflow(const QString& workflowId);

    WorkflowConfig workflow(const QString& workflowId) const;
    QVector<WorkflowConfig> allWorkflows() const;
    QVector<WorkflowRun> runs(const QString& workflowId) const;
    int workflowCount() const;

signals:
    void workflowScheduled(const WorkflowConfig& config);
    void workflowTriggered(const QString& workflowId);
    void workflowPaused(const QString& workflowId);
    void workflowResumed(const QString& workflowId);
    void workflowCompleted(const QString& workflowId, bool success);

private:
    QHash<QString, WorkflowConfig> workflows_;
    QHash<QString, QVector<WorkflowRun>> runs_;
    QString nextRunId();
    int runCounter_ = 0;
};
