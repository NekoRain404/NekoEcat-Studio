#include "WorkflowEdgeService.h"
#include <QDateTime>

WorkflowEdgeService::WorkflowEdgeService(QObject *parent)
    : QObject(parent)
{
}

WfEdgeResult WorkflowEdgeService::processAtEdge(const WfEdgeData &data)
{
    WfEdgeResult result;
    if (data.data.isEmpty()) {
        result.error = QStringLiteral("empty data");
        return result;
    }

    result.success = true;
    result.output = data.data;
    result.processingTime = 0.5;
    emit edgeProcessed(result);
    return result;
}

WfEdgeAnalysis WorkflowEdgeService::analyzeAtEdge(const WfEdgeData &data)
{
    WfEdgeAnalysis analysis;
    if (data.data.isEmpty())
        return analysis;

    analysis.success = true;
    analysis.sampleCount = data.data.size();
    analysis.mean = 0.0;
    analysis.variance = 0.0;
    analysis.min = 0.0;
    analysis.max = 0.0;
    analysis.pattern = QStringLiteral("uniform");
    emit edgeAnalyzed(analysis);
    return analysis;
}

bool WorkflowEdgeService::storeAtEdge(const WfEdgeData &data)
{
    if (data.data.isEmpty())
        return false;

    ++storedCount_;
    return true;
}

bool WorkflowEdgeService::syncFromEdge()
{
    return storedCount_ > 0;
}
