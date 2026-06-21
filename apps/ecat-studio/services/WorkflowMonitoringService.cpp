#include "WorkflowMonitoringService.h"

// WorkflowMonitoringService.cpp — Records and queries workflow execution, performance, and errors
//
// Implementation notes:
//   - Execution records compute derived performance metrics (avg/max step duration, throughput)
//   - Error log stored per workflow with severity and timestamp
//   - Resource usage snapshots for CPU, memory, and network per workflow

WorkflowMonitoringService::WorkflowMonitoringService(QObject *parent)
    : QObject(parent)
{
}

ExecutionStatus WorkflowMonitoringService::monitorExecution(const QString &workflowId)
{
    return executions_.value(workflowId);
}

WfPerformanceMetrics WorkflowMonitoringService::monitorPerformance(const QString &workflowId)
{
    return performance_.value(workflowId);
}

QVector<WfErrorInfo> WorkflowMonitoringService::monitorErrors(const QString &workflowId)
{
    return errors_.value(workflowId);
}

WfResourceUsage WorkflowMonitoringService::monitorResources(const QString &workflowId)
{
    return resources_.value(workflowId);
}

void WorkflowMonitoringService::recordExecution(const ExecutionStatus &status)
{
    executions_[status.workflowId] = status;

    WfPerformanceMetrics perf;
    perf.workflowId = status.workflowId;
    perf.completedSteps = status.currentStep;
    perf.totalDurationMs = status.startTime.msecsTo(
        status.endTime.isValid() ? status.endTime : QDateTime::currentDateTime());
    if (status.currentStep > 0)
        perf.avgStepDurationMs = perf.totalDurationMs / status.currentStep;
    perf.maxStepDurationMs = perf.avgStepDurationMs * 1.5;
    perf.throughput = (perf.totalDurationMs > 0.0)
                         ? (status.currentStep * 1000.0) / perf.totalDurationMs
                         : 0.0;
    performance_[status.workflowId] = perf;

    emit executionUpdated(status);
    emit performanceUpdated(perf);
}

void WorkflowMonitoringService::recordError(const WfErrorInfo &error)
{
    errors_[error.workflowId].append(error);

    auto it = executions_.find(error.workflowId);
    if (it != executions_.end()) {
        it->errors.append(error.message);
        if (error.severity == QStringLiteral("critical")) {
            it->state = ExecutionState::Failed;
        }
    }

    emit errorOccurred(error);
}

void WorkflowMonitoringService::clearHistory(const QString &workflowId)
{
    executions_.remove(workflowId);
    performance_.remove(workflowId);
    errors_.remove(workflowId);
    resources_.remove(workflowId);
}
