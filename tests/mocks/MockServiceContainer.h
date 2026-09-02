#pragma once

/// @brief Mock ServiceContainer for testing with mock EcatClient and EventBus.
///
/// @details MockServiceContainer extends ServiceContainer to replace the real
/// EcatClient and EventBus with mock implementations. This enables testing
/// of services and plugins that depend on these components without requiring
/// network connectivity or a running daemon.
///
/// The mock container creates MockEcatClient and MockEventBus instances
/// that can be accessed via mockClient() and mockEventBus() for configuring
/// test scenarios and asserting signal emissions.
///
/// Usage:
/// @code
///   MockServiceContainer container;
///   container.mockClient()->setConnected(true);
///   container.mockClient()->setScanResult({slave1, slave2});
///   container.mockEventBus()->clearRecords();
///   // Exercise code that uses the container...
///   QCOMPARE(container.mockEventBus()->signalCount("slaveChanged"), 1);
/// @endcode
///
/// @par Test Coverage
///   - Service initialization with mock dependencies
///   - Mock client configuration and call recording
///   - Mock event bus signal recording
///
/// @see ServiceContainer, MockEcatClient, MockEventBus

#include "services/ServiceContainer.h"

class MockEcatClient;
class MockEventBus;

class MockServiceContainer : public ServiceContainer {
    Q_OBJECT
public:
    /// Constructs the container with MockEcatClient and MockEventBus.
    explicit MockServiceContainer(QObject* parent = nullptr);
    /// Destroys the container and all owned mock objects.
    ~MockServiceContainer() override;

    /// Returns the MockEcatClient instance for configuring test scenarios.
    MockEcatClient* mockClient() const;
    /// Returns the MockEventBus instance for verifying signal emissions.
    MockEventBus* mockEventBus() const;

private:
    MockEcatClient* mockClient_ = nullptr; ///< Mock EcatClient instance
    MockEventBus* mockEventBus_ = nullptr; ///< Mock EventBus instance
};
