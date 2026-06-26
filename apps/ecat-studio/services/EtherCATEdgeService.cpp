#include "EtherCATEdgeService.h"

namespace {
QString edgeBackendUnavailableMessage()
{
    return QStringLiteral("Edge backend is not available");
}
}

EtherCATEdgeService::EtherCATEdgeService(QObject *parent)
    : QObject(parent)
{
}

EdgeResult EtherCATEdgeService::processAtEdge(const EdgeData &data)
{
    EdgeResult result;
    if (data.data.isEmpty()) {
        result.error = QStringLiteral("Empty data");
        emit edgeProcessed(result);
        return result;
    }

    result.error = edgeBackendUnavailableMessage();
    emit edgeProcessed(result);
    return result;
}

EdgeAnalysis EtherCATEdgeService::analyzeAtEdge(const EdgeData &data)
{
    EdgeAnalysis analysis;
    if (data.data.isEmpty()) {
        emit edgeAnalyzed(analysis);
        return analysis;
    }

    emit edgeAnalyzed(analysis);
    return analysis;
}

bool EtherCATEdgeService::storeAtEdge(const EdgeData &data)
{
    if (data.data.isEmpty())
        return false;
    return false;
}

bool EtherCATEdgeService::syncFromEdge()
{
    return false;
}
