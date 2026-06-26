#include "EtherCATOptimizerService.h"
#include "infra/EcatClient.h"
#include "EventBus.h"

// EtherCATOptimizerService.cpp — Targeted optimization request facade
//
// Implementation notes:
//   - Four areas: configuration, timing, buffers, traffic priorities
//   - Fails closed until wired to a backend that can measure before/after values
//   - Recommendations cover addressing, DC sync, buffer sizing, and thread affinity

EtherCATOptimizerService::EtherCATOptimizerService(EventBus *bus, EcatClient *client,
                                                   QObject *parent)
    : QObject(parent), bus_(bus), client_(client)
{
}

OptimizationResult EtherCATOptimizerService::makeRejectedResult(
    const QString &category, const QStringList &recommendations)
{
    OptimizationResult r;
    r.category = category;
    r.description = QStringLiteral(
        "%1 optimization requires a connected EtherCAT optimization backend")
                        .arg(category);
    r.recommendations = recommendations;
    return r;
}

OptimizationResult EtherCATOptimizerService::optimizeConfiguration()
{
    QStringList recs;
    recs << QStringLiteral("Enable auto-increment addressing for faster discovery");
    recs << QStringLiteral("Remove unused slave configurations to reduce scan time");
    recs << QStringLiteral("Use distributed clocks for synchronized operations");
    recs << QStringLiteral("Minimize SDO transfers during cyclic operation");

    return makeRejectedResult(QStringLiteral("Configuration"), recs);
}

OptimizationResult EtherCATOptimizerService::optimizeTiming()
{
    QStringList recs;
    recs << QStringLiteral("Reduce jitter by increasing thread priority");
    recs << QStringLiteral("Consider shorter cycle times for tighter control");
    recs << QStringLiteral("Use DC synchronization for deterministic timing");
    recs << QStringLiteral("Enable interrupt-based instead of polling mode");

    return makeRejectedResult(QStringLiteral("Timing"), recs);
}

OptimizationResult EtherCATOptimizerService::optimizeBuffers()
{
    QStringList recs;
    recs << QStringLiteral("Increase rx/tx buffer count to reduce frame loss");
    recs << QStringLiteral("Reduce payload size or increase bandwidth allocation");
    recs << QStringLiteral("Use datagram-level flow control");
    recs << QStringLiteral("Align buffer sizes to cache line boundaries");

    return makeRejectedResult(QStringLiteral("Buffers"), recs);
}

OptimizationResult EtherCATOptimizerService::optimizePriorities()
{
    QStringList recs;
    recs << QStringLiteral("Increase SDO queue depth for better throughput");
    recs << QStringLiteral("Prioritize PDO over SDO for real-time data");
    recs << QStringLiteral("Use separate threads for cyclic and acyclic traffic");
    recs << QStringLiteral("Apply CPU affinity for EtherCAT master thread");

    return makeRejectedResult(QStringLiteral("Priorities"), recs);
}
