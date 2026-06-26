#include "DcSyncOptimizerService.h"
#include "infra/EcatClient.h"
#include "EventBus.h"

DcSyncOptimizerService::DcSyncOptimizerService(EcatClient *client,
                                               EventBus *eventBus,
                                               QObject *parent)
    : QObject(parent), client_(client), eventBus_(eventBus) {}

DcSyncOptimizationResult DcSyncOptimizerService::offlineResult(
    const QString &category, const QString &description) const {
    DcSyncOptimizationResult result;
    result.category = category;
    result.description = description;
    result.timestamp = QDateTime::currentDateTime();
    result.recommendations = {
        QStringLiteral("Connect to a live EtherCAT daemon with DC telemetry before running DC sync optimization.")
    };
    return result;
}

QJsonObject DcSyncOptimizerService::collectSyncStatus() const {
    QJsonObject status;
    status["connected"] = client_ && client_->isConnected();
    return status;
}

bool DcSyncOptimizerService::hasDcTelemetry() const {
    return false;
}

DcSyncOptimizationResult DcSyncOptimizerService::optimizeSync() {
    return offlineResult(QStringLiteral("Sync"),
                         QStringLiteral("Distributed Clock synchronization optimization"));
}

DcSyncOptimizationResult DcSyncOptimizerService::optimizeDrift() {
    return offlineResult(QStringLiteral("Drift"),
                         QStringLiteral("Clock drift compensation optimization"));
}

DcSyncOptimizationResult DcSyncOptimizerService::optimizeJitter() {
    return offlineResult(QStringLiteral("Jitter"),
                         QStringLiteral("Jitter filtering and analysis optimization"));
}

DcSyncOptimizationResult DcSyncOptimizerService::optimizeConfiguration() {
    return offlineResult(QStringLiteral("Configuration"),
                         QStringLiteral("Overall DC configuration tuning"));
}

bool DcSyncOptimizerService::applyOptimization(
    const DcSyncOptimizationResult &result) {
    if (result.category.isEmpty())
        return false;
    if (!client_ || !client_->isConnected() || !hasDcTelemetry()) {
        emit error(QStringLiteral("Cannot apply DC sync optimization without live DC telemetry and backend acknowledgement"));
        return false;
    }
    return true;
}

void DcSyncOptimizerService::clearResults() { results_.clear(); }
