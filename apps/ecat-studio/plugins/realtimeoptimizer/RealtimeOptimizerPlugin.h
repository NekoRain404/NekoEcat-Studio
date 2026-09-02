#pragma once

/// @brief Workspace plugin for real-time performance optimization.
///
/// @details Provides an optimization dashboard with latency optimizer,
/// throughput optimizer, resource optimizer, and priority optimization.
/// Integrates LatencyOptimizerWidget and ThroughputOptimizerWidget.
///
/// @par Plugin Identity
///   - id: "realtimeoptimizer"
///   - defaultOrder: 38
///   - visible: always true

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QPushButton;
class QTabWidget;
class LatencyOptimizerWidget;
class ThroughputOptimizerWidget;
class RealtimeOptimizerService;

class RealtimeOptimizerPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit RealtimeOptimizerPlugin(RealtimeOptimizerService* service, QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QIcon icon() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;

    RealtimeOptimizerService* service() const { return service_; }
    LatencyOptimizerWidget* latencyOptimizer() const { return latencyOptimizer_; }
    ThroughputOptimizerWidget* throughputOptimizer() const { return throughputOptimizer_; }
    bool exportReportToFile(const QString& path);

private:
    void buildUi();
    void buildDashboardTab();
    void buildLatencyTab();
    void buildThroughputTab();
    void buildResourceTab();
    void exportReport();

    RealtimeOptimizerService* service_;
    QWidget* containerWidget_ = nullptr;
    QTabWidget* tabWidget_ = nullptr;

    // Dashboard
    QLabel* latencyImprovementLabel_ = nullptr;
    QLabel* throughputImprovementLabel_ = nullptr;
    QLabel* resourceImprovementLabel_ = nullptr;
    QLabel* priorityImprovementLabel_ = nullptr;

    // Latency
    LatencyOptimizerWidget* latencyOptimizer_ = nullptr;

    // Throughput
    ThroughputOptimizerWidget* throughputOptimizer_ = nullptr;

    // Resource
    QLabel* cpuOptLabel_ = nullptr;
    QLabel* memOptLabel_ = nullptr;
    QLabel* threadOptLabel_ = nullptr;
    QLabel* irqOptLabel_ = nullptr;

    // Controls
    QPushButton* optimizeAllBtn_ = nullptr;
    QPushButton* exportBtn_ = nullptr;
    QLabel* statusLabel_ = nullptr;
};
