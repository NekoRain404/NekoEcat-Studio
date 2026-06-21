#include "WorkflowOptimizationService.h"
#include <QJsonArray>
#include <QSet>
#include <QMap>
#include <QQueue>
#include <algorithm>

// WorkflowOptimizationService.cpp — Optimizes workflow task scheduling, allocation, and parallelism
//
// Implementation notes:
//   - Uses Kahn's topological sort with priority-based tie-breaking for task scheduling
//   - Resource allocation detects overloads and computes utilization percentage
//   - Parallel execution identifies critical path and calculates speedup ratio vs sequential

WorkflowOptimizationService::WorkflowOptimizationService(QObject *parent)
    : QObject(parent)
{
}

void WorkflowOptimizationService::emitResult(const QString &category,
                                              const QString &description,
                                              double improvement,
                                              const QStringList &recommendations,
                                              const QJsonObject &metrics)
{
    WorkflowOptimizationResult r;
    r.category = category;
    r.description = description;
    r.improvementPercent = improvement;
    r.recommendations = recommendations;
    r.metrics = metrics;
    emit optimizationCompleted(r);
}

static QJsonObject resourceUsageMap(const QVector<WfTask> &tasks)
{
    QJsonObject usage;
    for (const auto &t : tasks) {
        for (const auto &res : t.requiredResources) {
            double current = usage.value(res).toDouble();
            usage[res] = current + t.estimatedDurationMs;
        }
    }
    return usage;
}

OptimizedSchedule WorkflowOptimizationService::optimizeTaskSchedule(
    const QVector<WfTask> &tasks)
{
    OptimizedSchedule schedule;
    schedule.tasks = tasks;

    QSet<QString> taskIds;
    QMap<QString, int> inDegree;
    QMap<QString, QStringList> adjacency;

    for (const auto &t : tasks) {
        taskIds.insert(t.id);
        inDegree[t.id] = 0;
    }
    for (const auto &t : tasks) {
        for (const auto &dep : t.dependencies) {
            if (taskIds.contains(dep)) {
                adjacency[dep].append(t.id);
                inDegree[t.id]++;
            }
        }
    }

    // Kahn's topological sort with priority ordering
    QQueue<QString> queue;
    for (auto it = inDegree.begin(); it != inDegree.end(); ++it) {
        if (it.value() == 0)
            queue.enqueue(it.key());
    }

    // Sort queue by priority (higher first) for tie-breaking
    auto priorityOf = [&tasks](const QString &id) -> int {
        for (const auto &t : tasks)
            if (t.id == id) return t.priority;
        return 0;
    };

    QStringList order;
    double totalDuration = 0.0;
    double maxStageTime = 0.0;
    int stage = 0;

    while (!queue.isEmpty()) {
        // Sort current level by priority
        QStringList level;
        int levelSize = queue.size();
        for (int i = 0; i < levelSize; ++i)
            level.append(queue.dequeue());

        std::sort(level.begin(), level.end(),
                  [&priorityOf](const QString &a, const QString &b) {
                      return priorityOf(a) > priorityOf(b);
                  });

        double stageTime = 0.0;
        for (const auto &id : level) {
            for (const auto &t : tasks) {
                if (t.id == id) {
                    stageTime = qMax(stageTime, t.estimatedDurationMs);
                    break;
                }
            }
        }

        // Process level
        for (int i = 0; i < level.size(); ++i) {
            QString current = level[i];
            order.append(current);

            if (adjacency.contains(current)) {
                for (const auto &neighbor : adjacency[current]) {
                    inDegree[neighbor]--;
                    if (inDegree[neighbor] == 0)
                        queue.enqueue(neighbor);
                }
            }
        }

        maxStageTime += stageTime;
        stage++;
    }

    schedule.order = order;
    schedule.estimatedDurationMs = maxStageTime;
    schedule.resourceUsage = resourceUsageMap(tasks);
    schedule.parallelism = (stage > 0) ? static_cast<double>(tasks.size()) / stage : 1.0;

    // Identify bottlenecks (tasks with most dependents)
    QStringList bottlenecks;
    for (auto it = adjacency.begin(); it != adjacency.end(); ++it) {
        if (it.value().size() >= 2)
            bottlenecks.append(it.key());
    }
    schedule.bottlenecks = bottlenecks;

    QStringList recs;
    if (!bottlenecks.isEmpty())
        recs << QStringLiteral("Consider splitting bottleneck tasks: %1").arg(bottlenecks.join(", "));
    if (schedule.parallelism < 2.0)
        recs << QStringLiteral("Low parallelism detected — review task dependencies");

    emitResult(QStringLiteral("TaskSchedule"),
               QStringLiteral("Task scheduling optimization"),
               (tasks.size() > order.size()) ? 0.0 : 15.0,
               recs, schedule.resourceUsage);

    return schedule;
}

AllocationPlan WorkflowOptimizationService::optimizeResourceAllocation(
    const QVector<WfResource> &resources, const QVector<WfTask> &tasks)
{
    AllocationPlan plan;

    QJsonObject allocations;
    QJsonObject load;
    for (const auto &r : resources)
        load[r.id] = r.currentLoad;

    for (const auto &t : tasks) {
        QJsonArray assigned;
        for (const auto &req : t.requiredResources) {
            for (const auto &r : resources) {
                if (r.id == req || r.capabilities.contains(req)) {
                    assigned.append(r.id);
                    load[r.id] = load[r.id].toDouble() + t.estimatedDurationMs;
                    break;
                }
            }
        }
        allocations[t.id] = assigned;
    }

    plan.allocations = allocations;

    // Calculate utilization
    double totalCap = 0.0;
    double totalLoad = 0.0;
    for (const auto &r : resources) {
        totalCap += r.capacity;
        totalLoad += load[r.id].toDouble();
    }
    plan.utilization = (totalCap > 0.0) ? (totalLoad / totalCap) * 100.0 : 0.0;

    // Detect conflicts
    QStringList conflicts;
    for (const auto &r : resources) {
        if (load[r.id].toDouble() > r.capacity * 100.0)
            conflicts.append(QStringLiteral("Resource %1 overloaded").arg(r.id));
    }
    plan.conflicts = conflicts;

    QStringList recs;
    if (!conflicts.isEmpty())
        recs << QStringLiteral("Reduce load on overloaded resources or add capacity");
    if (plan.utilization > 80.0)
        recs << QStringLiteral("High resource utilization — consider load balancing");

    emitResult(QStringLiteral("ResourceAllocation"),
               QStringLiteral("Resource allocation optimization"),
               conflicts.isEmpty() ? 20.0 : 5.0, recs, allocations);

    return plan;
}

ExecutionPlan WorkflowOptimizationService::optimizeParallelExecution(
    const QVector<WfTask> &tasks)
{
    ExecutionPlan plan;

    QSet<QString> taskIds;
    QMap<QString, int> inDegree;
    QMap<QString, QStringList> adjacency;
    QMap<QString, double> taskDuration;

    for (const auto &t : tasks) {
        taskIds.insert(t.id);
        inDegree[t.id] = 0;
        taskDuration[t.id] = t.estimatedDurationMs;
    }
    for (const auto &t : tasks) {
        for (const auto &dep : t.dependencies) {
            if (taskIds.contains(dep)) {
                adjacency[dep].append(t.id);
                inDegree[t.id]++;
            }
        }
    }

    // Level-based parallel execution scheduling
    QQueue<QString> queue;
    for (auto it = inDegree.begin(); it != inDegree.end(); ++it) {
        if (it.value() == 0)
            queue.enqueue(it.key());
    }

    QStringList processed;
    double criticalPathDuration = 0.0;
    QStringList criticalPath;

    while (!queue.isEmpty()) {
        QStringList stage;
        int levelSize = queue.size();
        for (int i = 0; i < levelSize; ++i)
            stage.append(queue.dequeue());

        plan.stages.append(stage);
        plan.maxConcurrency = qMax(plan.maxConcurrency, stage.size());

        double stageMaxDuration = 0.0;
        QString stageCritical;
        for (const auto &id : stage) {
            double dur = taskDuration.value(id, 0.0);
            if (dur > stageMaxDuration) {
                stageMaxDuration = dur;
                stageCritical = id;
            }
            processed.append(id);

            if (adjacency.contains(id)) {
                for (const auto &neighbor : adjacency[id]) {
                    inDegree[neighbor]--;
                    if (inDegree[neighbor] == 0)
                        queue.enqueue(neighbor);
                }
            }
        }
        criticalPathDuration += stageMaxDuration;
        if (!stageCritical.isEmpty())
            criticalPath.append(stageCritical);
    }

    plan.criticalPath = criticalPath;

    // Calculate speedup vs sequential
    double sequentialDuration = 0.0;
    for (const auto &t : tasks)
        sequentialDuration += t.estimatedDurationMs;

    plan.speedupRatio = (criticalPathDuration > 0.0)
                            ? sequentialDuration / criticalPathDuration
                            : 1.0;

    QStringList recs;
    if (plan.speedupRatio < 2.0)
        recs << QStringLiteral("Consider breaking dependencies to increase parallelism");
    if (plan.stages.size() > 1)
        recs << QStringLiteral("Critical path: %1").arg(criticalPath.join(" → "));

    QJsonObject metrics;
    metrics[QStringLiteral("stages")] = static_cast<int>(plan.stages.size());
    metrics[QStringLiteral("maxConcurrency")] = plan.maxConcurrency;
    metrics[QStringLiteral("speedupRatio")] = plan.speedupRatio;

    emitResult(QStringLiteral("ParallelExecution"),
               QStringLiteral("Parallel execution optimization"),
               (plan.speedupRatio - 1.0) * 100.0, recs, metrics);

    return plan;
}

DependencyGraph WorkflowOptimizationService::resolveDependencies(
    const QVector<WfTask> &tasks)
{
    DependencyGraph graph;

    QSet<QString> taskIds;
    QMap<QString, int> inDegree;
    QMap<QString, QStringList> adjacency;

    for (const auto &t : tasks) {
        taskIds.insert(t.id);
        inDegree[t.id] = 0;
    }
    for (const auto &t : tasks) {
        for (const auto &dep : t.dependencies) {
            if (taskIds.contains(dep)) {
                adjacency[dep].append(t.id);
                inDegree[t.id]++;
            }
        }
    }

    // Build JSON representation
    QJsonObject nodes;
    for (const auto &t : tasks) {
        QJsonObject node;
        node[QStringLiteral("name")] = t.name;
        node[QStringLiteral("inDegree")] = inDegree[t.id];
        node[QStringLiteral("outDegree")] = adjacency.value(t.id).size();
        nodes[t.id] = node;
    }
    graph.nodes = nodes;

    QJsonObject edges;
    for (const auto &t : tasks) {
        if (adjacency.contains(t.id)) {
            QJsonArray targets;
            for (const auto &target : adjacency[t.id])
                targets.append(target);
            edges[t.id] = targets;
        }
    }
    graph.edges = edges;

    // Kahn's algorithm for topological sort + cycle detection
    QQueue<QString> queue;
    for (auto it = inDegree.begin(); it != inDegree.end(); ++it) {
        if (it.value() == 0)
            queue.enqueue(it.key());
    }

    QStringList topoOrder;
    while (!queue.isEmpty()) {
        QString current = queue.dequeue();
        topoOrder.append(current);

        if (adjacency.contains(current)) {
            for (const auto &neighbor : adjacency[current]) {
                inDegree[neighbor]--;
                if (inDegree[neighbor] == 0)
                    queue.enqueue(neighbor);
            }
        }
    }

    graph.topologicalOrder = topoOrder;
    graph.hasCycles = (topoOrder.size() != tasks.size());

    // Detect cycle members
    if (graph.hasCycles) {
        QStringList cycleMembers;
        for (auto it = inDegree.begin(); it != inDegree.end(); ++it) {
            if (it.value() > 0)
                cycleMembers.append(it.key());
        }
        graph.cycles = cycleMembers;
    }

    QStringList recs;
    if (graph.hasCycles)
        recs << QStringLiteral("Circular dependency detected among: %1").arg(graph.cycles.join(", "));

    QJsonObject metrics;
    metrics[QStringLiteral("nodeCount")] = static_cast<int>(tasks.size());
    metrics[QStringLiteral("topoOrderSize")] = static_cast<int>(topoOrder.size());
    metrics[QStringLiteral("hasCycles")] = graph.hasCycles;

    emitResult(QStringLiteral("DependencyResolution"),
               QStringLiteral("Dependency graph analysis"),
               graph.hasCycles ? 0.0 : 100.0, recs, metrics);

    return graph;
}
