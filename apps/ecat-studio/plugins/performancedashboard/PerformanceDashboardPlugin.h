#pragma once

// PerformanceDashboardPlugin — workspace plugin for real-time performance monitoring.
//
// Features:
//   - Startup time breakdown with phase durations
//   - Runtime performance metrics (SDO latency, state transitions, Free Run, UI)
//   - Memory usage tracking (services, plugins, caches)
//   - Historical performance charts
//   - Performance alerts with configurable thresholds
//   - Performance report generation
//
// UI Layout:
//   Tab 1: Overview — gauges for key metrics, alert status
//   Tab 2: Startup — phase breakdown, timing chart
//   Tab 3: Runtime — SDO latency, state transitions, Free Run, UI timing
//   Tab 4: Memory — service/plugin/cache memory breakdown
//   Tab 5: History — historical charts for all metrics
//   Tab 6: Reports — generate and view performance reports
//
// Default Order: 135 (appears after Dashboard)

#include "plugins/WorkspacePlugin.h"

class QTabWidget;
class QLabel;
class QTableWidget;
class QProgressBar;
class QTimer;
class PerformanceMonitorService;

class PerformanceDashboardPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit PerformanceDashboardPlugin(PerformanceMonitorService* service, QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;

public slots:
    void refresh();
    void generateReport();

private:
    void buildUi();
    void buildOverviewTab();
    void buildStartupTab();
    void buildRuntimeTab();
    void buildMemoryTab();
    void buildHistoryTab();
    void buildReportsTab();

    void updateOverview();
    void updateStartup();
    void updateRuntime();
    void updateMemory();
    void updateHistory();

    PerformanceMonitorService* service_;
    QWidget* container_ = nullptr;
    QTabWidget* tabs_ = nullptr;
    QTimer* refreshTimer_ = nullptr;

    // Overview tab
    QLabel* startupStatus_ = nullptr;
    QLabel* totalStartupTime_ = nullptr;
    QLabel* sdoReadLatencyLabel_ = nullptr;
    QLabel* sdoWriteLatencyLabel_ = nullptr;
    QLabel* stateTransitionLabel_ = nullptr;
    QLabel* freeRunCycleLabel_ = nullptr;
    QLabel* uiUpdateLabel_ = nullptr;
    QLabel* memoryUsageLabel_ = nullptr;
    QTableWidget* alertsTable_ = nullptr;

    // Startup tab
    QTableWidget* startupPhasesTable_ = nullptr;
    QLabel* startupTotalLabel_ = nullptr;

    // Runtime tab
    QTableWidget* runtimeTable_ = nullptr;

    // Memory tab
    QTableWidget* serviceMemoryTable_ = nullptr;
    QTableWidget* pluginMemoryTable_ = nullptr;
    QTableWidget* cacheMemoryTable_ = nullptr;
    QLabel* totalMemoryLabel_ = nullptr;

    // History tab
    QTableWidget* historyTable_ = nullptr;

    // Reports tab
    QLabel* reportLabel_ = nullptr;
};
