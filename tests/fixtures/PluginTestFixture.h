#pragma once

/// @brief Test fixture providing a ServiceContainer and PluginRegistry for plugin tests.
///
/// @details This fixture sets up the minimal infrastructure needed to test
/// WorkspacePlugin implementations in isolation. It creates a ServiceContainer
/// (with all services) and a PluginRegistry, and provides convenience methods
/// for plugin lifecycle operations.
///
/// Usage:
/// @code
///   PluginTestFixture fixture;
///   auto *plugin = new MyPlugin(fixture.container());
///   fixture.registerPlugin(plugin);
///   QCOMPARE(fixture.pluginCount(), 1);
///   fixture.activatePlugin("myplugin");
/// @endcode
///
/// @par Test Coverage
///   - Plugin registration and lookup
///   - Plugin activation/deactivation lifecycle
///   - Service container access
///
/// @par Dependencies
///   - Qt6::Core, Qt6::Widgets
///   - PluginRegistry, ServiceContainer, WorkspacePlugin
///
/// @see PluginRegistry, ServiceContainer, WorkspacePlugin

#include "plugins/PluginRegistry.h"
#include "services/ServiceContainer.h"

#include <QObject>
#include <QWidget>

class EcatClient;
class EventBus;
class WorkspacePlugin;

class PluginTestFixture : public QObject {
    Q_OBJECT
public:
    /// Constructs the fixture, creating a ServiceContainer and PluginRegistry.
    explicit PluginTestFixture(QObject* parent = nullptr);
    /// Destroys the fixture and all owned objects.
    ~PluginTestFixture() override;

    /// Returns the ServiceContainer instance (never null after construction).
    ServiceContainer* container() const;
    /// Returns the PluginRegistry instance (never null after construction).
    PluginRegistry* registry() const;

    /// Registers a plugin with the internal registry.
    /// @param plugin  The plugin to register (must not be null)
    void registerPlugin(WorkspacePlugin* plugin);
    /// Activates a plugin by its id (calls WorkspacePlugin::activate()).
    /// @param id  The plugin id to activate
    void activatePlugin(const QString& id);
    /// Deactivates a plugin by its id (calls WorkspacePlugin::deactivate()).
    /// @param id  The plugin id to deactivate
    void deactivatePlugin(const QString& id);

    /// Finds a registered plugin by id, or nullptr if not found.
    WorkspacePlugin* findPlugin(const QString& id) const;
    /// Returns the total number of registered plugins.
    int pluginCount() const;

private:
    ServiceContainer* container_ = nullptr; ///< Service container instance
    PluginRegistry* registry_ = nullptr;    ///< Plugin registry instance
    EcatClient* client_ = nullptr;
};
