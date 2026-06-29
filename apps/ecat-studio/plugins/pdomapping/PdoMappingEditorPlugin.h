#pragma once

// PdoMappingEditorPlugin — PDO Mapping editor workspace plugin.
// Provides a visual canvas, tree view, and property table for editing
// EtherCAT PDO mappings per slave. Supports add/remove of PDO entries,
// validation, and import/export of mapping configurations.

#include "plugins/WorkspacePlugin.h"

#include <QVector>

class QComboBox;
class QLabel;
class QPushButton;
class QSplitter;
class QTableWidget;
class QTabWidget;
class QTextEdit;
class QTreeWidget;
class QTreeWidgetItem;

class PdoMappingCanvas;
class PdoMappingService;
class PdoMappingValidator;

/// @brief Workspace plugin for PDO mapping editing and validation.
///
/// @details The PDO Mapping Editor workspace provides a visual and tabular
/// interface for configuring EtherCAT PDO mappings. It includes a canvas
/// for visual layout, a tree view for PDO structure, a property table for
/// entry details, and a validation panel for error reporting.
///
/// @par Plugin Identity
///   - id: "pdomapping"
///   - defaultOrder: 50
///   - visible: always true
///
/// @see WorkspacePlugin, PdoMappingService, PdoMappingCanvas

class PdoMappingEditorPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  /// Constructs the PDO Mapping Editor plugin.
  /// @param pdoService  PdoMappingService for PDO mapping data access
  /// @param parent      Qt parent object (typically MainWindow)
  explicit PdoMappingEditorPlugin(PdoMappingService *pdoService, QObject *parent = nullptr);

  QString id() const override;           ///< Returns "pdomapping"
  QString displayName() const override;  ///< Returns "PDO Mapping Editor"
  QString displayNameZh() const override; ///< Returns "PDO映射编辑器"
  QWidget *widget() override;            ///< Returns the root container widget
  int defaultOrder() const override;     ///< Returns 50
  bool visible() const override;         ///< Returns true (always visible)

  void activate() override;              ///< Called when user switches to this tab
  void deactivate() override;            ///< Called when user switches away

  /// Sets the active slave position for PDO mapping.
  /// @param position  Slave position on the bus
  void setSlavePosition(int position);
  int slavePosition() const;             ///< Returns the current slave position

  /// Adds a PDO entry to the specified SyncManager.
  /// @param smIndex   SyncManager index
  /// @param index     PDO object index (hex string)
  /// @param subIndex  PDO object subindex (hex string)
  /// @param name      Human-readable entry name
  /// @param dataType  Data type string (e.g. "UINT32")
  /// @param bitSize   Entry size in bits
  /// @param isOutput  true if this is an output PDO entry
  void addPdoEntry(int smIndex, const QString &index, const QString &subIndex,
                   const QString &name, const QString &dataType, int bitSize,
                   bool isOutput);
  /// Removes a PDO entry from the specified SyncManager.
  /// @param smIndex     SyncManager index
  /// @param entryIndex  Entry index within the SyncManager
  void removePdoEntry(int smIndex, int entryIndex);
  /// Returns the number of PDO entries in the specified SyncManager.
  /// @param smIndex  SyncManager index
  /// @return Entry count
  int pdoEntryCount(int smIndex) const;

  void validateMapping();                ///< Validates the current PDO mapping for errors
  bool hasErrors() const;                ///< Returns true if validation found errors
  int errorCount() const;                ///< Returns the number of validation errors

  /// Exports the current PDO mapping to a file.
  /// @param filePath  Destination file path
  /// @return true on successful export
  bool exportMapping(const QString &filePath);
  /// Imports a PDO mapping from a file.
  /// @param filePath  Source file path
  /// @return true on successful import
  bool importMapping(const QString &filePath);

  PdoMappingCanvas *canvas() const;      ///< Returns the visual PDO mapping canvas
  QTreeWidget *pdoTree() const;          ///< Returns the PDO structure tree widget
  QTableWidget *propertyTable() const;   ///< Returns the entry property table
  QTextEdit *validationPanel() const;    ///< Returns the validation results text panel

signals:
  void mappingChanged(int position);     ///< Emitted when a mapping is modified
  void validationCompleted(bool valid, int errorCount); ///< Emitted after validation completes

private:
  void buildUi();                          ///< Builds the splitter layout with canvas, tree, and panels
  void rebuildPdoTree();                   ///< Rebuilds the PDO tree from current mapping data
  void rebuildPropertyTable();             ///< Rebuilds the property table for the selected entry
  void updateValidationDisplay();          ///< Updates the validation panel with current results
  void populateSampleData();               ///< Populates sample data for development/testing

  PdoMappingService *pdoService_ = nullptr;
  QWidget *containerWidget_ = nullptr;

  QSplitter *mainSplitter_ = nullptr;
  QTreeWidget *pdoTree_ = nullptr;
  PdoMappingCanvas *canvas_ = nullptr;
  QTableWidget *propertyTable_ = nullptr;
  QTextEdit *validationPanel_ = nullptr;

  QComboBox *slaveCombo_ = nullptr;
  QPushButton *validateBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QPushButton *importBtn_ = nullptr;
  QPushButton *addEntryBtn_ = nullptr;
  QPushButton *removeEntryBtn_ = nullptr;
  QLabel *statusLabel_ = nullptr;

  int slavePosition_ = 0;
  bool hasErrors_ = false;
  int errorCount_ = 0;
};
