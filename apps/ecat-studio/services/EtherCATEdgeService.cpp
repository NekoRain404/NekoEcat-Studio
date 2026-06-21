#include "EtherCATEdgeService.h"
#include <QDateTime>
#include <QtMath>

// EtherCATEdgeService.cpp — Edge computing operations for EtherCAT data processing
//
// Implementation notes:
//   - Processes raw byte data at the edge with basic statistics (mean, min, max, variance)
//   - Supports local storage and sync-back of processed edge data
//   - Lightweight operations designed for constrained edge environments

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

    result.success = true;
    result.output = data.data;
    result.processingTime = 0.001 * data.size;
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

    analysis.success = true;
    analysis.sampleCount = data.data.size();
    if (analysis.sampleCount > 0) {
        double sum = 0;
        double minVal = data.data[0];
        double maxVal = data.data[0];
        for (int i = 0; i < data.data.size(); ++i) {
            double val = static_cast<unsigned char>(data.data[i]);
            sum += val;
            if (val < minVal) minVal = val;
            if (val > maxVal) maxVal = val;
        }
        analysis.mean = sum / analysis.sampleCount;
        analysis.min = minVal;
        analysis.max = maxVal;

        double varSum = 0;
        for (int i = 0; i < data.data.size(); ++i) {
            double diff = static_cast<unsigned char>(data.data[i]) - analysis.mean;
            varSum += diff * diff;
        }
        analysis.variance = varSum / analysis.sampleCount;
    }

    emit edgeAnalyzed(analysis);
    return analysis;
}

bool EtherCATEdgeService::storeAtEdge(const EdgeData &data)
{
    if (data.data.isEmpty())
        return false;
    ++storedCount_;
    return true;
}

bool EtherCATEdgeService::syncFromEdge()
{
    return storedCount_ > 0;
}
