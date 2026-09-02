#pragma once

// WorkflowAnalyticsService — analyzes workflow execution, performance,
// errors, and resource usage to produce actionable insights.
//
// Thread safety: main (GUI) thread only.

#include <QDateTime>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

struct WfExecutionAnalysis {
    QString workflowId;
    int totalExecutions = 0;
    int successfulExecutions = 0;
    int failedExecutions = 0;
    double successRate = 0.0;
    double averageDurationMs = 0.0;
    double minDurationMs = 0.0;
    double maxDurationMs = 0.0;
    QStringList bottlenecks;
    QStringList recommendations;
    QJsonObject details;
};

struct WfPerformanceAnalysis {
    QString workflowId;
    double averageStepDurationMs = 0.0;
    double p95StepDurationMs = 0.0;
    double throughputPerSecond = 0.0;
    double parallelismEfficiency = 0.0;
    QStringList slowSteps;
    QStringList recommendations;
    QJsonObject metrics;
};

struct WfErrorAnalysis {
    QString workflowId;
    int totalErrors = 0;
    int uniqueErrorTypes = 0;
    double errorRate = 0.0;
    QJsonObject errorDistribution;
    QString mostCommonError;
    QStringList recentErrors;
    QStringList recommendations;
};

struct WfResourceAnalysis {
    QString workflowId;
    double averageCpuPercent = 0.0;
    double peakCpuPercent = 0.0;
    double averageMemoryMb = 0.0;
    double peakMemoryMb = 0.0;
    double averageNetworkKbps = 0.0;
    QStringList overutilizedResources;
    QStringList underutilizedResources;
    QStringList recommendations;
    QJsonObject resourceTimeline;
};

struct WfAnalysisResult {
    QString workflowId;
    QString analysisType;
    QDateTime timestamp;
    QJsonObject data;
};

class WorkflowAnalyticsService : public QObject {
    Q_OBJECT
public:
    explicit WorkflowAnalyticsService(QObject* parent = nullptr);

    WfExecutionAnalysis analyzeExecution(const QString& workflowId);
    WfPerformanceAnalysis analyzePerformance(const QString& workflowId);
    WfErrorAnalysis analyzeErrors(const QString& workflowId);
    WfResourceAnalysis analyzeResources(const QString& workflowId);

    void recordExecution(const QString& workflowId, bool success, double durationMs, const QJsonObject& details = {});
    void recordError(const QString& workflowId, const QString& errorType, const QString& message);
    void recordResourceUsage(const QString& workflowId, double cpuPercent, double memoryMb, double networkKbps);

signals:
    void analysisCompleted(const WfAnalysisResult& result);

private:
    struct ExecutionRecord {
        bool success = false;
        double durationMs = 0.0;
        QDateTime timestamp;
        QJsonObject details;
    };

    struct ErrorRecord {
        QString errorType;
        QString message;
        QDateTime timestamp;
    };

    struct ResourceRecord {
        double cpuPercent = 0.0;
        double memoryMb = 0.0;
        double networkKbps = 0.0;
        QDateTime timestamp;
    };

    QHash<QString, QVector<ExecutionRecord>> executions_;
    QHash<QString, QVector<ErrorRecord>> errors_;
    QHash<QString, QVector<ResourceRecord>> resourceUsage_;
};
