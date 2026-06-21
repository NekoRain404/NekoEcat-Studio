#pragma once

// BusStatsService — polls the ecatd daemon for bus statistics and
// emits periodic updates. Metrics include frame counts, error counts,
// bandwidth usage, CRC errors, and lost frames.
//
// This service provides bus statistics monitoring for the EtherCAT
// network. It handles:
//   - Frame count tracking (TX/RX frames)
//   - Error count monitoring (TX/RX errors, CRC errors, lost frames)
//   - Bandwidth utilization measurement
//   - Frame rate calculation
//   - Periodic statistics polling from daemon
//   - Statistics history and current value access
//
// Usage:
//   ServiceContainer *container = ...;
//   BusStatsService *busStats = container->busStats();
//   busStats->startMonitoring(1000);  // Poll every 1 second
//   BusStats stats = busStats->currentStats();
//   QJsonObject statsJson = busStats->currentStatsJson();
//   quint64 txFrames = stats.txFrames;
//   double bandwidth = stats.bandwidthMbps;
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   uses a QTimer for periodic polling, which runs on the main thread.
//
// Performance:
//   - Polling interval is configurable (default 1000ms)
//   - Statistics updates are O(1) per poll
//   - Frame rate calculation uses previous frame counts

#include <QObject>
#include <QJsonObject>
#include <QTimer>

class EcatClient;

// Bus statistics structure.
struct BusStats {
  quint64 txFrames = 0;       // Total transmitted frames
  quint64 rxFrames = 0;       // Total received frames
  quint64 txErrors = 0;       // Transmit errors
  quint64 rxErrors = 0;       // Receive errors
  quint64 crcErrors = 0;      // CRC errors
  quint64 lostFrames = 0;     // Lost frames
  double bandwidthMbps = 0.0; // Bandwidth utilization in Mbps
  double frameRate = 0.0;     // Frame rate in frames per second
  qint64 timestampMs = 0;     // Timestamp (ms since epoch)
};

class BusStatsService : public QObject {
  Q_OBJECT
public:
  explicit BusStatsService(EcatClient *client, QObject *parent = nullptr);

  // Start periodic bus statistics monitoring.
  // @param intervalMs  Polling interval in milliseconds (default: 1000ms)
  void startMonitoring(int intervalMs = 1000);

  // Stop periodic bus statistics monitoring.
  void stopMonitoring();

  // Check if monitoring is currently active.
  // @return true if monitoring is running
  bool isMonitoring() const;

  // Get the current bus statistics.
  // @return BusStats structure
  BusStats currentStats() const;

  // Get the current bus statistics as JSON.
  // @return JSON object with statistics
  QJsonObject currentStatsJson() const;

signals:
  // Emitted when bus statistics are updated.
  // @param stats  JSON object with updated statistics
  void statsUpdated(const QJsonObject &stats);

  // Emitted when an error occurs.
  // @param msg  Human-readable error message
  void error(const QString &msg);

private:
  // Poll daemon for bus statistics.
  void poll();

  EcatClient *client_;          // TCP client to ecatd daemon
  QTimer *pollTimer_ = nullptr; // Timer for periodic polling
  BusStats stats_;              // Current bus statistics
  quint64 prevTxFrames_ = 0;   // Previous TX frame count (for rate calculation)
  quint64 prevRxFrames_ = 0;   // Previous RX frame count (for rate calculation)
};
