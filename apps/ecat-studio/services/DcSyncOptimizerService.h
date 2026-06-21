#pragma once

// DcSyncOptimizerService — optimizes Distributed Clock (DC) sync parameters.
//
// Analyzes and optimizes DC synchronization, drift compensation, jitter
// reduction, and configuration tuning for EtherCAT networks.
//
// DEMO STUB — This service generates synthetic data for UI demonstration.
// Replace with real hardware integration for production use.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include <QDateTime>

class EcatClient;
class EventBus;

struct DcSyncOptimizationResult {
    QString category;
    QString description;
    QJsonObject before;
    QJsonObject after;
    double improvement = 0.0;
    QStringList recommendations;
    bool applied = false;
    QDateTime timestamp;
};

class DcSyncOptimizerService : public QObject {
    Q_OBJECT
public:
    explicit DcSyncOptimizerService(EcatClient *client, EventBus *eventBus,
                                    QObject *parent = nullptr);

    DcSyncOptimizationResult optimizeSync();
    DcSyncOptimizationResult optimizeDrift();
    DcSyncOptimizationResult optimizeJitter();
    DcSyncOptimizationResult optimizeConfiguration();
    bool applyOptimization(const DcSyncOptimizationResult &result);

    QVector<DcSyncOptimizationResult> pendingResults() const { return results_; }
    void clearResults();

signals:
    void optimizationCompleted(const DcSyncOptimizationResult &result);
    void optimizationApplied(const DcSyncOptimizationResult &result);
    void error(const QString &message);

private:
    QJsonObject collectSyncStatus() const;
    QJsonObject analyzeSyncParameters(const QJsonObject &status) const;
    QJsonObject analyzeDriftParameters(const QJsonObject &status) const;
    QJsonObject analyzeJitterParameters(const QJsonObject &status) const;
    QJsonObject analyzeConfiguration(const QJsonObject &status) const;

    EcatClient *client_;
    EventBus *eventBus_;
    QVector<DcSyncOptimizationResult> results_;
};
