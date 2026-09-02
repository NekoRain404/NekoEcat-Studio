// =============================================================================
// PluginRegistry.cpp — Plugin registration, ordering, and lookup
// =============================================================================
//
// This file implements the PluginRegistry class, which manages all workspace
// plugins in NekoEcat Studio.
//
// Implementation Details:
//   - Registration validates plugin before adding (null, empty id, duplicate)
//   - Plugins are stored in a QVector sorted by defaultOrder()
//   - A QMap provides O(log n) id-based lookup
//   - Both containers store the same plugin pointers (redundant for performance)
//   - Registration failures emit registrationFailed() signal for diagnostics
//
// Plugin Ordering:
//   After each registration, the plugins_ vector is sorted by defaultOrder()
//   ascending. This ensures consistent tab ordering in the UI. The sort uses
//   std::sort with a lambda comparator that compares defaultOrder() values.
//
// Visibility Filtering:
//   The visiblePlugins() method iterates the sorted plugins_ vector and
//   collects only those where visible() returns true. This allows plugins
//   to dynamically show/hide based on application state.
//
// Thread Safety:
//   All methods are designed to be called from the main (GUI) thread only.
//   The registry is populated during MainWindow initialization and is
//   effectively read-only after that phase.

#include "PluginRegistry.h"
#include "WorkspacePlugin.h"
#include <algorithm>

// ── Registration ───────────────────────────────────────────────────────
// Adds a plugin to the registry with validation.
// Guards against:
//   - null plugin pointer
//   - empty plugin id
//   - duplicate plugin id (silently ignored)
// After registration, plugins are sorted by defaultOrder() ascending.
// Returns true on success, false on validation failure.
bool PluginRegistry::registerPlugin(WorkspacePlugin* plugin) {
    // Validate plugin — reject null pointers and empty ids
    if (!plugin) {
        emit registrationFailed("Null plugin pointer", QString());
        return false;
    }
    if (plugin->id().isEmpty()) {
        emit registrationFailed("Plugin has empty id", "(unnamed)");
        return false;
    }

    // Reject duplicate ids — first registration wins
    if (idMap_.contains(plugin->id())) {
        emit registrationFailed(QString("Duplicate plugin id '%1' — keeping first registration").arg(plugin->id()),
                                plugin->id());
        return false;
    }

    // Add to both containers (vector for ordering, map for lookup)
    plugins_.append(plugin);
    idMap_.insert(plugin->id(), plugin);

    // Sort by defaultOrder() to maintain consistent tab ordering
    // Lower defaultOrder() values appear first (leftmost tabs)
    std::sort(plugins_.begin(), plugins_.end(),
              [](const WorkspacePlugin* a, const WorkspacePlugin* b) { return a->defaultOrder() < b->defaultOrder(); });
    return true;
}

// ── Access Methods ─────────────────────────────────────────────────────

/// Returns the total number of registered plugins (including hidden ones).
int PluginRegistry::count() const {
    return plugins_.size();
}

/// Returns the plugin at the given index in sorted order, or nullptr if out of range.
/// Index 0 is the plugin with the lowest defaultOrder() value.
WorkspacePlugin* PluginRegistry::pluginAt(int index) const {
    if (index < 0 || index >= plugins_.size())
        return nullptr;
    return plugins_[index];
}

/// Finds a plugin by its unique id string, or nullptr if not found.
/// Uses QMap for O(log n) lookup performance.
WorkspacePlugin* PluginRegistry::findById(const QString& id) const {
    return idMap_.value(id, nullptr);
}

/// Returns a list of all plugins where visible() returns true.
/// This is used by MainWindow to create only the visible tabs.
/// The returned list is in sorted order (by defaultOrder ascending).
QVector<WorkspacePlugin*> PluginRegistry::visiblePlugins() const {
    QVector<WorkspacePlugin*> result;
    for (auto* p : plugins_) {
        if (p->visible())
            result.append(p);
    }
    return result;
}
