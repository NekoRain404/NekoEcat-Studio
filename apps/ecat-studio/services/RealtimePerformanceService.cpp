// RealtimePerformanceService — real-time EtherCAT bus performance monitoring.
//
// Polls EcatClient at a configurable interval and computes latency, throughput,
// resource, and quality metrics. Offline calls do not synthesize performance
// samples or active monitoring state.

#include "RealtimePerformanceService.h"
#include "infra/EcatClient.h"

#include <QtMath>

RealtimePerformanceService::RealtimePerformanceService(EcatClient *client,
                                                       QObject *parent)
    : QObject(parent), client_(client) {
  pollTimer_ = new QTimer(this);
  pollTimer_->setSingleShot(false);
  connect(pollTimer_, &QTimer::timeout, this, &RealtimePerformanceService::poll);
}

void RealtimePerformanceService::startMonitoring(int intervalMs) {
  if (pollTimer_->isActive()) return;
  if (!client_ || !client_->isConnected()) return;
  lastPollTimeMs_ = QDateTime::currentMSecsSinceEpoch();
  pollTimer_->start(intervalMs);
  emit monitoringStateChanged(true);
}

void RealtimePerformanceService::stopMonitoring() {
  if (!pollTimer_->isActive()) return;
  pollTimer_->stop();
  emit monitoringStateChanged(false);
}

bool RealtimePerformanceService::isMonitoring() const {
  return pollTimer_->isActive();
}

LatencyMetrics RealtimePerformanceService::latency() const { return latency_; }
ThroughputMetrics RealtimePerformanceService::throughput() const { return throughput_; }
ResourceMetrics RealtimePerformanceService::resources() const { return resources_; }
QualityAssessment RealtimePerformanceService::quality() const { return quality_; }

void RealtimePerformanceService::setLatencyThreshold(double us) {
  latencyThresholdUs_ = us;
}

void RealtimePerformanceService::setHistoryWindowSize(int samples) {
  historyWindowSize_ = samples;
  while (latencySamples_.size() > historyWindowSize_)
    latencySamples_.removeFirst();
  latency_.windowSize = historyWindowSize_;
}

void RealtimePerformanceService::poll() {
  if (!client_ || !client_->isConnected()) return;
  measureLatency();
  measureThroughput();
  measureResources();
  assessQuality();
  emit latencyUpdated(latency_);
  emit throughputUpdated(throughput_);
  emit resourceUpdated(resources_);
  emit qualityUpdated(quality_);
}

void RealtimePerformanceService::measureLatency() {
  double sampleUs = 0.0;
  qint64 startNs = QDateTime::currentMSecsSinceEpoch() * 1000;
  client_->ping();
  qint64 elapsedNs = (QDateTime::currentMSecsSinceEpoch() * 1000) - startNs;
  sampleUs = static_cast<double>(elapsedNs) / 1000.0;

  latencySamples_.append(sampleUs);
  while (latencySamples_.size() > historyWindowSize_)
    latencySamples_.removeFirst();

  if (latencySamples_.isEmpty()) return;

  double minV = latencySamples_.first();
  double maxV = latencySamples_.first();
  double sum = 0.0;
  for (double v : latencySamples_) {
    if (v < minV) minV = v;
    if (v > maxV) maxV = v;
    sum += v;
  }
  double avg = sum / latencySamples_.size();

  double variance = 0.0;
  for (double v : latencySamples_) {
    double d = v - avg;
    variance += d * d;
  }
  variance /= latencySamples_.size();

  latency_.minUs = minV;
  latency_.maxUs = maxV;
  latency_.avgUs = avg;
  latency_.stddevUs = qSqrt(variance);
  latency_.sampleCount = latencySamples_.size();
  latency_.windowSize = historyWindowSize_;

  latency_.histogram.resize(10);
  latency_.histogram.fill(0);
  if (maxV > minV) {
    for (double v : latencySamples_) {
      int bin = qBound(0, static_cast<int>((v - minV) / (maxV - minV) * 9), 9);
      latency_.histogram[bin]++;
    }
  }
}

void RealtimePerformanceService::measureThroughput() {
  qint64 nowMs = QDateTime::currentMSecsSinceEpoch();
  double elapsedS = (nowMs - lastPollTimeMs_) / 1000.0;
  if (elapsedS <= 0) elapsedS = 0.5;

  quint64 currentFrames = throughput_.totalFrames;
  quint64 currentBytes = currentFrames * 1518;
  quint64 currentErrors = throughput_.totalErrors;

  if (lastPollTimeMs_ > 0) {
    throughput_.framesPerSecond = static_cast<double>(currentFrames) / elapsedS;
    throughput_.bytesPerSecond = static_cast<double>(currentBytes) / elapsedS;
    throughput_.errorRate = static_cast<double>(currentErrors) / elapsedS;
  }

  throughput_.totalFrames += currentFrames;
  throughput_.totalBytes += currentBytes;
  throughput_.totalErrors += currentErrors;
  throughput_.utilizationPercent = qBound(0.0,
      throughput_.bytesPerSecond / (100.0 * 1024 * 1024) * 100.0, 100.0);

  lastPollTimeMs_ = nowMs;
  lastFrameCount_ = throughput_.totalFrames;
  lastByteCount_ = throughput_.totalBytes;
  lastErrorCount_ = throughput_.totalErrors;
}

void RealtimePerformanceService::measureResources() {
  resources_ = {};
}

void RealtimePerformanceService::assessQuality() {
  double jitter = 0.0;
  if (latencySamples_.size() >= 2) {
    double prevDiff = 0.0;
    for (int i = 1; i < latencySamples_.size(); ++i) {
      double diff = qAbs(latencySamples_[i] - latencySamples_[i - 1]);
      jitter += qAbs(diff - prevDiff);
      prevDiff = diff;
    }
    jitter /= (latencySamples_.size() - 1);
  }

  quality_.jitterUs = jitter;
  quality_.packetLossPercent = throughput_.totalFrames > 0
      ? static_cast<double>(throughput_.totalErrors) /
        throughput_.totalFrames * 100.0
      : 0.0;

  if (throughput_.totalErrors > 0)
    consecutiveErrors_++;
  else
    consecutiveErrors_ = 0;
  quality_.consecutiveErrors = consecutiveErrors_;

  double score = 100.0;
  score -= qMin(30.0, latency_.avgUs / latencyThresholdUs_ * 30.0);
  score -= qMin(20.0, jitter / latencyThresholdUs_ * 20.0);
  score -= qMin(30.0, quality_.packetLossPercent * 10.0);
  score -= qMin(20.0, consecutiveErrors_ * 5.0);
  quality_.score = qBound(0.0, score, 100.0);

  if (quality_.score >= 90.0)
    quality_.grade = QStringLiteral("Excellent");
  else if (quality_.score >= 75.0)
    quality_.grade = QStringLiteral("Good");
  else if (quality_.score >= 50.0)
    quality_.grade = QStringLiteral("Fair");
  else
    quality_.grade = QStringLiteral("Poor");
}
