#pragma once

// TraceService — multi-channel signal tracing backend.
// Manages trace channels bound to slave SDO entries with configurable
// sample rate, buffer size, and trigger modes.
//
// DEMO STUB — This service generates synthetic data for UI demonstration.
// Replace with real hardware integration for production use.
//
// This service provides multi-channel signal tracing capabilities for
// the EtherCAT network. It handles:
//   - Trace channel management (add, remove, configure)
//   - Signal tracing with configurable sample rate
//   - Buffer management with configurable size
//   - Trigger modes (Auto, Normal, Single, Rising, Falling)
//   - Trace data collection and retrieval
//   - Multi-channel synchronization
//
// Usage:
//   TraceService trace;
//   int chId = trace.addChannel("Motor Current", 0, "0x6000", "0x01");
//   trace.setSampleRate(1000);  // 1kHz sampling
//   trace.setBufferSize(10000);  // 10k points buffer
//   trace.setTriggerMode(TraceTriggerMode::Rising);
//   trace.startTrace();
//   QVector<TracePoint> data = trace.getTraceData(chId);
//   trace.stopTrace();
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   uses a QTimer for trace data collection, which runs on the main thread.
//
// Performance:
//   - Trace data collection is O(n) where n is number of channels
//   - Buffer management is O(1) per sample
//   - Data retrieval is O(n) where n is buffer size

#include <QObject>
#include <QVector>
#include <QString>
#include <QTimer>
#include <QElapsedTimer>

// Trigger mode enumeration.
enum class TraceTriggerMode { 
  Auto,     // Auto-trigger (continuous)
  Normal,   // Normal trigger (wait for trigger)
  Single,   // Single-shot trigger
  Rising,   // Rising edge trigger
  Falling   // Falling edge trigger
};

// Represents a single trace data point.
struct TracePoint {
  qint64 timestamp = 0;   // Timestamp in milliseconds
  double value = 0.0;      // Traced value
  int channelId = -1;      // Channel ID
  int quality = 100;       // Data quality (0-100)
};

// Trace channel configuration.
struct TraceChannelConfig {
  int id = -1;                    // Channel ID
  int slave = 0;                  // Slave position
  QString index;                  // SDO index in hex format
  QString subIndex;               // SDO subindex in hex format
  QString name;                   // Channel name
  QVector<TracePoint> data;       // Trace data buffer
};

class TraceService : public QObject {
  Q_OBJECT
public:
  explicit TraceService(QObject *parent = nullptr);

  // Add a new trace channel.
  // @param name      Channel name
  // @param slave     Slave position
  // @param index     SDO index in hex format
  // @param subIndex  SDO subindex in hex format
  // @return Channel ID
  int addChannel(const QString &name, int slave, const QString &index, const QString &subIndex);

  // Remove a trace channel.
  // @param channelId  Channel ID to remove
  void removeChannel(int channelId);

  // Get all trace channels.
  // @return Vector of TraceChannelConfig structures
  QVector<TraceChannelConfig> channels() const;

  // Start tracing.
  void startTrace();

  // Stop tracing.
  void stopTrace();

  // Check if tracing is active.
  // @return true if tracing is active
  bool isTracing() const;

  // Set the sample rate.
  // @param rate  Sample rate in Hz
  void setSampleRate(int rate);

  // Get the current sample rate.
  // @return Sample rate in Hz
  int sampleRate() const;

  // Set the buffer size.
  // @param size  Buffer size in samples
  void setBufferSize(int size);

  // Get the current buffer size.
  // @return Buffer size in samples
  int bufferSize() const;

  // Set the trigger mode.
  // @param mode  Trigger mode
  void setTriggerMode(TraceTriggerMode mode);

  // Get the current trigger mode.
  // @return Trigger mode
  TraceTriggerMode triggerMode() const;

  // Get trace data for a specific channel.
  // @param channelId  Channel ID
  // @return Vector of TracePoint data
  QVector<TracePoint> getTraceData(int channelId) const;

  static constexpr int kMaxChannels = 16;        // Maximum number of channels
  static constexpr int kDefaultBufferSize = 10000; // Default buffer size

signals:
  // Emitted when trace data is updated.
  // @param channelId  Channel ID
  // @param data       Updated trace data
  void traceDataUpdated(int channelId, const QVector<TracePoint> &data);

  // Emitted when a channel is added.
  // @param channelId  Channel ID
  void channelAdded(int channelId);

  // Emitted when a channel is removed.
  // @param channelId  Channel ID
  void channelRemoved(int channelId);

  // Emitted when tracing starts.
  void traceStarted();

  // Emitted when tracing stops.
  void traceStopped();

private:
  // Collect trace data (called by timer).
  void tick();

  QVector<TraceChannelConfig> channels_;  // Trace channels
  QTimer *timer_ = nullptr;               // Timer for data collection
  QElapsedTimer elapsed_;                 // Elapsed time tracker
  int nextId_ = 1;                        // Next channel ID
  int sampleRate_ = 1000;                 // Sample rate in Hz
  int bufferSize_ = kDefaultBufferSize;   // Buffer size
  TraceTriggerMode triggerMode_ = TraceTriggerMode::Auto;  // Trigger mode
  bool tracing_ = false;                  // Whether tracing is active
  int tickCount_ = 0;                     // Tick counter
};
