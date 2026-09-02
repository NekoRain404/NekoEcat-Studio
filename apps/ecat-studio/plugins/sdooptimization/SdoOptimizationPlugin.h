#pragma once

// SdoOptimizationPlugin — SDO communication optimization.
//
// Workspace plugin providing cache optimization, batch optimization,
// performance optimization, and error handling optimization for
// EtherCAT SDO transfers.
//
// Plugin Identity:
//   id: "sdooptimization"
//   displayName: "SDO Optimization"
//   displayNameZh: "SDO 优化"
//   defaultOrder: 48

#include "plugins/WorkspacePlugin.h"
#include "services/SdoOptimizationService.h"

class QTabWidget;
class QPushButton;
class QLabel;
class EcatClient;
class EventBus;
class CacheOptimizerWidget;
class BatchOptimizerWidget;

class SdoOptimizationPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit SdoOptimizationPlugin(EcatClient* client, EventBus* bus, QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;
    void onConnectionChanged(bool connected) override;

    CacheOptimizerWidget* cacheOptimizer() const { return cacheWidget_; }
    BatchOptimizerWidget* batchOptimizer() const { return batchWidget_; }
    QPushButton* exportButton() const { return exportBtn_; }
    bool exportReportToFile(const QString& path);

private slots:
    void handleCacheOptimize();
    void handleBatchOptimize();
    void handlePerformanceOptimize();
    void handleErrorHandlingOptimize();
    void handleOptimizationCompleted(const SdoOptimizationResult& result);
    void handleOptimizationApplied(const SdoOptimizationResult& result);
    void handleExport();

private:
    void buildUi();
    QWidget* buildCacheTab();
    QWidget* buildBatchTab();
    QWidget* buildPerformanceTab();
    QWidget* buildErrorHandlerTab();

    SdoOptimizationService* service_;
    QWidget* containerWidget_ = nullptr;
    QTabWidget* tabs_ = nullptr;

    CacheOptimizerWidget* cacheWidget_ = nullptr;
    BatchOptimizerWidget* batchWidget_ = nullptr;

    QLabel* perfThroughputLabel_ = nullptr;
    QLabel* perfLatencyLabel_ = nullptr;
    QLabel* perfCpuLabel_ = nullptr;
    QPushButton* perfOptimizeBtn_ = nullptr;

    QLabel* errorRecoveryLabel_ = nullptr;
    QLabel* errorRetryLabel_ = nullptr;
    QLabel* errorRateLabel_ = nullptr;
    QPushButton* errorOptimizeBtn_ = nullptr;

    QLabel* statusLabel_ = nullptr;
    QPushButton* exportBtn_ = nullptr;
};
