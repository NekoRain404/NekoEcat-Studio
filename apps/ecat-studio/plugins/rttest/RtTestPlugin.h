#pragma once
// RtTestPlugin — real-time stability test workspace plugin.
// Extracted from MainWindowRtTestWorkspace.cpp into the plugin architecture.
// Provides controls, live statistics, latency chart, and jitter sparkline
// for EtherCAT cycle-timing stability testing.

#include "plugins/WorkspacePlugin.h"

#include <QJsonObject>

class ServiceContainer;
class EcatClient;

class QPushButton;
class QComboBox;
class QLineEdit;
class QLabel;
class QPlainTextEdit;

class RtTestLatencyChart;
class RtTestJitterSpark;

class RtTestPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit RtTestPlugin(ServiceContainer *container,
                        QObject *parent = nullptr);

  // WorkspacePlugin identity
  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  // Lifecycle
  void activate() override;
  void deactivate() override;
  void onSettingsChanged(const AppSettings &settings) override;
  void onConnectionChanged(bool connected) override;

  static QString formatDuration(double seconds);

private slots:
  void handleRtTestTelemetry(const QJsonObject &telemetry);

private:
  void buildUi();
  void updateFreqLabel();
  void updateActionAvailability();

  ServiceContainer *container_;
  EcatClient *client_;
  QWidget *containerWidget_ = nullptr;
  bool running_ = false;

  // Control bar
  QPushButton *startButton_ = nullptr;
  QPushButton *stopButton_ = nullptr;
  QComboBox *cycleCombo_ = nullptr;
  QLineEdit *customCycle_ = nullptr;
  QLabel *freqLabel_ = nullptr;
  QLabel *statusLabel_ = nullptr;

  // Metrics
  QLabel *minLabel_ = nullptr;
  QLabel *maxLabel_ = nullptr;
  QLabel *avgLabel_ = nullptr;
  QLabel *jitterLabel_ = nullptr;
  QLabel *cyclesLabel_ = nullptr;
  QLabel *errorsLabel_ = nullptr;
  QLabel *lossLabel_ = nullptr;
  QLabel *durationLabel_ = nullptr;
  QLabel *healthLabel_ = nullptr;

  // Chart & sparkline
  RtTestLatencyChart *chart_ = nullptr;
  RtTestJitterSpark *jitterSpark_ = nullptr;

  // Timeline log
  QPlainTextEdit *timelineText_ = nullptr;
};
