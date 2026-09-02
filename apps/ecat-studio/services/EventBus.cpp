// =============================================================================
// EventBus.cpp — Central event hub implementation
// =============================================================================
//
// This file implements the EventBus class, which provides a publish/subscribe
// mechanism for inter-plugin communication in NekoEcat Studio.
//
// Implementation Details:
//   - Each emitXxx() method is a thin wrapper around the corresponding Qt signal
//   - Qt's signal-slot mechanism handles:
//     * Type-safe dispatch
//     * Automatic connection management
//     * Thread-safe delivery (with queued connections for cross-thread)
//   - No filtering or routing logic — all subscribers receive all events
//   - Signal parameters use const references to minimize data copying
//
// Performance Characteristics:
//   - Signal emission is O(n) where n = number of connected slots
//   - For high-frequency events (e.g., signalData at 1kHz), producers
//     should consider batching or throttling
//   - Qt's direct connections have ~nanosecond overhead per emission
//
// Thread Safety:
//   All emit methods should be called from the main (GUI) thread.
//   Services that perform background I/O must marshal results to the
//   main thread before calling emit methods.

#include "EventBus.h"

// Constructor — no special initialization needed beyond QObject parent.
EventBus::EventBus(QObject* parent) : QObject(parent) {}

// ── Emitter Method Implementations ─────────────────────────────────────
// Each method simply emits the corresponding Qt signal. The separation of
// emit methods from signals provides a clean API boundary and allows future
// enhancements (e.g., logging, metrics) without changing the signal interface.

// Topology scan results available (slave list updated)
void EventBus::emitSlaveChanged(const QVector<SlaveInfo>& slaves) {
    emit slaveChanged(slaves);
}

// SDO value read from a slave device
void EventBus::emitSdoValue(int pos, const QString& idx, const QString& sub, const QString& val) {
    emit sdoValueReceived(pos, idx, sub, val);
}

// Daemon connection state changed (connected/disconnected)
void EventBus::emitConnectionStateChanged(bool connected) {
    emit connectionStateChanged(connected);
}

// Free Run process data telemetry snapshot
void EventBus::emitFreeRunTelemetry(const QJsonObject& tel) {
    emit freeRunTelemetry(tel);
}

// Topology change detected (diff from baseline)
void EventBus::emitTopologyChanged(const QVector<SlaveInfo>& slaves) {
    emit topologyChanged(slaves);
}

// DC sync status update per slave
void EventBus::emitDcSyncUpdate(const QJsonObject& data) {
    emit dcSyncUpdate(data);
}

// Application Layer event log entry
void EventBus::emitAlEvent(const QJsonObject& event) {
    emit alEvent(event);
}

// Multi-channel signal data for real-time visualization
void EventBus::emitSignalData(int channel, const QVector<double>& values, const QVector<qint64>& timestamps) {
    emit signalData(channel, values, timestamps);
}
