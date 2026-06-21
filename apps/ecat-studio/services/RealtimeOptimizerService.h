#pragma once

/// @brief Service for real-time performance optimization of the EtherCAT bus.
///
/// @details Provides latency optimization, throughput optimization, resource
/// optimization, and priority optimization. Analyzes current performance metrics
/// and generates actionable optimization recommendations.

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

  bool applyOptimization(const OptimizationResult &result);

  QVector<OptimizationResult> optimizationHistory() const;
  void clearHistory();

signals:
  void optimizationCompleted(const OptimizationResult &result);
  void optimizationApplied(const OptimizationResult &result);

private:
  QVector<OptimizationResult> history_;
};
