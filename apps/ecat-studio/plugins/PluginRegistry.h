#pragma once
// PluginRegistry — manages all workspace plugins.
// Provides ordered access, lookup by id, and visibility filtering.
#include <QVector>
#include <QMap>
#include <QString>

class WorkspacePlugin;

class PluginRegistry {
public:
  void registerPlugin(WorkspacePlugin *plugin);
  int count() const;
  WorkspacePlugin *pluginAt(int index) const;
  WorkspacePlugin *findById(const QString &id) const;
  QVector<WorkspacePlugin *> visiblePlugins() const;
private:
  QVector<WorkspacePlugin *> plugins_;
  QMap<QString, WorkspacePlugin *> idMap_;
};
