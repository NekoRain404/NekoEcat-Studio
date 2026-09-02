#pragma once

// WorkflowMonitoringService -- monitors workflow execution, performance,
// errors, and resource usage in real time.
//
// Thread safety: main (GUI) thread only.

#include <QDateTime>
#include <QHash>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

enum class ExecutionState { Idle, Running, Completed, Failed, Cancelled };

struct ExecutionStatus {
    QString workflowId;
    ExecutionState state = ExecutionState::Idle;
    QDateTime startTime;
    QDateTime endTime;
    double progress = 0.0;
    int currentStep = 0;
    int totalSteps = 0;
    QStringList errors;
    QStringList warnings;
    QJsonObject context;
};

struct WfPerformanceMetrics {
    QString workflowId;
    double totalDurationMs = 0.0;
    double avgStepDurationMs = 0.0;
    double maxStepDurationMs = 0.0;
    int completedSteps = 0;
    int failedSteps = 0;
    double throughput = 0.0;
    QJsonObject stepDurations;
};

struct WfErrorInfo {
    QString workflowId;
    QString stepId;
    QString message;
    QString severity;
    QDateTime timestamp;
    QJsonObject details;
};

struct WfResourceUsage {
    QString workflowId;
    double cpuPercent = 0.0;
    double memoryMb = 0.0;
    double networkKbps = 0.0;
    int activeThreads = 0;
    QJsonObject customResources;
};

class WorkflowMonitoringService : public QObject {
    Q_OBJECT
public:
    explicit WorkflowMonitoringService(QObject* parent = nullptr);

    ExecutionStatus monitorExecution(const QString& workflowId);
    WfPerformanceMetrics monitorPerformance(const QString& workflowId);
    QVector<WfErrorInfo> monitorErrors(const QString& workflowId);
    WfResourceUsage monitorResources(const QString& workflowId);

    void recordExecution(const ExecutionStatus& status);
    void recordError(const WfErrorInfo& error);
    void clearHistory(const QString& workflowId);

signals:
    void executionUpdated(const ExecutionStatus& status);
    void performanceUpdated(const WfPerformanceMetrics& metrics);
    void errorOccurred(const WfErrorInfo& error);
    void resourceUsageUpdated(const WfResourceUsage& usage);

private:
    QHash<QString, ExecutionStatus> executions_;
    QHash<QString, WfPerformanceMetrics> performance_;
    QHash<QString, QVector<WfErrorInfo>> errors_;
    QHash<QString, WfResourceUsage> resources_;
};
