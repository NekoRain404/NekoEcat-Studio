#pragma once

// PdoMappingOptimizationPlugin — PDO mapping optimization for NekoEcat Studio.
//
// Workspace plugin providing mapping optimization, size optimization,
// alignment optimization, and performance optimization for PDO configurations.
//
// Plugin Identity:
//   id: "pdomappingoptimization"
//   displayName: "PDO Mapping Optimization"
//   displayNameZh: "PDO 映射优化"
//   defaultOrder: 46

#include "plugins/WorkspacePlugin.h"
#include "services/PdoMappingOptimizationService.h"

class QTabWidget;
class QPushButton;
class QLabel;
class MappingOptimizerWidget;
class SizeOptimizerWidget;

class PdoMappingOptimizationPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit PdoMappingOptimizationPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;
  void onConnectionChanged(bool connected) override;

  PdoMappingOptimizationService *service() const { return service_; }
  MappingOptimizerWidget *mappingOptimizer() const { return mappingWidget_; }
  SizeOptimizerWidget *sizeOptimizer() const { return sizeWidget_; }
  QPushButton *exportButton() const { return exportBtn_; }

private slots:
  void handleMappingOptimize();
  void handleSizeOptimize();
  void handleAlignmentOptimize();
  void handlePerformanceOptimize();
  void handleOptimizationCompleted(const PdoMappingOptimizationResult &result);
  void handleOptimizationApplied(const PdoMappingOptimizationResult &result);
  void handleExport();

private:
  void buildUi();
  QWidget *buildMappingTab();
  QWidget *buildSizeTab();
  QWidget *buildAlignmentTab();
  QWidget *buildPerformanceTab();

  PdoMappingOptimizationService *service_;
  QWidget *containerWidget_ = nullptr;
  QTabWidget *tabs_ = nullptr;

  MappingOptimizerWidget *mappingWidget_ = nullptr;
  SizeOptimizerWidget *sizeWidget_ = nullptr;

  QLabel *alignMisalignLabel_ = nullptr;
  QLabel *alignPaddingLabel_ = nullptr;
  QLabel *alignCrossLabel_ = nullptr;
  QPushButton *alignOptimizeBtn_ = nullptr;

  QLabel *perfCycleLabel_ = nullptr;
  QLabel *perfBusLabel_ = nullptr;
  QLabel *perfThroughputLabel_ = nullptr;
  QPushButton *perfOptimizeBtn_ = nullptr;

  QLabel *statusLabel_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
};
