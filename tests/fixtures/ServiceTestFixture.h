#pragma once

/// @brief Test fixture for service-layer tests with simulated EtherCAT events.
///
/// @details ServiceTestFixture provides a convenient way to test services
/// that depend on EcatClient and EventBus. It creates a ServiceContainer
/// and provides methods to simulate daemon events (connection state changes,
/// slave list updates, SDO value responses) without requiring a running
/// ecatd daemon.
///
/// Usage:
/// @code
///   ServiceTestFixture fixture;
///   fixture.simulateConnection(true);
///   fixture.simulateSlaveChange(0, "Slave0", "OP");
///   fixture.simulateSdoValue(0, "0x6040", "0x00", "0x000f");
///   // Assert service state...
/// @endcode
///
/// @par Test Coverage
///   - Service initialization with mock dependencies
///   - Connection state change handling
///   - Slave list update processing
///   - SDO value response handling
///
/// @par Dependencies
///   - Qt6::Core, Qt6::Widgets
///   - ServiceContainer, MockEcatClient, MockEventBus
///
/// @see ServiceContainer, MockEcatClient, MockEventBus

#include <QObject>

class EcatClient;
class MockEcatClient;
class MockEventBus;
class ServiceContainer;

class ServiceTestFixture : public QObject {
    Q_OBJECT
public:
    /// Constructs the fixture, creating a ServiceContainer with mock dependencies.
    explicit ServiceTestFixture(QObject *parent = nullptr);
    /// Destroys the fixture and all owned objects.
    ~ServiceTestFixture() override;

    /// Returns the ServiceContainer instance (never null after construction).
    ServiceContainer *container() const;

    /// Simulates a daemon connection state change.
    /// @param connected  true for connected, false for disconnected
    void simulateConnection(bool connected);
    /// Simulates a slave list update with a single slave entry.
    /// @param position  Slave position on the bus
    /// @param name      Slave name
    /// @param state     Slave state (INIT, PREOP, SAFEOP, OP)
    void simulateSlaveChange(int position, const QString &name, const QString &state);
    /// Simulates an SDO value response from the daemon.
    /// @param position  Slave position
    /// @param index     SDO index (e.g. "0x6040")
    /// @param subIndex  SDO subindex (e.g. "0x00")
    /// @param value     SDO value string
    void simulateSdoValue(int position, const QString &index, const QString &subIndex, const QString &value);

private:
    ServiceContainer *container_ = nullptr; ///< Service container with mock dependencies
    EcatClient *client_ = nullptr;
};
