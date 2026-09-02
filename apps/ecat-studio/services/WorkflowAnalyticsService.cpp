#include "WorkflowAnalyticsService.h"
#include <algorithm>
#include <numeric>
#include <QJsonArray>

// WorkflowAnalyticsService.cpp — Analytics engine for workflow execution, performance, errors, and resources
//
// Implementation notes:
//   - Records execution history (success/fail, duration) and error/resource events per workflow
//   - Execution analysis computes success rate, min/max/avg duration, and outlier detection
//   - Performance analysis calculates p95 latency, throughput, and parallelism efficiency

WorkflowAnalyticsService::WorkflowAnalyticsService(QObject* parent) : QObject(parent) {}

void WorkflowAnalyticsService::recordExecution(const QString& workflowId, bool success, double durationMs,
                                               const QJsonObject& details) {
    ExecutionRecord rec;
    rec.success = success;
    rec.durationMs = durationMs;
    rec.timestamp = QDateTime::currentDateTime();
    rec.details = details;
    executions_[workflowId].append(rec);
}

void WorkflowAnalyticsService::recordError(const QString& workflowId, const QString& errorType,
                                           const QString& message) {
    ErrorRecord rec;
    rec.errorType = errorType;
    rec.message = message;
    rec.timestamp = QDateTime::currentDateTime();
    errors_[workflowId].append(rec);
}

void WorkflowAnalyticsService::recordResourceUsage(const QString& workflowId, double cpuPercent, double memoryMb,
                                                   double networkKbps) {
    ResourceRecord rec;
    rec.cpuPercent = cpuPercent;
    rec.memoryMb = memoryMb;
    rec.networkKbps = networkKbps;
    rec.timestamp = QDateTime::currentDateTime();
    resourceUsage_[workflowId].append(rec);
}

WfExecutionAnalysis WorkflowAnalyticsService::analyzeExecution(const QString& workflowId) {
    WfExecutionAnalysis analysis;
    analysis.workflowId = workflowId;

    const auto& records = executions_[workflowId];
    analysis.totalExecutions = records.size();

    if (records.isEmpty()) {
        WfAnalysisResult result;
        result.workflowId = workflowId;
        result.analysisType = QStringLiteral("Execution");
        result.timestamp = QDateTime::currentDateTime();
        emit analysisCompleted(result);
        return analysis;
    }

    int successCount = 0;
    double totalDuration = 0.0;
    double minDur = records.first().durationMs;
    double maxDur = records.first().durationMs;

    for (const auto& rec : records) {
        if (rec.success)
            successCount++;
        totalDuration += rec.durationMs;
        minDur = qMin(minDur, rec.durationMs);
        maxDur = qMax(maxDur, rec.durationMs);
    }

    analysis.successfulExecutions = successCount;
    analysis.failedExecutions = records.size() - successCount;
    analysis.successRate = static_cast<double>(successCount) / records.size() * 100.0;
    analysis.averageDurationMs = totalDuration / records.size();
    analysis.minDurationMs = minDur;
    analysis.maxDurationMs = maxDur;

    if (analysis.successRate < 90.0)
        analysis.recommendations << QStringLiteral("Success rate below 90%% — investigate failure patterns");
    if (maxDur > analysis.averageDurationMs * 3.0)
        analysis.bottlenecks << QStringLiteral("Outlier executions detected (max/avg > 3x)");

    QJsonObject details;
    details[QStringLiteral("totalExecutions")] = analysis.totalExecutions;
    details[QStringLiteral("successRate")] = analysis.successRate;
    details[QStringLiteral("averageDurationMs")] = analysis.averageDurationMs;
    analysis.details = details;

    WfAnalysisResult result;
    result.workflowId = workflowId;
    result.analysisType = QStringLiteral("Execution");
    result.timestamp = QDateTime::currentDateTime();
    result.data = details;
    emit analysisCompleted(result);

    return analysis;
}

WfPerformanceAnalysis WorkflowAnalyticsService::analyzePerformance(const QString& workflowId) {
    WfPerformanceAnalysis analysis;
    analysis.workflowId = workflowId;

    const auto& records = executions_[workflowId];
    if (records.isEmpty()) {
        WfAnalysisResult result;
        result.workflowId = workflowId;
        result.analysisType = QStringLiteral("Performance");
        result.timestamp = QDateTime::currentDateTime();
        emit analysisCompleted(result);
        return analysis;
    }

    QVector<double> durations;
    for (const auto& rec : records)
        durations.append(rec.durationMs);

    std::sort(durations.begin(), durations.end());

    double sum = 0.0;
    for (double d : durations)
        sum += d;
    analysis.averageStepDurationMs = sum / durations.size();

    int p95Index = static_cast<int>(durations.size() * 0.95);
    if (p95Index >= durations.size())
        p95Index = durations.size() - 1;
    analysis.p95StepDurationMs = durations[p95Index];

    double totalTimeSec = sum / 1000.0;
    analysis.throughputPerSecond = (totalTimeSec > 0.0) ? static_cast<double>(records.size()) / totalTimeSec : 0.0;

    analysis.parallelismEfficiency = (analysis.p95StepDurationMs > 0.0)
                                         ? analysis.averageStepDurationMs / analysis.p95StepDurationMs * 100.0
                                         : 100.0;

    for (int i = 0; i < records.size(); ++i) {
        if (records[i].durationMs > analysis.averageStepDurationMs * 2.0)
            analysis.slowSteps << QString::number(i);
    }

    if (analysis.p95StepDurationMs > analysis.averageStepDurationMs * 2.0)
        analysis.recommendations << QStringLiteral("High tail latency detected — optimize slow paths");
    if (analysis.parallelismEfficiency < 50.0)
        analysis.recommendations << QStringLiteral("Low parallelism efficiency — review task dependencies");

    QJsonObject metrics;
    metrics[QStringLiteral("averageMs")] = analysis.averageStepDurationMs;
    metrics[QStringLiteral("p95Ms")] = analysis.p95StepDurationMs;
    metrics[QStringLiteral("throughput")] = analysis.throughputPerSecond;
    metrics[QStringLiteral("parallelismEfficiency")] = analysis.parallelismEfficiency;
    analysis.metrics = metrics;

    WfAnalysisResult result;
    result.workflowId = workflowId;
    result.analysisType = QStringLiteral("Performance");
    result.timestamp = QDateTime::currentDateTime();
    result.data = metrics;
    emit analysisCompleted(result);

    return analysis;
}

WfErrorAnalysis WorkflowAnalyticsService::analyzeErrors(const QString& workflowId) {
    WfErrorAnalysis analysis;
    analysis.workflowId = workflowId;

    const auto& records = errors_[workflowId];
    analysis.totalErrors = records.size();

    if (records.isEmpty()) {
        WfAnalysisResult result;
        result.workflowId = workflowId;
        result.analysisType = QStringLiteral("Error");
        result.timestamp = QDateTime::currentDateTime();
        emit analysisCompleted(result);
        return analysis;
    }

    QJsonObject distribution;
    QHash<QString, int> typeCounts;
    for (const auto& rec : records) {
        typeCounts[rec.errorType]++;
    }
    for (auto it = typeCounts.begin(); it != typeCounts.end(); ++it) {
        distribution[it.key()] = it.value();
    }
    analysis.errorDistribution = distribution;
    analysis.uniqueErrorTypes = typeCounts.size();

    int maxCount = 0;
    for (auto it = typeCounts.begin(); it != typeCounts.end(); ++it) {
        if (it.value() > maxCount) {
            maxCount = it.value();
            analysis.mostCommonError = it.key();
        }
    }

    const auto& execs = executions_[workflowId];
    analysis.errorRate = execs.isEmpty() ? 0.0 : static_cast<double>(records.size()) / execs.size() * 100.0;

    int recentCount = qMin(5, records.size());
    for (int i = records.size() - recentCount; i < records.size(); ++i)
        analysis.recentErrors << records[i].message;

    if (analysis.errorRate > 10.0)
        analysis.recommendations << QStringLiteral("Error rate above 10%% — review workflow stability");
    if (analysis.uniqueErrorTypes > 5)
        analysis.recommendations << QStringLiteral("Many error types — consolidate error handling");

    WfAnalysisResult result;
    result.workflowId = workflowId;
    result.analysisType = QStringLiteral("Error");
    result.timestamp = QDateTime::currentDateTime();
    result.data = distribution;
    emit analysisCompleted(result);

    return analysis;
}

WfResourceAnalysis WorkflowAnalyticsService::analyzeResources(const QString& workflowId) {
    WfResourceAnalysis analysis;
    analysis.workflowId = workflowId;

    const auto& records = resourceUsage_[workflowId];
    if (records.isEmpty()) {
        WfAnalysisResult result;
        result.workflowId = workflowId;
        result.analysisType = QStringLiteral("Resource");
        result.timestamp = QDateTime::currentDateTime();
        emit analysisCompleted(result);
        return analysis;
    }

    double totalCpu = 0.0, totalMem = 0.0, totalNet = 0.0;
    double peakCpu = 0.0, peakMem = 0.0;

    for (const auto& rec : records) {
        totalCpu += rec.cpuPercent;
        totalMem += rec.memoryMb;
        totalNet += rec.networkKbps;
        peakCpu = qMax(peakCpu, rec.cpuPercent);
        peakMem = qMax(peakMem, rec.memoryMb);
    }

    analysis.averageCpuPercent = totalCpu / records.size();
    analysis.peakCpuPercent = peakCpu;
    analysis.averageMemoryMb = totalMem / records.size();
    analysis.peakMemoryMb = peakMem;
    analysis.averageNetworkKbps = totalNet / records.size();

    if (peakCpu > 80.0)
        analysis.overutilizedResources << QStringLiteral("CPU");
    if (peakMem > 1024.0)
        analysis.overutilizedResources << QStringLiteral("Memory");

    if (analysis.averageCpuPercent < 20.0)
        analysis.underutilizedResources << QStringLiteral("CPU");
    if (analysis.averageMemoryMb < 100.0)
        analysis.underutilizedResources << QStringLiteral("Memory");

    if (!analysis.overutilizedResources.isEmpty())
        analysis.recommendations
            << QStringLiteral("Resource contention on: %1").arg(analysis.overutilizedResources.join(", "));
    if (!analysis.underutilizedResources.isEmpty())
        analysis.recommendations << QStringLiteral("Underutilized resources: %1 — consider scaling down")
                                        .arg(analysis.underutilizedResources.join(", "));

    QJsonObject timeline;
    QJsonArray cpuArray, memArray;
    for (const auto& rec : records) {
        cpuArray.append(rec.cpuPercent);
        memArray.append(rec.memoryMb);
    }
    timeline[QStringLiteral("cpu")] = cpuArray;
    timeline[QStringLiteral("memory")] = memArray;
    analysis.resourceTimeline = timeline;

    QJsonObject metrics;
    metrics[QStringLiteral("avgCpu")] = analysis.averageCpuPercent;
    metrics[QStringLiteral("peakCpu")] = analysis.peakCpuPercent;
    metrics[QStringLiteral("avgMemory")] = analysis.averageMemoryMb;
    metrics[QStringLiteral("peakMemory")] = analysis.peakMemoryMb;

    WfAnalysisResult result;
    result.workflowId = workflowId;
    result.analysisType = QStringLiteral("Resource");
    result.timestamp = QDateTime::currentDateTime();
    result.data = metrics;
    emit analysisCompleted(result);

    return analysis;
}
