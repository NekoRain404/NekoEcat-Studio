// PerformanceMonitorService.cpp — Comprehensive performance monitoring implementation.
//
// Tracks startup time, runtime performance, bus metrics, and memory usage.
// Provides ring buffer history, alerts, and performance reports.

#include "PerformanceMonitorService.h"
#include "EventBus.h"
#include "infra/EcatClient.h"

#include <QTimer>
#include <QJsonArray>
#include <QDateTime>
#include <QCoreApplication>
#include <algorithm>
#include <numeric>

PerformanceMonitorService::PerformanceMonitorService(EventBus *bus,
                                                     EcatClient *client,
                                                     QObject *parent)
    : QObject(parent), bus_(bus), client_(client) {
  ringBuffer_.resize(kHistorySize);

  connect(bus_, &EventBus::dcSyncUpdate, this,
          &PerformanceMonitorService::onDcSyncUpdate);

  timer_ = new QTimer(this);
  connect(timer_, &QTimer::timeout, this,
          &PerformanceMonitorService::collectMetrics);
}

// ── Monitoring Control ───────────────────────────────────────────────

void PerformanceMonitorService::startMonitoring(int intervalMs) {
  if (running_) return;
  running_ = true;
  timer_->setInterval(intervalMs);
  timer_->start();
}

void PerformanceMonitorService::stopMonitoring() {
  running_ = false;
  timer_->stop();
}

// ── Startup Time Tracking ────────────────────────────────────────────

void PerformanceMonitorService::beginStartup() {
  startupComplete_ = false;
  startupPhases_.clear();
  totalStartupMs_ = 0.0;
  startupTimer_.start();
}

void PerformanceMonitorService::recordStartupPhase(const QString &phase,
                                                    double durationMs) {
  startupPhases_[phase] = durationMs;
}

void PerformanceMonitorService::endStartup() {
  totalStartupMs_ = startupTimer_.elapsed();
  startupComplete_ = true;
}

QJsonObject PerformanceMonitorService::startupReport() const {
  QJsonObject report;
  report["complete"] = startupComplete_;
  report["totalMs"] = totalStartupMs_;

  QJsonObject phases;
  double phaseSum = 0.0;
  for (auto it = startupPhases_.constBegin(); it != startupPhases_.constEnd(); ++it) {
    phases[it.key()] = it.value();
    phaseSum += it.value();
  }
  report["phases"] = phases;

  // Use phase sum if timer-based total is 0 (very fast startup)
  const double effectiveTotal = (totalStartupMs_ > 0) ? totalStartupMs_ : phaseSum;

  // Breakdown percentages
  QJsonObject percentages;
  if (effectiveTotal > 0) {
    for (auto it = startupPhases_.constBegin(); it != startupPhases_.constEnd(); ++it) {
      percentages[it.key()] = (it.value() / effectiveTotal) * 100.0;
    }
  }
  report["percentages"] = percentages;

  return report;
}

// ── Runtime Performance Tracking ─────────────────────────────────────

void PerformanceMonitorService::recordSdoReadLatency(double latencyMs) {
  sdoReadLatencyMs_ = latencyMs;
  sdoReadHistory_.append(latencyMs);
  if (sdoReadHistory_.size() > kLatencyHistorySize) {
    sdoReadHistory_.removeFirst();
  }
}

void PerformanceMonitorService::recordSdoWriteLatency(double latencyMs) {
  sdoWriteLatencyMs_ = latencyMs;
  sdoWriteHistory_.append(latencyMs);
  if (sdoWriteHistory_.size() > kLatencyHistorySize) {
    sdoWriteHistory_.removeFirst();
  }
}

void PerformanceMonitorService::recordStateTransition(double durationMs) {
  stateTransitionMs_ = durationMs;
  stateTransitionHistory_.append(durationMs);
  if (stateTransitionHistory_.size() > kLatencyHistorySize) {
    stateTransitionHistory_.removeFirst();
  }
}

void PerformanceMonitorService::recordFreeRunCycleTime(double cycleTimeUs) {
  freeRunCycleUs_ = cycleTimeUs;
  freeRunCycleHistory_.append(cycleTimeUs);
  if (freeRunCycleHistory_.size() > kLatencyHistorySize) {
    freeRunCycleHistory_.removeFirst();
  }
}

void PerformanceMonitorService::recordUiUpdateTime(double durationMs) {
  uiUpdateMs_ = durationMs;
  uiUpdateHistory_.append(durationMs);
  if (uiUpdateHistory_.size() > kLatencyHistorySize) {
    uiUpdateHistory_.removeFirst();
  }
}

// ── Memory Usage Tracking ────────────────────────────────────────────

void PerformanceMonitorService::recordServiceMemory(const QString &serviceName,
                                                     qint64 bytes) {
  serviceMemory_[serviceName] = bytes;
}

void PerformanceMonitorService::recordPluginMemory(const QString &pluginName,
                                                    qint64 bytes) {
  pluginMemory_[pluginName] = bytes;
}

void PerformanceMonitorService::recordCacheMemory(const QString &cacheName,
                                                   qint64 bytes) {
  cacheMemory_[cacheName] = bytes;
}

QJsonObject PerformanceMonitorService::memoryReport() const {
  QJsonObject report;

  // Service memory
  qint64 totalService = 0;
  QJsonObject services;
  for (auto it = serviceMemory_.constBegin(); it != serviceMemory_.constEnd(); ++it) {
    services[it.key()] = static_cast<double>(it.value());
    totalService += it.value();
  }
  report["services"] = services;
  report["totalServiceBytes"] = static_cast<double>(totalService);

  // Plugin memory
  qint64 totalPlugin = 0;
  QJsonObject plugins;
  for (auto it = pluginMemory_.constBegin(); it != pluginMemory_.constEnd(); ++it) {
    plugins[it.key()] = static_cast<double>(it.value());
    totalPlugin += it.value();
  }
  report["plugins"] = plugins;
  report["totalPluginBytes"] = static_cast<double>(totalPlugin);

  // Cache memory
  qint64 totalCache = 0;
  QJsonObject caches;
  for (auto it = cacheMemory_.constBegin(); it != cacheMemory_.constEnd(); ++it) {
    caches[it.key()] = static_cast<double>(it.value());
    totalCache += it.value();
  }
  report["caches"] = caches;
  report["totalCacheBytes"] = static_cast<double>(totalCache);

  // Total
  report["totalBytes"] = static_cast<double>(totalService + totalPlugin + totalCache);
  report["totalMB"] = static_cast<double>(totalService + totalPlugin + totalCache) / (1024.0 * 1024.0);

  return report;
}

// ── Performance Alerts ───────────────────────────────────────────────

void PerformanceMonitorService::setAlertThresholds(const AlertThresholds &thresholds) {
  thresholds_ = thresholds;
}

void PerformanceMonitorService::checkAlerts(const QJsonObject &metrics) {
  const double sdoRead = metrics["sdoReadLatencyMs"].toDouble();
  const double sdoWrite = metrics["sdoWriteLatencyMs"].toDouble();
  const double stateTrans = metrics["stateTransitionMs"].toDouble();
  const double freeRun = metrics["freeRunCycleUs"].toDouble();
  const double uiUpdate = metrics["uiUpdateMs"].toDouble();
  const double memMB = metrics["memoryMB"].toDouble();

  if (sdoRead > thresholds_.sdoLatencyMs) {
    emit performanceAlert("SDO Read", "Latency exceeded threshold",
                          sdoRead, thresholds_.sdoLatencyMs);
  }
  if (sdoWrite > thresholds_.sdoLatencyMs) {
    emit performanceAlert("SDO Write", "Latency exceeded threshold",
                          sdoWrite, thresholds_.sdoLatencyMs);
  }
  if (stateTrans > thresholds_.stateTransitionMs) {
    emit performanceAlert("State Transition", "Duration exceeded threshold",
                          stateTrans, thresholds_.stateTransitionMs);
  }
  if (freeRun > thresholds_.freeRunCycleUs) {
    emit performanceAlert("Free Run", "Cycle time exceeded threshold",
                          freeRun, thresholds_.freeRunCycleUs);
  }
  if (uiUpdate > thresholds_.uiUpdateMs) {
    emit performanceAlert("UI Update", "Duration exceeded threshold",
                          uiUpdate, thresholds_.uiUpdateMs);
  }
  if (memMB > thresholds_.memoryMB) {
    emit performanceAlert("Memory", "Usage exceeded threshold",
                          memMB, thresholds_.memoryMB);
  }
}

// ── Metrics Access ───────────────────────────────────────────────────

QJsonObject PerformanceMonitorService::currentMetrics() const {
  QJsonObject m;

  // Bus metrics
  m["cycleTimeUs"] = cycleTimeUs_;
  m["jitterUs"] = jitterUs_;
  m["frameLoss"] = frameLoss_;
  m["pdoUpdateRate"] = pdoUpdateRate_;

  // Runtime performance
  m["sdoReadLatencyMs"] = sdoReadLatencyMs_;
  m["sdoWriteLatencyMs"] = sdoWriteLatencyMs_;
  m["stateTransitionMs"] = stateTransitionMs_;
  m["freeRunCycleUs"] = freeRunCycleUs_;
  m["uiUpdateMs"] = uiUpdateMs_;

  // Averages
  auto avg = [](const QVector<double> &v) -> double {
    if (v.isEmpty()) return 0.0;
    return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
  };
  m["avgSdoReadLatencyMs"] = avg(sdoReadHistory_);
  m["avgSdoWriteLatencyMs"] = avg(sdoWriteHistory_);
  m["avgStateTransitionMs"] = avg(stateTransitionHistory_);
  m["avgFreeRunCycleUs"] = avg(freeRunCycleHistory_);
  m["avgUiUpdateMs"] = avg(uiUpdateHistory_);

  // Memory
  qint64 totalMem = 0;
  for (auto it = serviceMemory_.constBegin(); it != serviceMemory_.constEnd(); ++it)
    totalMem += it.value();
  for (auto it = pluginMemory_.constBegin(); it != pluginMemory_.constEnd(); ++it)
    totalMem += it.value();
  for (auto it = cacheMemory_.constBegin(); it != cacheMemory_.constEnd(); ++it)
    totalMem += it.value();
  m["memoryMB"] = static_cast<double>(totalMem) / (1024.0 * 1024.0);

  // Metadata
  m["timestamp"] = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
  m["sampleCount"] = sampleCount_;
  m["startupComplete"] = startupComplete_;

  return m;
}

QVector<QJsonObject> PerformanceMonitorService::history() const {
  QVector<QJsonObject> result;
  if (sampleCount_ == 0) return result;

  const int count = qMin(sampleCount_, kHistorySize);
  const int start = (sampleCount_ <= kHistorySize) ? 0 : writeIndex_;

  result.reserve(count);
  for (int i = 0; i < count; ++i) {
    result.append(ringBuffer_[(start + i) % kHistorySize]);
  }
  return result;
}

// ── Performance Report ───────────────────────────────────────────────

QJsonObject PerformanceMonitorService::performanceReport() const {
  QJsonObject report;

  // Current metrics
  report["current"] = currentMetrics();

  // Startup report
  report["startup"] = startupReport();

  // Memory report
  report["memory"] = memoryReport();

  // Historical statistics
  QJsonObject stats;
  auto calcStats = [](const QVector<double> &v) -> QJsonObject {
    QJsonObject s;
    if (v.isEmpty()) {
      s["min"] = 0.0;
      s["max"] = 0.0;
      s["avg"] = 0.0;
      s["count"] = 0;
      return s;
    }
    double min = *std::min_element(v.begin(), v.end());
    double max = *std::max_element(v.begin(), v.end());
    double avg = std::accumulate(v.begin(), v.end(), 0.0) / v.size();
    s["min"] = min;
    s["max"] = max;
    s["avg"] = avg;
    s["count"] = v.size();
    return s;
  };

  stats["sdoReadLatency"] = calcStats(sdoReadHistory_);
  stats["sdoWriteLatency"] = calcStats(sdoWriteHistory_);
  stats["stateTransition"] = calcStats(stateTransitionHistory_);
  stats["freeRunCycleTime"] = calcStats(freeRunCycleHistory_);
  stats["uiUpdateTime"] = calcStats(uiUpdateHistory_);
  report["statistics"] = stats;

  // Alert thresholds
  QJsonObject thresh;
  thresh["sdoLatencyMs"] = thresholds_.sdoLatencyMs;
  thresh["stateTransitionMs"] = thresholds_.stateTransitionMs;
  thresh["freeRunCycleUs"] = thresholds_.freeRunCycleUs;
  thresh["uiUpdateMs"] = thresholds_.uiUpdateMs;
  thresh["memoryMB"] = thresholds_.memoryMB;
  report["thresholds"] = thresh;

  return report;
}

// ── DC Sync Handler ──────────────────────────────────────────────────

void PerformanceMonitorService::onDcSyncUpdate(const QJsonObject &data) {
  const QJsonArray slaves = data.value("slaves").toArray();
  if (slaves.isEmpty()) return;

  double maxJitter = 0.0;
  double avgCycleTime = 0.0;
  int count = 0;

  for (const auto &entry : slaves) {
    const QJsonObject s = entry.toObject();
    if (s.value("syncing").toBool()) {
      const double jMax = s.value("jitterMax").toDouble();
      if (jMax > maxJitter) maxJitter = jMax;

      avgCycleTime += s.value("driftNs").toDouble();
      ++count;
    }
  }

  if (count > 0) {
    cycleTimeUs_ = (avgCycleTime / count) / 1000.0;
    jitterUs_ = maxJitter / 1000.0;
  }
}

// ── Periodic Collection ──────────────────────────────────────────────

void PerformanceMonitorService::collectMetrics() {
  if (!client_ || !client_->isConnected()) return;

  const qint64 now = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
  if (lastPdoTimestamp_ > 0) {
    const qint64 elapsed = now - lastPdoTimestamp_;
    if (elapsed > 0) {
      pdoUpdateRate_ = 1000.0 / static_cast<double>(elapsed);
    }
  }
  lastPdoTimestamp_ = now;

  QJsonObject sample = currentMetrics();
  recordSample(sample);
  checkAlerts(sample);
  emit metricsUpdated(sample);
}

void PerformanceMonitorService::recordSample(const QJsonObject &sample) {
  ringBuffer_[writeIndex_] = sample;
  writeIndex_ = (writeIndex_ + 1) % kHistorySize;
  ++sampleCount_;
}
