#pragma once

/// @brief Service for real-time performance monitoring of the EtherCAT bus.
///
/// @details Provides latency measurement, throughput measurement, resource
/// monitoring, and quality assessment. Uses a QTimer to poll metrics at a
/// configurable interval and emits signals when data is updated. Offline start
/// requests do not synthesize active monitoring or random performance data.

#include <QObject>
#include <QTimer>
#include <QVector>

struct LatencyMetrics {
  double minUs = 0.0;
  double maxUs = 0.0;
  double avgUs = 0.0;
  double stddevUs = 0.0;
  int sampleCount = 0;
  int windowSize = 200;
  QVector<int> histogram;
};

struct ThroughputMetrics {
  double framesPerSecond = 0.0;
  double bytesPerSecond = 0.0;
  double errorRate = 0.0;
  double utilizationPercent = 0.0;
  quint64 totalFrames = 0;
  quint64 totalBytes = 0;
  quint64 totalErrors = 0;
};

struct ResourceMetrics {
  double cpuPercent = 0.0;
  double memoryMB = 0.0;
  int threadCount = 0;
  int socketCount = 0;
  double openFilesPercent = 0.0;
};

struct QualityAssessment {
  double score = 100.0;
  QString grade;
  double jitterUs = 0.0;
  double packetLossPercent = 0.0;
  int consecutiveErrors = 0;
};

class EcatClient;

class RealtimePerformanceService : public QObject {
  Q_OBJECT
public:
  explicit RealtimePerformanceService(EcatClient *client,
                                      QObject *parent = nullptr);

  void startMonitoring(int intervalMs = 500);
  void stopMonitoring();
  bool isMonitoring() const;

  LatencyMetrics latency() const;
  ThroughputMetrics throughput() const;
  ResourceMetrics resources() const;
  QualityAssessment quality() const;

  void setLatencyThreshold(double us);
  double latencyThreshold() const { return latencyThresholdUs_; }
  void setHistoryWindowSize(int samples);
  int historyWindowSize() const { return historyWindowSize_; }

signals:
  void latencyUpdated(const LatencyMetrics &metrics);
  void throughputUpdated(const ThroughputMetrics &metrics);
  void resourceUpdated(const ResourceMetrics &metrics);
  void qualityUpdated(const QualityAssessment &quality);
  void monitoringStateChanged(bool active);

private:
  void poll();
  void measureLatency();
  void measureThroughput();
  void measureResources();
  void assessQuality();

  EcatClient *client_;
  QTimer *pollTimer_ = nullptr;
  LatencyMetrics latency_;
  ThroughputMetrics throughput_;
  ResourceMetrics resources_;
  QualityAssessment quality_;
  double latencyThresholdUs_ = 1000.0;
  int historyWindowSize_ = 200;
  QVector<double> latencySamples_;
  quint64 lastFrameCount_ = 0;
  quint64 lastByteCount_ = 0;
  quint64 lastErrorCount_ = 0;
  qint64 lastPollTimeMs_ = 0;
  int consecutiveErrors_ = 0;
};
