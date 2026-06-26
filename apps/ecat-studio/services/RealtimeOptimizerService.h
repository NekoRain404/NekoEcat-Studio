#pragma once

/// @brief Service for real-time performance optimization of the EtherCAT bus.
///
/// @details Provides latency optimization, throughput optimization, resource
/// optimization, and priority optimization request surfaces. Producing measured
/// before/after data or applying recommendations requires a real privileged
/// backend for scheduler, IRQ, CPU, and memory-lock configuration; this service
/// does not synthesize success.

#include "EtherCATOptimizerService.h"

#include <QObject>
#include <QDateTime>
#include <QVector>

class RealtimeOptimizerService : public QObject {
  Q_OBJECT
public:
  explicit RealtimeOptimizerService(QObject *parent = nullptr);

  OptimizationResult optimizeLatency();
  OptimizationResult optimizeThroughput();
  OptimizationResult optimizeResources();
  OptimizationResult optimizePriorities();

  // Returns false until a privileged host realtime execution backend is wired.
  bool applyOptimization(const OptimizationResult &result);

  QVector<OptimizationResult> optimizationHistory() const;
  void clearHistory();

signals:
  void optimizationCompleted(const OptimizationResult &result);
  void optimizationApplied(const OptimizationResult &result);

private:
  OptimizationResult createRejectedResult(const QString &category,
                                          const QStringList &recommendations) const;

  QVector<OptimizationResult> history_;
};
