#pragma once

// FreeRunOptimizationService — Free Run optimization for process data exchange.
//
// Provides Free Run optimization request surfaces. Until a real optimization
// backend is wired, requests return backend-required recommendations instead of
// synthetic measurements or success signals.
//
// Usage:
//   FreeRunOptimizationService *svc = new FreeRunOptimizationService(client, bus, this);
//   OptimizationResult result = svc->optimizeCycleTime();
//   svc->applyOptimization(result);

#include <QDateTime>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QVector>

class EcatClient;
class EventBus;

struct FreeRunOptimizationResult {
    QString category;
    QString description;
    QJsonObject before;
    QJsonObject after;
    double improvement = 0.0;
    QStringList recommendations;
    bool applied = false;
    QDateTime timestamp;
};

Q_DECLARE_METATYPE(FreeRunOptimizationResult)

class FreeRunOptimizationService : public QObject {
    Q_OBJECT
public:
    explicit FreeRunOptimizationService(EcatClient* client, EventBus* bus, QObject* parent = nullptr);

    FreeRunOptimizationResult optimizeCycleTime();
    FreeRunOptimizationResult optimizeDataMapping();
    FreeRunOptimizationResult optimizePerformance();
    FreeRunOptimizationResult optimizeErrorHandling();

    // Apply an optimization to live Free Run settings.
    // Returns false until a backend can confirm the change.
    bool applyOptimization(const FreeRunOptimizationResult& result);

    QVector<FreeRunOptimizationResult> optimizationHistory() const { return history_; }
    void clearHistory();

signals:
    void optimizationCompleted(const FreeRunOptimizationResult& result);
    void optimizationApplied(const FreeRunOptimizationResult& result);

private:
    FreeRunOptimizationResult createRejectedResult(const QString& category, const QStringList& recommendations);

    EcatClient* client_;
    EventBus* bus_;
    QVector<FreeRunOptimizationResult> history_;
};
