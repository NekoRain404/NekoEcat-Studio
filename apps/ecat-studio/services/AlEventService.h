#pragma once

// AlEventService — polls the ecatd daemon for AL event log entries and emits
// parsed results.  Also supports clearing the remote event log.  Intended for
// use by AlEventPlugin to populate its workspace table.
//
// This service provides Application Layer (AL) event monitoring for the
// EtherCAT network. It handles:
//   - Periodic polling of AL event log entries from the daemon
//   - Single-shot event log requests
//   - Clearing the remote event log
//   - Parsing event data (timestamps, error codes, severity levels)
//
// Usage:
//   ServiceContainer *container = ...;
//   AlEventService *alEvent = container->alEvent();
//   alEvent->startPolling(2000);  // Poll every 2 seconds
//   alEvent->requestUpdate();     // Single request
//   alEvent->clearEvents();       // Clear event log
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   uses a QTimer for periodic polling, which runs on the main thread.
//
// Performance:
//   - Default polling interval is 2000ms (configurable)
//   - Each poll sends a single JSON request to the daemon
//   - Event log is limited by daemon-side buffer size

#include <QJsonObject>
#include <QObject>

class QTimer;
class EcatClient;

class AlEventService : public QObject {
    Q_OBJECT
public:
    explicit AlEventService(EcatClient* client, QObject* parent = nullptr);

    // Start periodic polling of AL event log.
    // @param intervalMs  Polling interval in milliseconds (default: 2000ms)
    void startPolling(int intervalMs = 2000);

    // Stop periodic polling.
    void stopPolling();

    // Issue a single event log request to the daemon.
    // Emits alEventUpdate() on success, error() on failure.
    void requestUpdate();

    // Clear all AL events on the daemon side.
    // Emits alEventUpdate() with empty log on success.
    void clearEvents();

signals:
    // Emitted after each successful daemon log response.
    // @param data  JSON object containing AL event log entries
    void alEventUpdate(const QJsonObject& data);

    // Emitted when an AL event request fails.
    // @param message  Human-readable error description
    void error(const QString& message);

private:
    EcatClient* client_; // TCP client to ecatd daemon
    QTimer* pollTimer_;  // Timer for periodic polling
};
