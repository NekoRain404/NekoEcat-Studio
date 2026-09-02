#pragma once

/// @brief Mock EventBus for testing inter-plugin communication without real signals.
///
/// @details MockEventBus extends EventBus to record all emitted signals and
/// their arguments. This enables testing of event-driven code by verifying
/// that the correct events were emitted with the expected payloads.
///
/// Features:
///   - **Signal recording**: All EventBus signals are recorded with their
///     name and argument list for later assertion.
///   - **Signal counting**: Query how many times a specific signal was emitted.
///   - **Configurable delay**: Simulate signal processing delays for
///     timing-sensitive tests.
///   - **Clear records**: Reset the recording state between test cases.
///
/// Usage:
/// @code
///   MockEventBus bus;
///   bus.emitSlaveChanged({slave1, slave2});
///   QCOMPARE(bus.signalCount("slaveChanged"), 1);
///   QCOMPARE(bus.records().first().name, QString("slaveChanged"));
///   bus.clearRecords();
/// @endcode
///
/// @par Test Coverage
///   - Signal emission verification
///   - Argument capture and validation
///   - Signal ordering and frequency
///
/// @see EventBus, MockServiceContainer

#include "services/EventBus.h"

#include <QMap>
#include <QVector>

/// Records a single signal emission with its name and arguments.
struct SignalRecord {
    QString name;      ///< Signal name (e.g. "slaveChanged", "sdoValueReceived")
    QVariantList args; ///< Signal arguments as QVariant list
};

class MockEventBus : public EventBus {
    Q_OBJECT
public:
    explicit MockEventBus(QObject* parent = nullptr);

    /// Returns all recorded signal emissions.
    QVector<SignalRecord> records() const;
    /// Returns the number of times a specific signal was emitted.
    int signalCount(const QString& name) const;
    /// Clears all recorded signals.
    void clearRecords();
    /// Sets an artificial delay (in ms) before recording each signal.
    void setDelayMs(int ms);

private:
    /// Records a signal emission with its name and arguments.
    void recordSignal(const QString& name, const QVariantList& args);

    QVector<SignalRecord> records_; ///< All recorded signal emissions
    int delayMs_ = 0;               ///< Artificial delay before recording (ms)

    /// Connects all EventBus signals to recording slots.
    void connectSignals();

private slots:
    void onSlaveChanged(const QVector<SlaveInfo>& slaves);
    void onSdoValueReceived(int pos, const QString& idx, const QString& sub, const QString& val);
    void onConnectionStateChanged(bool connected);
    void onTopologyChanged(const QVector<SlaveInfo>& slaves);
};
