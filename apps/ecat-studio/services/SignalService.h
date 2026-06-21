#pragma once

// SignalService — manages signal channels for the Signal Analyzer plugin.
// Tracks per-channel time-series data, computes running statistics, and
// accumulates samples pushed through EventBus::signalData.
//
// This service provides multi-channel signal monitoring capabilities for
// the Signal Analyzer workspace. It handles:
//   - Channel subscription management (add/remove signal channels)
//   - Time-series data accumulation with ring buffer (10,000 points max)
//   - Running statistics computation (min, max, avg, stddev)
//   - Data injection for testing and external sources
//
// Usage:
//   ServiceContainer *container = ...;
//   SignalService *signal = container->signal();
//   int chId = signal->addChannel("Motor Current", 0, "0x6000", "0x01");
//   signal->startPolling(100);  // Poll every 100ms
//   ChannelStats stats = signal->stats(chId);
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. Data pushed
//   via EventBus is marshaled to the main thread internally.
//
// Performance:
//   - Ring buffer limits memory usage to 10,000 points per channel
//   - Statistics are computed on-demand (not cached)
//   - Channel operations are O(1) for add/remove, O(n) for stats

#include <QObject>
#include <QVector>
#include <QString>
#include <cstdint>

class QTimer;
class EventBus;

// Describes a single subscribed signal channel.
struct SignalChannelInfo {
  int id = -1;                    // Unique channel identifier
  QString name;                   // Human-readable channel name
  int slave = -1;                 // Slave position on the bus
  QString index;                  // SDO index in hex format
  QString subIndex;               // SDO subindex in hex format
  QVector<double> values;         // Time-series data values
  QVector<qint64> timestamps;     // Corresponding timestamps (ms)
  static constexpr int kMaxPoints = 10000;  // Ring buffer limit
};

// Running statistics for a channel's visible data window.
struct ChannelStats {
  double min = 0.0;    // Minimum value in the data window
  double max = 0.0;    // Maximum value in the data window
  double avg = 0.0;    // Average value in the data window
  double stddev = 0.0; // Standard deviation in the data window
};

class SignalService : public QObject {
  Q_OBJECT
public:
  explicit SignalService(EventBus *bus, QObject *parent = nullptr);

  // Add a new signal channel for monitoring.
  // @param name     Human-readable channel name
  // @param slave    Slave position on the bus (0-based)
  // @param idx      SDO index in hex format (e.g., "0x6000")
  // @param sub      SDO subindex in hex format (e.g., "0x01")
  // @return Channel ID (positive integer) or -1 on failure
  int addChannel(const QString &name, int slave,
                 const QString &idx, const QString &sub);

  // Remove a signal channel by ID.
  // @param channelId  Channel ID returned by addChannel()
  void removeChannel(int channelId);

  // Get information about all active channels.
  // @return Vector of SignalChannelInfo structures
  QVector<SignalChannelInfo> channels() const;

  // Get running statistics for a specific channel.
  // @param channelId  Channel ID returned by addChannel()
  // @return ChannelStats structure with min, max, avg, stddev
  ChannelStats stats(int channelId) const;

  // Start periodic polling of signal data.
  // @param intervalMs  Polling interval in milliseconds (default: 100ms)
  void startPolling(int intervalMs = 100);

  // Stop periodic polling.
  void stopPolling();

  // Push data directly into a channel (for testing or external injection).
  // @param channelId   Channel ID returned by addChannel()
  // @param values      Vector of data values
  // @param timestamps  Vector of corresponding timestamps (ms)
  void pushData(int channelId, const QVector<double> &values,
                const QVector<qint64> &timestamps);

signals:
  // Emitted when channel data is updated (new samples added).
  // @param channelId  Channel ID that was updated
  void channelDataUpdated(int channelId);

  // Emitted when a new channel is added.
  // @param channelId  Channel ID of the new channel
  void channelAdded(int channelId);

  // Emitted when a channel is removed.
  // @param channelId  Channel ID that was removed
  void channelRemoved(int channelId);

  // Emitted when an error occurs.
  // @param msg  Human-readable error message
  void error(const QString &msg);

private slots:
  // Handle signal data from EventBus.
  void handleSignalData(int channel, const QVector<double> &values,
                        const QVector<qint64> &timestamps);

private:
  EventBus *bus_;                      // Event bus for signal data
  QTimer *pollTimer_ = nullptr;        // Timer for periodic polling
  QVector<SignalChannelInfo> channels_; // Active signal channels
  int nextId_ = 1;                     // Next channel ID to assign
};
