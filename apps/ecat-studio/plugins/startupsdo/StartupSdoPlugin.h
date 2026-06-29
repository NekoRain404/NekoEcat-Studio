#pragma once

/// @brief Workspace plugin for Startup SDO management and configuration.
///
/// @details The Startup SDO workspace manages a list of SDO write operations
/// that should be applied when the EtherCAT bus starts. It serves as the
/// "saved configuration" layer — values verified in Watch or OD can be
/// promoted to startup SDOs for automatic application on boot.
///
/// Features:
///   - **Startup SDO table**: Displays configured startup SDOs with
///     position, index, subindex, type, value, and verification status.
///   - **Watch diff filtering**: Show only rows where live watch values
///     differ from expected startup values.
///   - **Pre-flight checks**: Validate all startup SDOs before application
///     (address validity, type compatibility, value range).
///   - **Batch apply**: Apply all or selected startup SDOs with confirmation.
///   - **Verification**: Read back applied values and verify they match.
///   - **Watch comparison**: Compare live values against startup expectations.
///
/// @par Constructor
///   StartupSdoPlugin(ServiceContainer *container, QObject *parent = nullptr)
///
/// @par Plugin Identity
///   - id: "startupsdo"
///   - defaultOrder: 35
///   - visible: always true
///
/// @par Signals
///   - startupSdoTableSelectionChanged(): Emitted when table selection changes
///
/// @see WorkspacePlugin, MainWindow, SdoService

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QTableWidget;
class QPushButton;
class QCheckBox;
class ServiceContainer;

/// @brief Workspace plugin for Startup SDO management and configuration.
class StartupSdoPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  /// Constructs the Startup SDO plugin, building the toolbar and table UI.
  /// @param container  Service container for accessing domain services
  /// @param parent     Qt parent object (typically MainWindow)
  explicit StartupSdoPlugin(ServiceContainer *container,
                            QObject *parent = nullptr);

  // WorkspacePlugin identity
  QString id() const override;           ///< Returns "startupsdo"
  QString displayName() const override;  ///< Returns "Startup SDO"
  QString displayNameZh() const override; ///< Returns "启动SDO"
  QIcon icon() const override;           ///< Returns the Startup SDO theme icon
  QWidget *widget() override;            ///< Returns the root container widget
  int defaultOrder() const override;     ///< Returns 35
  bool visible() const override;         ///< Returns true (always visible)

  // Lifecycle
  void activate() override;              ///< Called when user switches to this tab
  void deactivate() override;            ///< Called when user switches away
  void onSettingsChanged(const AppSettings &settings) override; ///< Reacts to settings changes
  void onConnectionChanged(bool connected) override; ///< Reacts to daemon connection state

  // ── Table Accessors ──────────────────────────────────────────────
  QTableWidget *startupSdoTable() const; ///< Returns the startup SDO table widget

  // ── Control Widgets ──────────────────────────────────────────────
  QCheckBox *startupWatchDiffsOnly() const;  ///< Returns the watch-diffs-only filter checkbox
  QLabel *startupWatchSummaryLabel() const;  ///< Returns the watch comparison summary label
  QLabel *startupSdoDetailLabel() const;     ///< Returns the per-entry detail label

  // ── Table Management ─────────────────────────────────────────────
  void ensureStartupSdoTable();        ///< Creates the table if not yet built
  /// Enables or disables controls based on daemon connection state.
  /// @param connected  true if the daemon is connected
  void updateStartupSdoControls(bool connected);
  /// Filters the table to show only rows where live values differ from startup.
  /// @param diffsOnly  true to show only differing rows
  void filterStartupSdoTable(bool diffsOnly);

signals:
  void startupSdoTableSelectionChanged(); ///< Emitted when the table selection changes

private:
  void buildUi();                      ///< Builds the toolbar and table layout
  void buildToolbar(QWidget *parent);  ///< Builds the toolbar with filter and summary controls
  void buildTable(QWidget *parent);    ///< Builds the startup SDO table

  ServiceContainer *container_;
  QWidget *containerWidget_ = nullptr;

  // Toolbar
  QCheckBox *startupWatchDiffsOnly_ = nullptr;
  QLabel *startupWatchSummaryLabel_ = nullptr;
  QLabel *startupSdoDetailLabel_ = nullptr;

  // Table
  QTableWidget *startupSdoTable_ = nullptr;
};
