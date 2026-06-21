#pragma once

// WorkflowOptimizationService — optimizes task scheduling, resource allocation,
// parallel execution, and dependency resolution for workflow pipelines.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QJsonObject>

struct WfTask {
  QString id;
  QString name;
  int priority = 0;
  double estimatedDurationMs = 0.0;
  QStringList dependencies;
  QStringList requiredResources;
};

struct WfResource {
  QString id;
  QString name;
  double capacity = 1.0;
  double currentLoad = 0.0;
  QStringList capabilities;
};

struct OptimizedSchedule {
  QVector<WfTask> tasks;
  QStringList order;
  double estimatedDurationMs = 0.0;
  QJsonObject resourceUsage;
  double parallelism = 1.0;
  QStringList bottlenecks;
};

struct AllocationPlan {
  QJsonObject allocations;
  double utilization = 0.0;
  QStringList conflicts;
  QStringList recommendations;
};

struct ExecutionPlan {
  QVector<QStringList> stages;
  double speedupRatio = 1.0;
  int maxConcurrency = 1;
  QStringList criticalPath;
};

struct DependencyGraph {
  QJsonObject nodes;
  QJsonObject edges;
  QStringList topologicalOrder;
  bool hasCycles = false;
  QStringList cycles;
};

struct WorkflowOptimizationResult {
  QString category;
  QString description;
  double improvementPercent = 0.0;
  QStringList recommendations;
  QJsonObject metrics;
};

class WorkflowOptimizationService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowOptimizationService(QObject *parent = nullptr);

  OptimizedSchedule optimizeTaskSchedule(const QVector<WfTask> &tasks);
  AllocationPlan optimizeResourceAllocation(const QVector<WfResource> &resources,
                                            const QVector<WfTask> &tasks);
  ExecutionPlan optimizeParallelExecution(const QVector<WfTask> &tasks);
  DependencyGraph resolveDependencies(const QVector<WfTask> &tasks);

signals:
  void optimizationCompleted(const WorkflowOptimizationResult &result);

private:
  void emitResult(const QString &category, const QString &description,
                  double improvement, const QStringList &recommendations,
                  const QJsonObject &metrics);
};
