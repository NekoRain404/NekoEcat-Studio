#pragma once

// FreeRunOptimizationPlugin — Free Run process data exchange optimization.
//
// Workspace plugin providing cycle time optimization, data mapping optimization,
// performance optimization, and error handling optimization for EtherCAT Free Run.
//
// Plugin Identity:
//   id: "freerunoptimization"
//   displayName: "Free Run Optimization"
//   displayNameZh: "自由运行优化"
//   defaultOrder: 44

#include "plugins/WorkspacePlugin.h"

class QTabWidget;
class QPushButton;
class QLabel;
class EcatClient;
class EventBus;
class FreeRunOptimizationService;
class CycleTimeOptimizerWidget;
class DataMappingOptimizerWidget;
struct FreeRunOptimizationResult;

class FreeRunOptimizationPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit FreeRunOptimizationPlugin(EcatClient *client, EventBus *bus,
                                     QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;
  void onConnectionChanged(bool connected) override;

  CycleTimeOptimizerWidget *cycleTimeOptimizer() const { return cycleTimeWidget_; }
  DataMappingOptimizerWidget *dataMappingOptimizer() const { return dataMappingWidget_; }
  QPushButton *exportButton() const { return exportBtn_; }

private slots:
  void handleCycleTimeOptimize();
  void handleDataMappingOptimize();
  void handlePerformanceOptimize();
  void handleErrorHandlingOptimize();
  void handleOptimizationCompleted(const FreeRunOptimizationResult &result);
  void handleOptimizationApplied(const FreeRunOptimizationResult &result);
  void handleExport();

private:
  void buildUi();
  QWidget *buildCycleTimeTab();
  QWidget *buildDataMappingTab();
  QWidget *buildPerformanceTab();
  QWidget *buildErrorHandlerTab();

  FreeRunOptimizationService *service_;
  QWidget *containerWidget_ = nullptr;
  QTabWidget *tabs_ = nullptr;

  CycleTimeOptimizerWidget *cycleTimeWidget_ = nullptr;
  DataMappingOptimizerWidget *dataMappingWidget_ = nullptr;

  QLabel *perfCpuLabel_ = nullptr;
  QLabel *perfBusLabel_ = nullptr;
  QLabel *perfFrameLabel_ = nullptr;
  QPushButton *perfOptimizeBtn_ = nullptr;

  QLabel *errorRecoveryLabel_ = nullptr;
  QLabel *errorRetryLabel_ = nullptr;
  QLabel *errorRateLabel_ = nullptr;
  QPushButton *errorOptimizeBtn_ = nullptr;

  QLabel *statusLabel_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
};
