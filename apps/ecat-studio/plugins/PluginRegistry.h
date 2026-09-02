#pragma once
// =============================================================================
// PluginRegistry — Central registry for workspace plugin management
// =============================================================================
//
// Overview:
//   PluginRegistry manages all workspace plugins in NekoEcat Studio. It provides
//   ordered access, lookup by id, and visibility filtering. The registry ensures
//   plugins are properly validated before registration and maintains them in
//   sorted order for consistent tab ordering in the UI.
//
// Plugin Management:
//   - Registration: Adds plugins to the registry with validation
//   - Ordering: Plugins are sorted by defaultOrder() ascending after registration
//   - Lookup: O(1) access by index, O(log n) lookup by id
//   - Filtering: Provides filtered lists of visible plugins for UI rendering
//   - Error Reporting: Emits signals for registration failures
//
// Registration Process:
//   1. Plugin is validated (not null, non-empty id, no duplicate id)
//   2. Plugin is added to the internal vector and map
//   3. Vector is sorted by defaultOrder() to maintain consistent tab order
//   4. MainWindow uses the registry to create tabs in the correct order
//
// Usage Example:
//   // Registering a plugin:
//   pluginRegistry_->registerPlugin(new MyPlugin(serviceContainer, this));
//
//   // Iterating plugins in order:
//   for (int i = 0; i < pluginRegistry_->count(); ++i) {
//       auto *plugin = pluginRegistry_->pluginAt(i);
//       if (plugin->visible()) {
//           tabWidget->addTab(plugin->widget(), plugin->displayName());
//       }
//   }
//
//   // Looking up a plugin by id:
//   auto *odPlugin = pluginRegistry_->findById("od");
//   if (odPlugin) {
//       odPlugin->activate();
//   }
//
// Thread Safety:
//   All access is expected from the main (GUI) thread only. The registry
//   is created once during MainWindow initialization and is not modified
//   after the initial plugin registration phase.
//
// Internal Storage:
//   - QVector<WorkspacePlugin*>: Ordered traversal (sorted by defaultOrder)
//   - QMap<QString, WorkspacePlugin*>: O(log n) id-based lookup
//   - Both containers store the same plugin pointers (redundant for performance)

#include <QMap>
#include <QObject>
#include <QString>
#include <QVector>

class WorkspacePlugin;

class PluginRegistry : public QObject {
    Q_OBJECT
public:
    explicit PluginRegistry(QObject* parent = nullptr) : QObject(parent) {}

    // ── Registration ─────────────────────────────────────────────────────
    // Adds a plugin to the registry with validation.
    // Guards against:
    //   - null plugin pointer
    //   - empty plugin id
    //   - duplicate plugin id (silently ignored)
    // After registration, plugins are sorted by defaultOrder() ascending.
    // Emits registrationFailed() on validation failures.
    bool registerPlugin(WorkspacePlugin* plugin);

    // ── Access Methods ───────────────────────────────────────────────────

    /// Returns the total number of registered plugins (including hidden ones).
    int count() const;

    /// Returns the plugin at the given index in sorted order, or nullptr if out of range.
    /// Index 0 is the plugin with the lowest defaultOrder() value.
    WorkspacePlugin* pluginAt(int index) const;

    /// Finds a plugin by its unique id string, or nullptr if not found.
    /// Uses QMap for O(log n) lookup performance.
    WorkspacePlugin* findById(const QString& id) const;

    /// Returns a list of all plugins where visible() returns true.
    /// This is used by MainWindow to create only the visible tabs.
    QVector<WorkspacePlugin*> visiblePlugins() const;

signals:
    // Emitted when a plugin registration fails validation.
    // @param reason  Human-readable description of why registration failed
    // @param pluginId  The plugin id (may be empty if plugin was null)
    void registrationFailed(const QString& reason, const QString& pluginId);

private:
    // Internal storage — both containers hold the same plugin pointers.
    // plugins_ is sorted by defaultOrder() for ordered traversal.
    // idMap_ provides O(log n) lookup by plugin id.
    QVector<WorkspacePlugin*> plugins_;
    QMap<QString, WorkspacePlugin*> idMap_;
};
