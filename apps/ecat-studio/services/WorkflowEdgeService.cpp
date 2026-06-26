#include "WorkflowEdgeService.h"

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

    return result;
}

WfEdgeAnalysis WorkflowEdgeService::analyzeAtEdge(const WfEdgeData &data)
{
    WfEdgeAnalysis analysis;
    if (data.data.isEmpty())
        return analysis;

    return analysis;
}

bool WorkflowEdgeService::storeAtEdge(const WfEdgeData &data)
{
    Q_UNUSED(data);
    return false;
}

bool WorkflowEdgeService::syncFromEdge()
{
    return storedCount_ > 0;
}
