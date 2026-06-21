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

class StartupSdoPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit StartupSdoPlugin(ServiceContainer *container,
                            QObject *parent = nullptr);

  // WorkspacePlugin identity
  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  // Lifecycle
  void activate() override;
  void deactivate() override;
  void onSettingsChanged(const AppSettings &settings) override;
  void onConnectionChanged(bool connected) override;

  // ── Table Accessors ──────────────────────────────────────────────
  QTableWidget *startupSdoTable() const;

  // ── Control Widgets ──────────────────────────────────────────────
  QCheckBox *startupWatchDiffsOnly() const;
  QLabel *startupWatchSummaryLabel() const;
  QLabel *startupSdoDetailLabel() const;

  // ── Table Management ─────────────────────────────────────────────
  void ensureStartupSdoTable();
  void updateStartupSdoControls(bool connected);
  void filterStartupSdoTable(bool diffsOnly);

signals:
  void startupSdoTableSelectionChanged();

private:
  void buildUi();
  void buildToolbar(QWidget *parent);
  void buildTable(QWidget *parent);

  ServiceContainer *container_;
  QWidget *containerWidget_ = nullptr;

  // Toolbar
  QCheckBox *startupWatchDiffsOnly_ = nullptr;
  QLabel *startupWatchSummaryLabel_ = nullptr;
  QLabel *startupSdoDetailLabel_ = nullptr;

  // Table
  QTableWidget *startupSdoTable_ = nullptr;
};
