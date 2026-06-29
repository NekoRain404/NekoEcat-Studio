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

/// @brief Comprehensive performance monitoring service for NekoEcat Studio.
///
/// Monitors startup time, runtime performance (SDO latency, state transitions,
/// Free Run cycle time, UI updates), bus metrics, and memory usage. Provides
/// ring buffer history, periodic online collection, and performance alerts.
class PerformanceMonitorService : public QObject {
  Q_OBJECT
public:
  /// @brief Maximum number of samples retained in the ring buffer history.
  static constexpr int kHistorySize = 1000;

  /// @brief Construct the performance monitor service.
  /// @param bus     Event bus for receiving DC sync updates.
  /// @param client  TCP client to the ecatd daemon.
  /// @param parent  Parent QObject.
  explicit PerformanceMonitorService(EventBus *bus, EcatClient *client,
                                     QObject *parent = nullptr);

  // ── Monitoring Control ───────────────────────────────────────────
  /// @brief Start periodic online metric collection when the daemon is connected.
  /// @param intervalMs  Polling interval in milliseconds (default: 1000ms).
  /// @note Offline calls are ignored and leave the service inactive.
  void startMonitoring(int intervalMs = 1000);

  /// @brief Stop periodic online metric collection.
  void stopMonitoring();

  /// @brief Check if periodic monitoring is currently active.
  /// @return true if monitoring is running.
  bool isMonitoring() const { return running_; }

  // ── Startup Time Tracking ────────────────────────────────────────
  /// @brief Begin a new startup timing session.
  /// @note Call this at the very start of application initialization.
  void beginStartup();

  /// @brief Record the duration of a named startup phase.
  /// @param phase      Phase name (e.g., "pluginLoad", "serviceInit").
  /// @param durationMs Duration of the phase in milliseconds.
  void recordStartupPhase(const QString &phase, double durationMs);

  /// @brief Mark the startup timing session as complete.
  void endStartup();

  /// @brief Get a JSON report of all recorded startup phase durations.
  /// @return QJsonObject containing phase names, durations, and total startup time.
  QJsonObject startupReport() const;

  /// @brief Check if the startup timing session has been completed.
  /// @return true if endStartup() has been called.
  bool startupComplete() const { return startupComplete_; }

  // ── Runtime Performance Tracking ─────────────────────────────────
  /// @brief Record an SDO read latency measurement.
  /// @param latencyMs  Latency in milliseconds.
  void recordSdoReadLatency(double latencyMs);

  /// @brief Record an SDO write latency measurement.
  /// @param latencyMs  Latency in milliseconds.
  void recordSdoWriteLatency(double latencyMs);

  /// @brief Record a state transition duration.
  /// @param durationMs  Duration in milliseconds.
  void recordStateTransition(double durationMs);

  /// @brief Record a Free Run cycle time measurement.
  /// @param cycleTimeUs  Cycle time in microseconds.
  void recordFreeRunCycleTime(double cycleTimeUs);

  /// @brief Record a UI update duration.
  /// @param durationMs  Duration in milliseconds.
  void recordUiUpdateTime(double durationMs);

  // ── Memory Usage Tracking ────────────────────────────────────────
  /// @brief Record memory usage for a named service.
  /// @param serviceName  Name of the service.
  /// @param bytes        Memory usage in bytes.
  void recordServiceMemory(const QString &serviceName, qint64 bytes);

  /// @brief Record memory usage for a named plugin.
  /// @param pluginName  Name of the plugin.
  /// @param bytes       Memory usage in bytes.
  void recordPluginMemory(const QString &pluginName, qint64 bytes);

  /// @brief Record memory usage for a named cache.
  /// @param cacheName  Name of the cache.
  /// @param bytes      Memory usage in bytes.
  void recordCacheMemory(const QString &cacheName, qint64 bytes);

  /// @brief Get a JSON report of all tracked memory usage.
  /// @return QJsonObject with service, plugin, and cache memory breakdowns.
  QJsonObject memoryReport() const;

  // ── Performance Alerts ───────────────────────────────────────────
  /// @brief Thresholds that trigger performance alerts when exceeded.
  struct AlertThresholds {
    double sdoLatencyMs = 100.0;      ///< SDO latency alert threshold (ms).
    double stateTransitionMs = 500.0; ///< State transition alert threshold (ms).
    double freeRunCycleUs = 2000.0;   ///< Free Run cycle alert threshold (us).
    double uiUpdateMs = 50.0;         ///< UI update alert threshold (ms).
    double memoryMB = 512.0;          ///< Memory usage alert threshold (MB).
  };

  /// @brief Set custom alert thresholds for performance monitoring.
  /// @param thresholds  New threshold values.
  void setAlertThresholds(const AlertThresholds &thresholds);

  /// @brief Get the current alert thresholds.
  /// @return Current AlertThresholds structure.
  AlertThresholds alertThresholds() const { return thresholds_; }

  // ── Metrics Access ───────────────────────────────────────────────
  /// @brief Get the most recent set of collected metrics.
  /// @return QJsonObject containing current bus, runtime, and memory metrics.
  QJsonObject currentMetrics() const;

  /// @brief Get the full ring buffer history of collected metrics.
  /// @return Vector of QJsonObjects, oldest first, up to kHistorySize samples.
  QVector<QJsonObject> history() const;

  // ── Performance Report ───────────────────────────────────────────
  /// @brief Generate a comprehensive performance report combining all metrics.
  /// @return QJsonObject with startup, runtime, bus, memory, and alert data.
  QJsonObject performanceReport() const;

signals:
  /// @brief Emitted when a new set of metrics is collected.
  /// @param metrics  QJsonObject with the latest collected metrics.
  void metricsUpdated(const QJsonObject &metrics);

  /// @brief Emitted when a monitored value exceeds its alert threshold.
  /// @param category   Alert category (e.g., "sdoLatency", "memory").
  /// @param message    Human-readable alert description.
  /// @param value      The measured value that triggered the alert.
  /// @param threshold  The threshold that was exceeded.
  void performanceAlert(const QString &category, const QString &message,
                        double value, double threshold);

private slots:
  /// @brief Handle DC sync update events from the event bus.
  /// @param data  DC sync update data.
  void onDcSyncUpdate(const QJsonObject &data);

  /// @brief Periodic slot that collects metrics from the daemon.
  void collectMetrics();

private:
  /// @brief Store a metric sample in the ring buffer.
  /// @param sample  Metric sample to record.
  void recordSample(const QJsonObject &sample);

  /// @brief Check metrics against alert thresholds and emit alerts.
  /// @param metrics  Current metrics to evaluate.
  void checkAlerts(const QJsonObject &metrics);

  EventBus *bus_;              ///< Event bus for DC sync updates.
  EcatClient *client_;         ///< TCP client to ecatd daemon.
  QTimer *timer_ = nullptr;    ///< Timer for periodic metric collection.
  bool running_ = false;       ///< Whether periodic monitoring is active.

  // ── Startup Timing ───────────────────────────────────────────────
  QElapsedTimer startupTimer_;          ///< Timer for measuring startup phases.
  bool startupComplete_ = false;        ///< Whether startup timing session is complete.
  QHash<QString, double> startupPhases_; ///< Map of phase name to duration (ms).
  double totalStartupMs_ = 0.0;         ///< Total startup duration (ms).

  // ── Bus Metrics ──────────────────────────────────────────────────
  double cycleTimeUs_ = 0.0;        ///< Last measured bus cycle time (us).
  double jitterUs_ = 0.0;           ///< Last measured bus jitter (us).
  int frameLoss_ = 0;               ///< Frame loss count.
  double pdoUpdateRate_ = 0.0;      ///< PDO update rate (updates/sec).
  qint64 lastPdoTimestamp_ = 0;     ///< Timestamp of last PDO update (ms).
  int pdoCount_ = 0;                ///< PDO update counter within measurement window.

  // ── Runtime Performance ──────────────────────────────────────────
  double sdoReadLatencyMs_ = 0.0;     ///< Most recent SDO read latency (ms).
  double sdoWriteLatencyMs_ = 0.0;    ///< Most recent SDO write latency (ms).
  double stateTransitionMs_ = 0.0;    ///< Most recent state transition duration (ms).
  double freeRunCycleUs_ = 0.0;       ///< Most recent Free Run cycle time (us).
  double uiUpdateMs_ = 0.0;           ///< Most recent UI update duration (ms).

  // ── Runtime History (for averaging) ──────────────────────────────
  QVector<double> sdoReadHistory_;          ///< Recent SDO read latencies.
  QVector<double> sdoWriteHistory_;         ///< Recent SDO write latencies.
  QVector<double> stateTransitionHistory_;  ///< Recent state transition durations.
  QVector<double> freeRunCycleHistory_;     ///< Recent Free Run cycle times.
  QVector<double> uiUpdateHistory_;         ///< Recent UI update durations.
  /// @brief Maximum number of samples retained for averaging.
  static constexpr int kLatencyHistorySize = 100;

  // ── Memory Tracking ──────────────────────────────────────────────
  QHash<QString, qint64> serviceMemory_;  ///< Service name to memory usage (bytes).
  QHash<QString, qint64> pluginMemory_;   ///< Plugin name to memory usage (bytes).
  QHash<QString, qint64> cacheMemory_;    ///< Cache name to memory usage (bytes).

  // ── Alert Thresholds ─────────────────────────────────────────────
  AlertThresholds thresholds_;  ///< Current alert thresholds.

  // ── Ring Buffer ──────────────────────────────────────────────────
  QVector<QJsonObject> ringBuffer_;  ///< Circular buffer of metric samples.
  int writeIndex_ = 0;               ///< Current write position in ring buffer.
  int sampleCount_ = 0;              ///< Total number of samples recorded.
};
