#pragma once

// DcSyncOptimizerPlugin — DC synchronization optimization.
//
// Workspace plugin providing sync optimization, drift optimization,
// jitter optimization, and configuration optimization for the
// EtherCAT Distributed Clock.
//
// Plugin Identity:
//   id: "dcsyncoptimizer"
//   displayName: "DC Sync Optimizer"
//   displayNameZh: "DC 同步优化器"
//   defaultOrder: 40

#include "plugins/WorkspacePlugin.h"
#include "services/DcSyncOptimizerService.h"

class QTabWidget;
class QLabel;
class QPushButton;
class QTableWidget;
class EcatClient;
class EventBus;
class DcSyncOptimizerService;
class SyncOptimizerWidget;
class DriftOptimizerWidget;

class DcSyncOptimizerPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit DcSyncOptimizerPlugin(EcatClient* client, EventBus* bus, QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;
    void onConnectionChanged(bool connected) override;

    DcSyncOptimizerService* service() const { return service_; }
    SyncOptimizerWidget* syncOptimizer() const { return syncWidget_; }
    DriftOptimizerWidget* driftOptimizer() const { return driftWidget_; }
    QTabWidget* tabs() const { return tabs_; }

private slots:
    void handleSyncOptimize();
    void handleDriftOptimize();
    void handleJitterOptimize();
    void handleConfigOptimize();
    void handleApplySync();
    void handleApplyDrift();
    void handleOptimizationCompleted(const DcSyncOptimizationResult& result);
    void handleOptimizationApplied(const DcSyncOptimizationResult& result);
    void handleExport();

private:
    void buildUi();
    QWidget* buildSyncTab();
    QWidget* buildDriftTab();
    QWidget* buildJitterTab();
    QWidget* buildConfigTab();

    DcSyncOptimizerService* service_;
    QWidget* containerWidget_ = nullptr;
    QTabWidget* tabs_ = nullptr;

    SyncOptimizerWidget* syncWidget_ = nullptr;
    DriftOptimizerWidget* driftWidget_ = nullptr;

    QWidget* jitterWidget_ = nullptr;
    QTableWidget* jitterParamsTable_ = nullptr;
    QLabel* jitterImprovementLabel_ = nullptr;
    QLabel* jitterRecommendationsLabel_ = nullptr;
    QPushButton* jitterOptimizeBtn_ = nullptr;
    QPushButton* jitterApplyBtn_ = nullptr;

    QWidget* configWidget_ = nullptr;
    QTableWidget* configParamsTable_ = nullptr;
    QLabel* configImprovementLabel_ = nullptr;
    QLabel* configRecommendationsLabel_ = nullptr;
    QPushButton* configOptimizeBtn_ = nullptr;
    QPushButton* configApplyBtn_ = nullptr;

    QPushButton* exportBtn_ = nullptr;

    DcSyncOptimizationResult lastSyncResult_;
    DcSyncOptimizationResult lastDriftResult_;
    DcSyncOptimizationResult lastJitterResult_;
    DcSyncOptimizationResult lastConfigResult_;
};
