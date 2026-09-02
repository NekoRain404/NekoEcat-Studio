#include "FreeRunOptimizationService.h"
#include "EventBus.h"
#include "infra/EcatClient.h"

#include <QJsonArray>
#include <QJsonObject>

FreeRunOptimizationService::FreeRunOptimizationService(EcatClient* client, EventBus* bus, QObject* parent)
    : QObject(parent), client_(client), bus_(bus) {}

FreeRunOptimizationResult FreeRunOptimizationService::createRejectedResult(const QString& category,
                                                                           const QStringList& recommendations) {
    FreeRunOptimizationResult r;
    r.category = category;
    r.description = tr("%1 optimization requires a connected Free Run optimization backend").arg(category);
    r.improvement = 0.0;
    r.recommendations = recommendations;
    r.applied = false;
    r.timestamp = QDateTime::currentDateTime();
    return r;
}

FreeRunOptimizationResult FreeRunOptimizationService::optimizeCycleTime() {
    QStringList recs;
    recs << tr("Collect live Free Run cycle-time and jitter evidence")
         << tr("Require backend validation before changing process-data timing")
         << tr("Keep current timing unchanged until measured targets are available");

    return createRejectedResult(tr("Cycle Time"), recs);
}

FreeRunOptimizationResult FreeRunOptimizationService::optimizeDataMapping() {
    QStringList recs;
    recs << tr("Collect live PDO byte counts and mapped-entry evidence")
         << tr("Require backend validation before pruning or remapping process data")
         << tr("Keep current PDO mapping unchanged until a measured plan is available");

    return createRejectedResult(tr("Data Mapping"), recs);
}

FreeRunOptimizationResult FreeRunOptimizationService::optimizePerformance() {
    QStringList recs;
    recs << tr("Collect live CPU, bus-load, and frame-rate evidence")
         << tr("Require backend validation before changing Free Run scheduling")
         << tr("Keep current performance settings unchanged until measured targets are available");

    return createRejectedResult(tr("Performance"), recs);
}

FreeRunOptimizationResult FreeRunOptimizationService::optimizeErrorHandling() {
    QStringList recs;
    recs << tr("Collect live error-rate and recovery-time evidence")
         << tr("Require backend validation before changing retry or recovery policy")
         << tr("Keep current error handling unchanged until measured targets are available");

    return createRejectedResult(tr("Error Handling"), recs);
}

bool FreeRunOptimizationService::applyOptimization(const FreeRunOptimizationResult& result) {
    Q_UNUSED(result);
    return false;
}

void FreeRunOptimizationService::clearHistory() {
    history_.clear();
}
