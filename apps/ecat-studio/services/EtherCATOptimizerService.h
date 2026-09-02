#pragma once

// EtherCATOptimizerService — performance optimization for EtherCAT
// configuration, timing, buffers, and priorities.
//
// Provides on-demand optimization request methods. Without a connected
// optimization backend, requests return rejected results with recommendations
// and no synthetic before/after measurements.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

class EcatClient;
class EventBus;

struct OptimizationResult {
    QString category;
    QString description;
    double before = 0.0;
    double after = 0.0;
    double improvement = 0.0;
    QStringList recommendations;
};

class EtherCATOptimizerService : public QObject {
    Q_OBJECT
public:
    explicit EtherCATOptimizerService(EventBus* bus, EcatClient* client, QObject* parent = nullptr);

    OptimizationResult optimizeConfiguration();
    OptimizationResult optimizeTiming();
    OptimizationResult optimizeBuffers();
    OptimizationResult optimizePriorities();

signals:
    void optimizationCompleted(const OptimizationResult& result);

private:
    OptimizationResult makeRejectedResult(const QString& category, const QStringList& recommendations);

    EventBus* bus_;
    EcatClient* client_;
};
