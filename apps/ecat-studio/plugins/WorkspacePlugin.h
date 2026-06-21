#pragma once

// WorkspacePlugin — abstract interface for every workspace tab in NekoEcat Studio.
//
// Each workspace (Overview, Object Dictionary, Watch, Startup SDO, Free Run,
// I/O Variables, Consistency, State Machine, Diagnostics, DC Sync, AL Events,
// Signal Analyzer, RT Test, Export, Bus Stats, ESI, Session, Topology, Notes)
// implements this interface and registers with PluginRegistry.
//
// Interface contract:
//   Identity:     id() returns a unique short string (e.g. "od", "watch").
//                 displayName() / displayNameZh() provide bilingual tab labels.
//                 icon() returns an optional QIcon for the tab bar.
//   UI:           widget() returns the workspace's root QWidget. The plugin
//                 owns the widget and must keep it alive for the plugin's
//                 lifetime. defaultOrder() determines tab position; lower values
//                 appear first. visible() controls whether the tab is shown.
//   Lifecycle:    activate() / deactivate() are called when the user switches
//                 to/from this tab — use them for expensive refresh or pause
//                 operations. onSettingsChanged() notifies of preference
//                 updates. onConnectionChanged() notifies of daemon link state.
//   Signals:      requestNavigate(pluginId) asks MainWindow to switch tabs.
//                 updateDiagnostics(level, source, msg) posts to the log.
//
// Plugin instances are created by MainWindow, receive a ServiceContainer in
// their constructor for service access, and are registered with PluginRegistry.
// MainWindow queries the registry for tab ordering and visibility.

#include <QObject>
#include <QString>
#include <QWidget>
#include <QIcon>

struct AppSettings;

class WorkspacePlugin : public QObject {
  Q_OBJECT
public:
  virtual ~WorkspacePlugin() = default;

  // Identity
  virtual QString id() const = 0;
  virtual QString displayName() const = 0;
  virtual QString displayNameZh() const = 0;
  virtual QIcon icon() const { return QIcon(); }

  // UI
  virtual QWidget *widget() = 0;
  virtual int defaultOrder() const = 0;
  virtual bool visible() const = 0;

  // Lifecycle
  virtual void activate() {}
  virtual void deactivate() {}
  virtual void onSettingsChanged(const AppSettings &) {}
  virtual void onConnectionChanged(bool connected) {}

signals:
  void requestNavigate(const QString &pluginId);
  void updateDiagnostics(const QString &level, const QString &source, const QString &msg);
};
