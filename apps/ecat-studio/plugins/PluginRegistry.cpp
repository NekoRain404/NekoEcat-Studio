#include "PluginRegistry.h"
#include "WorkspacePlugin.h"
#include <algorithm>

void PluginRegistry::registerPlugin(WorkspacePlugin *plugin) {
  if (!plugin || plugin->id().isEmpty()) return;
  if (idMap_.contains(plugin->id())) return;
  plugins_.append(plugin);
  idMap_.insert(plugin->id(), plugin);
  std::sort(plugins_.begin(), plugins_.end(), [](const WorkspacePlugin *a, const WorkspacePlugin *b) {
    return a->defaultOrder() < b->defaultOrder();
  });
}

int PluginRegistry::count() const { return plugins_.size(); }

WorkspacePlugin *PluginRegistry::pluginAt(int index) const {
  if (index < 0 || index >= plugins_.size()) return nullptr;
  return plugins_[index];
}

WorkspacePlugin *PluginRegistry::findById(const QString &id) const {
  return idMap_.value(id, nullptr);
}

QVector<WorkspacePlugin *> PluginRegistry::visiblePlugins() const {
  QVector<WorkspacePlugin *> result;
  for (auto *p : plugins_) {
    if (p->visible()) result.append(p);
  }
  return result;
}
