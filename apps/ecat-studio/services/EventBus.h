#pragma once

// =============================================================================
// EventBus — Central event hub for decoupled inter-plugin communication
// =============================================================================
//
// Overview:
//   EventBus implements a publish/subscribe (pub/sub) pattern for inter-plugin
//   communication. Instead of plugins holding direct references to each other,
//   they emit and subscribe to events through this central bus. This creates
//   loose coupling between plugins, enabling independent development and testing.
//
// Architecture:
//   EventBus acts as a mediator between producers (services that generate data)
//   and consumers (plugins that display or react to data). It uses Qt's
//   signal-slot mechanism under the hood, providing type-safe, thread-safe
//   event dispatch.
//
//   Producer (e.g., SdoService)                    Consumer (e.g., OdPlugin)
//          │                                              │
//          │  emitSdoValue(pos, idx, sub, val)            │  connect(bus, &EventBus::sdoValueReceived, ...)
//          ▼                                              ▲
//   ┌─────────────────────────────────────────────────────────────────────┐
//   │                           EventBus                                 │
//   │   - Receives emit calls from producers                             │
//   │   - Dispatches Qt signals to all connected slots                   │
//   │   - No filtering, no routing — all subscribers receive all events  │
//   └─────────────────────────────────────────────────────────────────────┘
//
// Event Catalogue:
//   ┌────────────────────────┬─────────────────────────┬──────────────────┬────────────────────────┐
//   │ Signal                 │ Data                    │ Typical Producer │ Typical Consumer       │
//   ├────────────────────────┼─────────────────────────┼──────────────────┼────────────────────────┤
//   │ slaveChanged           │ QVector<SlaveInfo>      │ EcatClient       │ Topology, StateMachine │
//   │ sdoValueReceived       │ pos, idx, sub, value    │ SdoService       │ OD, Watch, IoVariable  │
//   │ connectionStateChanged │ bool connected          │ EcatClient       │ All workspace plugins  │
//   │ freeRunTelemetry       │ QJsonObject             │ EcatClient       │ FreeRun, Signal        │
//   │ topologyChanged        │ QVector<SlaveInfo>      │ TopologyService  │ Topology, Diagnostics  │
//   │ dcSyncUpdate           │ QJsonObject             │ DcSyncService    │ DcSync                 │
//   │ alEvent                │ QJsonObject             │ AlEventService   │ AlEvent                │
//   │ signalData             │ ch, values[], timestamps│ SignalService    │ Signal chart widget    │
//   └────────────────────────┴─────────────────────────┴──────────────────┴────────────────────────┘
//
// Usage Example:
//   // Publishing an event:
//   container_->eventBus()->emitSlaveChanged(slaveList);
//
//   // Subscribing to an event:
//   connect(container_->eventBus(), &EventBus::slaveChanged,
//           this, [this](const QVector<SlaveInfo> &slaves) {
//               updateSlaveTable(slaves);
//           });
//
// Thread Safety:
//   EventBus uses Qt's direct signal-slot connections for same-thread dispatch.
//   All event emission and subscription should happen on the main (GUI) thread.
//   Services that do background I/O must marshal results to the main thread
//   before emitting through EventBus.
//
// Performance Notes:
//   - Signal emission is O(n) where n is the number of connected slots
//   - Use const references in signal parameters to minimize data copying
//   - For high-frequency events (e.g., signalData at 1kHz), consider
//     batching or throttling in the producer service

#include <QObject>
#include <QJsonObject>
#include <QVector>
#include <QString>
#include "EthercatTypes.h"

class EventBus : public QObject {
  Q_OBJECT
public:
  // ── Construction ─────────────────────────────────────────────────────
  explicit EventBus(QObject *parent = nullptr);

  // ── Emitter Methods (Convenience Wrappers) ───────────────────────────
  // These methods provide type-safe wrappers around Qt signal emission.
  // Call these instead of emitting signals directly to maintain a clean API.

  /// Emitted when a topology scan completes and slave list is available.
  /// @param slaves  Current slave list from the bus scan
  /// @note  Producers: TopologyService, EcatClient polling
  /// @note  Consumers: TopologyPlugin, StateMachinePlugin, OdPlugin
  void emitSlaveChanged(const QVector<SlaveInfo> &slaves);

  /// Emitted when an SDO value is read from a slave device.
  /// @param pos  Slave position on the bus
  /// @param idx  SDO index (hex string, e.g., "0x1000")
  /// @param sub  SDO sub-index (hex string, e.g., "0x00")
  /// @param val  Read value as string
  /// @note  Producers: SdoService
  /// @note  Consumers: OdPlugin, WatchPlugin, IoVariablePlugin
  void emitSdoValue(int pos, const QString &idx, const QString &sub, const QString &val);

  /// Emitted when the connection to ecatd daemon changes state.
  /// @param connected  true if connected, false if disconnected
  /// @note  Producers: EcatClient
  /// @note  Consumers: All workspace plugins (for UI state updates)
  void emitConnectionStateChanged(bool connected);

  /// Emitted with Free Run process data telemetry at ~1kHz.
  /// @param tel  JSON object containing input/output process values
  /// @note  Producers: EcatClient (via FreeRunController polling)
  /// @note  Consumers: FreeRunPlugin, SignalPlugin
  void emitFreeRunTelemetry(const QJsonObject &tel);

  /// Emitted when the topology differs from the stored baseline.
  /// @param slaves  Current slave list showing changes
  /// @note  Producers: TopologyService (on rescan or periodic check)
  /// @note  Consumers: TopologyPlugin, DiagnosticsPlugin
  void emitTopologyChanged(const QVector<SlaveInfo> &slaves);

  /// Emitted with DC sync diagnostics data per slave.
  /// @param data  JSON object with per-slave sync status, drift, jitter
  /// @note  Producers: DcSyncService (polled every 2s)
  /// @note  Consumers: DcSyncPlugin
  void emitDcSyncUpdate(const QJsonObject &data);

  /// Emitted when an Application Layer event is logged.
  /// @param event  JSON object with timestamp, error code, severity
  /// @note  Producers: AlEventService (polled every 1s)
  /// @note  Consumers: AlEventPlugin, DiagnosticsPlugin
  void emitAlEvent(const QJsonObject &event);

  /// Emitted with multi-channel signal data for real-time visualization.
  /// @param channel    Signal channel index
  /// @param values     Sample values for this channel
  /// @param timestamps Timestamps (ms since epoch) for each sample
  /// @note  Producers: SignalService
  /// @note  Consumers: SignalPlugin (chart rendering)
  void emitSignalData(int channel, const QVector<double> &values, const QVector<qint64> &timestamps);

signals:
  // ── Qt Signals ───────────────────────────────────────────────────────
  // These are the actual Qt signals that subscribers connect to.
  // Each emitXxx() method above calls the corresponding signal below.

  /// Topology scan results available (slave list updated)
  void slaveChanged(const QVector<SlaveInfo> &slaves);

  /// SDO value read from a slave device
  void sdoValueReceived(int position, const QString &index, const QString &subIndex, const QString &value);

  /// Daemon connection state changed (connected/disconnected)
  void connectionStateChanged(bool connected);

  /// Free Run process data telemetry snapshot
  void freeRunTelemetry(const QJsonObject &telemetry);

  /// Topology change detected (diff from baseline)
  void topologyChanged(const QVector<SlaveInfo> &slaves);

  /// DC sync status update per slave
  void dcSyncUpdate(const QJsonObject &data);

  /// Application Layer event log entry
  void alEvent(const QJsonObject &event);

  /// Multi-channel signal data for real-time visualization
  void signalData(int channel, const QVector<double> &values, const QVector<qint64> &timestamps);
};
