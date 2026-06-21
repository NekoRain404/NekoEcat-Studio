#pragma once

/// @brief Workspace plugin for ESI (EtherCAT Slave Information) repository management.
///
/// @details The ESI Repository workspace manages ESI XML files that describe
/// EtherCAT slave devices. It provides a file browser for importing ESI files,
/// a device list for browsing registered devices, and a detail panel for
/// viewing device descriptions, PDO mappings, and vendor information.
///
/// Features:
///   - **ESI file import**: Import ESI XML files into the local repository.
///   - **Device browser**: List all registered ESI devices with filtering.
///   - **Device detail panel**: View vendor, product, revision, PDO mappings,
///     and supported mailbox protocols.
///   - **Export**: Export selected ESI data for documentation or sharing.
///   - **Refresh**: Reload the ESI repository from disk.
///
/// @par Constructor
///   EsiPlugin(EsiService *service, QObject *parent = nullptr)
///   Uses fine-grained injection pattern.
///
/// @par Plugin Identity
///   - id: "esi"
///   - defaultOrder: 90
///   - visible: always true
///
/// @par Usage Example
///   @code
///   // In MainWindow constructor:
///   auto *esiService = new EsiService(this);
///   pluginRegistry_->registerPlugin(new EsiPlugin(esiService, this));
///
///   // Import ESI files:
///   esiPlugin->importFile();
///
///   // Refresh the repository:
///   esiPlugin->refreshList();
///   @endcode
///
/// @see WorkspacePlugin, EsiService, PluginRegistry

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QSplitter;
class QTableWidget;
class EsiService;
class ServiceContainer;

/// @brief Workspace plugin for ESI (EtherCAT Slave Information) repository management.
///
/// @details This plugin provides a complete ESI file management interface with:
///   - File import dialog for ESI XML files
///   - Device list with text-based filtering
///   - Detail table showing device properties, PDO mappings, and mailbox protocols
///   - Export functionality for documentation and sharing
///   - Repository refresh from disk
///
/// The plugin communicates with EsiService for data operations and uses
/// EventBus for connection state notifications.
class EsiPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  /// @brief Constructs the ESI plugin with fine-grained service injection.
  /// @param service  EsiService instance for ESI data operations
  /// @param parent   Qt parent object (typically MainWindow)
  explicit EsiPlugin(EsiService *service, QObject *parent = nullptr);

  // ── WorkspacePlugin Identity ──────────────────────────────────
  QString id() const override;           ///< Returns "esi"
  QString displayName() const override;  ///< Returns "ESI Repository"
  QString displayNameZh() const override; ///< Returns "ESI 仓库"
  QIcon icon() const override;           ///< Returns "application-xml" theme icon
  QWidget *widget() override;            ///< Returns the root container widget
  int defaultOrder() const override;     ///< Returns 90
  bool visible() const override;         ///< Returns true (always visible)

  // ── Lifecycle Hooks ───────────────────────────────────────────
  void activate() override;              ///< Called when user switches to this tab
  void deactivate() override;            ///< Called when user switches away

  // ── Actions ───────────────────────────────────────────────────
  /// @brief Opens a file dialog to import ESI XML files.
  /// @details Supports multiple file selection. Imported files are added to
  /// the local repository and the device list is refreshed automatically.
  void importFile();

  /// @brief Refreshes the device list from the ESI repository.
  /// @details Reloads all registered ESI devices and updates the list widget.
  void refreshList();

  /// @brief Exports selected device data to a file.
  /// @details Exports device description, PDO mappings, and vendor info
  /// to a user-selected file location.
  void exportSelected();

  // ── Accessors ─────────────────────────────────────────────────
  EsiService *service() const { return service_; } ///< Returns the ESI service instance
  QListWidget *deviceList() const { return deviceList_; } ///< Returns the device list widget
  QTableWidget *detailTable() const { return detailTable_; } ///< Returns the detail table widget

private:
  /// @brief Builds the UI layout with splitter, device list, and detail table.
  void buildUi();

  /// @brief Updates the device list from the ESI repository.
  /// @details Clears the current list and repopulates with all registered devices.
  void updateDeviceList();

  /// @brief Shows device detail for the selected device.
  /// @param index  Index of the selected device in the list
  /// @details Populates the detail table with device properties, PDO mappings,
  /// and mailbox protocol information.
  void showDeviceDetail(int index);

  EsiService *service_;                    ///< ESI service for data operations
  QWidget *containerWidget_ = nullptr;     ///< Root container widget
  QListWidget *deviceList_ = nullptr;      ///< Device list widget with filtering
  QTableWidget *detailTable_ = nullptr;    ///< Device detail table
  QLabel *summaryLabel_ = nullptr;         ///< Summary label showing device count
  QPushButton *importBtn_ = nullptr;       ///< Import button for ESI files
  QPushButton *exportBtn_ = nullptr;       ///< Export button for device data
  QPushButton *refreshBtn_ = nullptr;      ///< Refresh button for repository
  QLineEdit *filterEdit_ = nullptr;        ///< Filter input for device list
};
