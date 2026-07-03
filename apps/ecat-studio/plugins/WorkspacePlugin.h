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
  /// @brief Virtual destructor for proper cleanup of derived plugins.
  virtual ~WorkspacePlugin() = default;

  // -- Identity --

  /// @brief Unique plugin identifier string used for navigation and registry lookups.
  /// @return A short, unique string (e.g. "od", "watch", "topology").
  ///         Called by PluginRegistry to index the plugin and by MainWindow
  ///         to resolve requestNavigate() targets.
  virtual QString id() const = 0;

  /// @brief Human-readable tab label in English.
  /// @return Display string shown in the tab bar (e.g. "Object Dictionary").
  ///         Called by MainWindow when building the tab bar UI.
  virtual QString displayName() const = 0;

  /// @brief Human-readable tab label in Chinese (Simplified).
  /// @return Display string shown in the tab bar for Chinese locale
  ///         (e.g. "对象字典"). Called by MainWindow to support bilingual UI.
  virtual QString displayNameZh() const = 0;

  /// @brief Optional icon to display alongside the tab label.
  /// @return A QIcon for the tab bar, or a default-constructed QIcon if unused.
  ///         Called by MainWindow when rendering the tab bar.
  virtual QIcon icon() const { return QIcon(); }

  // -- UI --

  /// @brief Root widget for this workspace tab.
  /// @return A QWidget pointer owned by the plugin. Must remain valid for the
  ///         plugin's lifetime. Called by MainWindow to embed the tab content.
  virtual QWidget *widget() = 0;

  /// @brief Default tab ordering priority.
  /// @return An integer sort key; lower values appear first in the tab bar.
  ///         Called by PluginRegistry to establish the initial tab order.
  virtual int defaultOrder() const = 0;

  /// @brief Whether this workspace tab should be shown in the current context.
  /// @return true to display the tab, false to hide it.
  ///         Called by MainWindow each time the tab bar is rebuilt.
  virtual bool visible() const = 0;

  // -- Lifecycle --

  /// @brief Called when the user switches to this workspace tab.
  ///        Use this to trigger expensive refresh operations or start
  ///        periodic timers that should only run while the tab is active.
  virtual void activate() {}

  /// @brief Called when the user switches away from this workspace tab.
  ///        Use this to pause periodic timers or release resources that
  ///        are not needed while the tab is hidden.
  virtual void deactivate() {}

  /// @brief Called when application-wide settings are updated.
  /// @param settings The new AppSettings struct containing all preferences.
  ///        Plugins should re-read any cached settings and update their UI accordingly.
  virtual void onSettingsChanged(const AppSettings &) {}

  /// @brief Called when the daemon connection state changes.
  /// @param connected true if the daemon is now connected, false otherwise.
  ///        Plugins should disable/enable network-dependent UI elements.
  virtual void onConnectionChanged(bool connected) {}

signals:
  /// @brief Requests MainWindow to navigate to the specified plugin tab.
  /// @param pluginId The id() of the target plugin to switch to.
  void requestNavigate(const QString &pluginId);

  /// @brief Posts a diagnostic message to the application log/diagnostics panel.
  /// @param level Severity level string (e.g. "info", "warning", "error").
  /// @param source Source identifier for the message (e.g. plugin id).
  /// @param msg The diagnostic message text.
  void updateDiagnostics(const QString &level, const QString &source, const QString &msg);
};