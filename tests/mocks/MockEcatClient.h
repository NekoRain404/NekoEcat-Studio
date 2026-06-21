#pragma once

/// @brief Mock EcatClient for unit testing without a running ecatd daemon.
///
/// @details MockEcatClient extends EcatClient to provide a testable interface
/// that records all method calls and allows configuring canned responses.
/// This enables testing of services and plugins that depend on EcatClient
/// without requiring network connectivity or a running daemon.
///
/// Features:
///   - **Call recording**: All method calls (scan, upload, download, etc.)
///     are recorded in a vector for later assertion.
///   - **Canned responses**: Pre-configure scan results, SDO values, and
///     error conditions before exercising the code under test.
///   - **Signal triggering**: Manually emit EcatClient signals (slavesChanged,
///     sdoValue, error) to simulate daemon responses.
///   - **Connection state**: Control the connected/disconnected state.
///
/// Usage:
/// @code
///   MockEcatClient client;
///   client.setConnected(true);
///   client.setScanResult({slave1, slave2});
///   client.setSdoResult(0, "0x6040", "0x00", "0x000f");
///
///   // Exercise code that calls client methods...
///   QCOMPARE(client.callCount("scan"), 1);
///   QCOMPARE(client.calls().first().method, QString("scan"));
///
///   // Trigger signals to simulate daemon responses:
///   client.triggerSlavesChanged({slave1, slave2});
///   client.triggerSdoValue(0, "0x6040", "0x00", "0x000f");
/// @endcode
///
/// @par Test Coverage
///   - Method call recording and counting
///   - Configurable scan/SDO/error responses
///   - Signal emission simulation
///   - Connection state management
///
/// @see EcatClient, ServiceContainer, EventBus

#include "infra/EcatClient.h"

#include <QJsonObject>
#include <QVector>
#include <functional>

/// Records a single method call to the mock client.
struct MethodCall {
    QString method;     ///< The method name (e.g. "scan", "upload", "download")
    QJsonObject params; ///< The JSON parameters passed to the method
};

class MockEcatClient : public EcatClient {
    Q_OBJECT
public:
    explicit MockEcatClient(QObject *parent = nullptr);

    // ── Response Configuration ───────────────────────────────────
    /// Sets the simulated connection state.
    void setConnected(bool connected);
    /// Configures the canned response for scan() calls.
    void setScanResult(const QVector<SlaveInfo> &slaves);
    /// Configures the canned response for upload() calls.
    void setSdoResult(int pos, const QString &idx, const QString &sub, const QString &value);
    /// Configures an error to be returned on the next call.
    void setErrorOnNext(const QString &error);

    // ── Call Inspection ──────────────────────────────────────────
    /// Returns all recorded method calls.
    QVector<MethodCall> calls() const;
    /// Returns the number of times a specific method was called.
    int callCount(const QString &method) const;
    /// Clears all recorded method calls.
    void clearCalls();

    // ── Custom Handlers ──────────────────────────────────────────
    /// Handler type for custom scan behavior.
    using ScanHandler = std::function<void()>;
    /// Sets a custom handler invoked on scan() calls.
    void setScanHandler(ScanHandler handler);

    // ── Signal Triggers ──────────────────────────────────────────
    /// Manually emits the slavesChanged signal (simulates daemon scan response).
    void triggerSlavesChanged(const QVector<SlaveInfo> &slaves);
    /// Manually emits the sdoValue signal (simulates daemon SDO read response).
    void triggerSdoValue(int pos, const QString &idx, const QString &sub, const QString &val);
    /// Manually emits the error signal (simulates daemon error).
    void triggerError(const QString &msg);

private:
    bool connected_ = false;              ///< Simulated connection state
    QVector<SlaveInfo> scanResult_;       ///< Canned scan response
    QVector<MethodCall> calls_;           ///< Recorded method calls
    QString pendingError_;                ///< Error to return on next call
    ScanHandler scanHandler_;             ///< Custom scan handler
};
