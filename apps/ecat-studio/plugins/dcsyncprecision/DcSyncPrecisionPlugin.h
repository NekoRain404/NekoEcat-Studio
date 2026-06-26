#pragma once

// DcSyncPrecisionPlugin — hardware-verified DC synchronization precision.
//
// Workspace plugin providing sync status, drift monitoring, jitter analysis,
// and configuration for the EtherCAT Distributed Clock.
//
// Plugin Identity:
//   id: "dcsyncprecision"
//   displayName: "DC Sync Precision"
//   displayNameZh: "DC 同步精度"
//   defaultOrder: 26

#include "plugins/WorkspacePlugin.h"
#include "services/DcSyncPrecisionService.h"

class QTabWidget;
class QTableWidget;
class QLabel;
class QPushButton;
class QDoubleSpinBox;
class QSpinBox;
class QComboBox;
class EcatClient;
class EventBus;
class DriftMonitorWidget;
class JitterAnalysisWidget;

class DcSyncPrecisionPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit DcSyncPrecisionPlugin(EcatClient *client, EventBus *bus,
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

  QWidget *syncStatusWidget() const { return syncStatusWidget_; }
  DriftMonitorWidget *driftMonitor() const { return driftMonitor_; }
  JitterAnalysisWidget *jitterAnalysis() const { return jitterAnalysis_; }
  QTableWidget *syncTable() const { return syncTable_; }
  QPushButton *startStopButton() const { return startStopBtn_; }
  QPushButton *exportButton() const { return exportBtn_; }
  bool exportReportToFile(const QString &path);

private slots:
  void handleStartStop();
  void handleExport();
  void handleDriftUpdated(const DriftStatusEx &status);
  void handleJitterUpdated(const JitterStatsEx &stats);
  void handleSyncQualityChanged(const SyncQuality &quality);
  void handleMonitoringStateChanged(bool active);
  void handleThresholdChanged(double value);
  void handleHistoryWindowChanged(int value);

private:
  void buildUi();
  QWidget *buildSyncStatusTab();
  QWidget *buildDriftMonitorTab();
  QWidget *buildJitterAnalysisTab();
  QWidget *buildConfigTab();
  void updateSyncStatusTable();
  void updateQualityDisplay(const SyncQuality &q);

  DcSyncPrecisionService *service_;
  QWidget *containerWidget_ = nullptr;
  QTabWidget *tabs_ = nullptr;

  // Sync status tab
  QWidget *syncStatusWidget_ = nullptr;
  QTableWidget *syncTable_ = nullptr;
  QLabel *qualityLabel_ = nullptr;
  QLabel *qualityScoreLabel_ = nullptr;
  QLabel *refClockLabel_ = nullptr;

  // Drift monitor tab
  DriftMonitorWidget *driftMonitor_ = nullptr;

  // Jitter analysis tab
  JitterAnalysisWidget *jitterAnalysis_ = nullptr;

  // Config tab
  QDoubleSpinBox *thresholdSpin_ = nullptr;
  QSpinBox *historyWindowSpin_ = nullptr;
  QComboBox *pollIntervalCombo_ = nullptr;

  // Controls
  QPushButton *startStopBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
};
