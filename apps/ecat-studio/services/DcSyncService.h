#pragma once

// DcSyncService — polls the daemon for DC sync diagnostics and emits
// parsed results.  Intended for use by DcSyncPlugin to populate its
// workspace table with per-slave timing information.
//
// This service provides Distributed Clock (DC) synchronization diagnostics
// for the EtherCAT network. It handles:
//   - Periodic polling of DC sync status from the daemon
//   - Single-shot status requests
//   - Parsing of DC timing data (reference clock, drift, jitter)
//   - Per-slave synchronization state tracking
//
// Usage:
//   ServiceContainer *container = ...;
//   DcSyncService *dcSync = container->dcSync();
//   dcSync->startPolling(2000);  // Poll every 2 seconds
//   dcSync->requestUpdate();     // Single request
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   uses a QTimer for periodic polling, which runs on the main thread.
//
// Performance:
//   - Default polling interval is 2000ms (configurable)
//   - Each poll sends a single JSON request to the daemon
//   - Response parsing is O(n) where n is number of slaves

#include <QJsonObject>
#include <QObject>

class QTimer;
class EcatClient;

class DcSyncService : public QObject {
    Q_OBJECT
public:
    explicit DcSyncService(EcatClient* client, QObject* parent = nullptr);

    // Start periodic polling of DC sync status.
    // @param intervalMs  Polling interval in milliseconds (default: 2000ms)
    void startPolling(int intervalMs = 2000);

    // Stop periodic polling.
    void stopPolling();

    // Issue a single DC sync status request to the daemon.
    // Emits dcSyncUpdate() on success, error() on failure.
    void requestUpdate();

    // Query DC configuration from a slave's ESI XML descriptor.
    // @param position  Slave index on the bus
    // Emits dcConfigureResult() on success, error() on failure.
    void configure(int position);

    // Activate DC synchronization with the specified reference clock slave.
    // @param refClockSlave  Slave index to use as reference clock (-1 for auto-detect)
    // Emits dcActivateResult() on success, error() on failure.
    void activate(int refClockSlave);

    // Deactivate DC synchronization.
    // Emits dcDeactivateResult() on success, error() on failure.
    void deactivate();

signals:
    // Emitted after each successful daemon response with DC sync data.
    // @param data  JSON object containing per-slave DC sync information
    void dcSyncUpdate(const QJsonObject& data);

    // Emitted after a successful dcConfigure response with DC parameters.
    // @param data  JSON object containing parsed DC configuration
    void dcConfigureResult(const QJsonObject& data);

    // Emitted after a successful dcActivate response.
    // @param data  JSON object containing activation status
    void dcActivateResult(const QJsonObject& data);

    // Emitted after a successful dcDeactivate response.
    // @param data  JSON object containing deactivation status
    void dcDeactivateResult(const QJsonObject& data);

    // Emitted when a DC sync request fails.
    // @param message  Human-readable error description
    void error(const QString& message);

    // Emitted when polling cannot be started due to invalid parameters.
    // @param reason  Human-readable rejection reason
    void pollingRejected(const QString& reason);

private:
    EcatClient* client_; // TCP client to ecatd daemon
    QTimer* pollTimer_;  // Timer for periodic polling
};
