#pragma once

// DcSyncPrecisionService — hardware-verified DC synchronization precision monitoring.
//
// Provides reference clock tracking, drift measurement, jitter analysis,
// and sync quality assessment for the EtherCAT Distributed Clock.
//
// Usage:
//   DcSyncPrecisionService *svc = new DcSyncPrecisionService(client, eventBus, this);
//   svc->startMonitoring();
//   int ref = svc->referenceClock();
//   DriftStatusEx drift = svc->driftStatus();
//   JitterStatsEx jitter = svc->jitterStatistics();
//   SyncQuality quality = svc->syncQuality();

#include <QObject>
#include <QJsonObject>
#include <QVector>
#include <QMap>
#include <QTimer>

class EcatClient;
class EventBus;

// Extended drift status with history.
struct DriftStatusEx {
  int slave = -1;
  double drift = 0.0;
  double threshold = 1000.0;
  QString status;
  qint64 timestamp = 0;
  QVector<double> history;
};

// Extended jitter statistics with histogram.
struct JitterStatsEx {
  double min = 0.0;
  double max = 0.0;
  double avg = 0.0;
  double stddev = 0.0;
  int sampleCount = 0;
  int windowSize = 1000;
  QVector<int> histogram;
  double histogramBinWidth = 10.0;
};

// Sync quality assessment.
struct SyncQuality {
  double score = 100.0;
  QString grade;
  int totalSlaves = 0;
  int syncedSlaves = 0;
  int warningSlaves = 0;
  int errorSlaves = 0;
  double maxDrift = 0.0;
  double avgJitter = 0.0;
};

class DcSyncPrecisionService : public QObject {
  Q_OBJECT
public:
  explicit DcSyncPrecisionService(EcatClient *client, EventBus *bus,
                                  QObject *parent = nullptr);

  void startMonitoring();
  void stopMonitoring();
  bool isMonitoring() const { return monitoring_; }

  int referenceClock() const { return refClock_; }
  DriftStatusEx driftStatus() const { return driftStatus_; }
  JitterStatsEx jitterStatistics() const { return jitterStats_; }
  SyncQuality syncQuality() const { return syncQuality_; }

  QVector<DriftStatusEx> slaveDrifts() const { return slaveDrifts_; }

  void setDriftThreshold(double ns);
  double driftThreshold() const { return driftThreshold_; }

  void setHistoryWindow(int samples);
  int historyWindow() const { return historyWindow_; }

signals:
  void referenceClockChanged(int position);
  void driftUpdated(const DriftStatusEx &status);
  void jitterUpdated(const JitterStatsEx &stats);
  void syncQualityChanged(const SyncQuality &quality);
  void monitoringStateChanged(bool active);

private slots:
  void pollSyncData();
  void handleDcSyncUpdate(const QJsonObject &data);

private:
  void processData(const QJsonObject &data);
  void computeJitterStats();
  void computeSyncQuality();

  EcatClient *client_;
  EventBus *bus_;
  QTimer *pollTimer_ = nullptr;
  bool monitoring_ = false;
  int refClock_ = -1;
  double driftThreshold_ = 1000.0;
  int historyWindow_ = 500;

  DriftStatusEx driftStatus_;
  JitterStatsEx jitterStats_;
  SyncQuality syncQuality_;
  QVector<DriftStatusEx> slaveDrifts_;
  QVector<double> jitterSamples_;
};
