#pragma once

// PerformanceMonitorService — comprehensive performance monitoring for NekoEcat Studio.
//
// Monitors:
//   - Startup time: plugin load, service init, UI construction, total startup
//   - Runtime performance: SDO read/write latency, state transition time,
//     Free Run cycle time, UI update time
//   - Bus metrics: cycle time, jitter, frame loss, PDO update rate
//   - Memory usage: service, plugin, cache memory tracking
//
// Provides ring buffer history (1000 samples), periodic online collection,
// startup time reporting, and performance alerts. Offline callers can still
// record local startup, memory, and explicit operation timings, but periodic
// monitoring only starts with a live daemon connection.
//
// Usage:
//   PerformanceMonitorService *perf = new PerformanceMonitorService(bus, client);
//   perf->startMonitoring(1000);
//   perf->recordStartupPhase("pluginLoad", elapsedMs);
//   QJsonObject metrics = perf->currentMetrics();
//   QJsonObject startup = perf->startupReport();
//   QJsonObject memory = perf->memoryReport();

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include <QElapsedTimer>
#include <QHash>
#include <QString>

class QTimer;
class EventBus;
class EcatClient;

class PerformanceMonitorService : public QObject {
  Q_OBJECT
public:
  static constexpr int kHistorySize = 1000;

  explicit PerformanceMonitorService(EventBus *bus, EcatClient *client,
                                     QObject *parent = nullptr);

  // ── Monitoring Control ───────────────────────────────────────────
  // Start periodic online metric collection when the daemon is connected.
  // Offline calls are ignored and leave the service inactive.
  void startMonitoring(int intervalMs = 1000);
  void stopMonitoring();
  bool isMonitoring() const { return running_; }

  // ── Startup Time Tracking ────────────────────────────────────────
  // Call these during application startup to record phase durations.
  void beginStartup();
  void recordStartupPhase(const QString &phase, double durationMs);
  void endStartup();
  QJsonObject startupReport() const;
  bool startupComplete() const { return startupComplete_; }

  // ── Runtime Performance Tracking ─────────────────────────────────
  // Call these to record individual operation timings.
  void recordSdoReadLatency(double latencyMs);
  void recordSdoWriteLatency(double latencyMs);
  void recordStateTransition(double durationMs);
  void recordFreeRunCycleTime(double cycleTimeUs);
  void recordUiUpdateTime(double durationMs);

  // ── Memory Usage Tracking ────────────────────────────────────────
  void recordServiceMemory(const QString &serviceName, qint64 bytes);
  void recordPluginMemory(const QString &pluginName, qint64 bytes);
  void recordCacheMemory(const QString &cacheName, qint64 bytes);
  QJsonObject memoryReport() const;

  // ── Performance Alerts ───────────────────────────────────────────
  struct AlertThresholds {
    double sdoLatencyMs = 100.0;
    double stateTransitionMs = 500.0;
    double freeRunCycleUs = 2000.0;
    double uiUpdateMs = 50.0;
    double memoryMB = 512.0;
  };
  void setAlertThresholds(const AlertThresholds &thresholds);
  AlertThresholds alertThresholds() const { return thresholds_; }

  // ── Metrics Access ───────────────────────────────────────────────
  QJsonObject currentMetrics() const;
  QVector<QJsonObject> history() const;

  // ── Performance Report ───────────────────────────────────────────
  QJsonObject performanceReport() const;

signals:
  void metricsUpdated(const QJsonObject &metrics);
  void performanceAlert(const QString &category, const QString &message,
                        double value, double threshold);

private slots:
  void onDcSyncUpdate(const QJsonObject &data);
  void collectMetrics();

private:
  void recordSample(const QJsonObject &sample);
  void checkAlerts(const QJsonObject &metrics);

  EventBus *bus_;
  EcatClient *client_;
  QTimer *timer_ = nullptr;
  bool running_ = false;

  // ── Startup Timing ───────────────────────────────────────────────
  QElapsedTimer startupTimer_;
  bool startupComplete_ = false;
  QHash<QString, double> startupPhases_;
  double totalStartupMs_ = 0.0;

  // ── Bus Metrics ──────────────────────────────────────────────────
  double cycleTimeUs_ = 0.0;
  double jitterUs_ = 0.0;
  int frameLoss_ = 0;
  double pdoUpdateRate_ = 0.0;
  qint64 lastPdoTimestamp_ = 0;
  int pdoCount_ = 0;

  // ── Runtime Performance ──────────────────────────────────────────
  double sdoReadLatencyMs_ = 0.0;
  double sdoWriteLatencyMs_ = 0.0;
  double stateTransitionMs_ = 0.0;
  double freeRunCycleUs_ = 0.0;
  double uiUpdateMs_ = 0.0;

  // ── Runtime History (for averaging) ──────────────────────────────
  QVector<double> sdoReadHistory_;
  QVector<double> sdoWriteHistory_;
  QVector<double> stateTransitionHistory_;
  QVector<double> freeRunCycleHistory_;
  QVector<double> uiUpdateHistory_;
  static constexpr int kLatencyHistorySize = 100;

  // ── Memory Tracking ──────────────────────────────────────────────
  QHash<QString, qint64> serviceMemory_;
  QHash<QString, qint64> pluginMemory_;
  QHash<QString, qint64> cacheMemory_;

  // ── Alert Thresholds ─────────────────────────────────────────────
  AlertThresholds thresholds_;

  // ── Ring Buffer ──────────────────────────────────────────────────
  QVector<QJsonObject> ringBuffer_;
  int writeIndex_ = 0;
  int sampleCount_ = 0;
};
