#pragma once

/// @brief Workspace plugin for I/O variable management and PLC handoff.
///
/// @details The I/O Variables workspace merges data from PDO Map, Free Run,
/// Watch, Startup SDO, and user metadata into a single engineering table.
/// It serves as the bridge between raw EtherCAT bus data and structured
/// signal planning for PLC integration.
///
/// Features:
///   - **Unified signal table**: Combines PDO entries, watch values, startup
///     expectations, aliases, tags, and notes into one view.
///   - **PLC handoff quality**: Assesses duplicate symbols, missing metadata,
///     and generates PLC declaration blocks (Structured Text format).
///   - **Scope filtering**: Filter by signal scope (input/output/config).
///   - **Text filtering**: Filter by any column content.
///   - **Bulk operations**: Bulk rename, metadata editing, and export.
///   - **Color-coded severity**: Rows colored by PLC handoff quality
///     (ok, warning, error, info, changed).
///
/// @par Constructor
///   IoVariablePlugin(ServiceContainer *container, QObject *parent = nullptr)
///
/// @par Plugin Identity
///   - id: "iovariable"
///   - defaultOrder: 40
///   - visible: always true
///
/// @par Signals
///   - filterChanged(): Emitted when the text filter changes
///   - scopeFilterChanged(index): Emitted when the scope filter changes
///   - rowSelectionChanged(row): Emitted when the selected row changes
///
/// @see WorkspacePlugin, MainWindow, IoVariableModel

#include "plugins/WorkspacePlugin.h"

#include <QColor>
#include <QStringList>

class QComboBox;
class QLabel;
class QLineEdit;
class QTableWidget;
class ServiceContainer;

/// @brief Workspace plugin for I/O variable management and PLC handoff.
class IoVariablePlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  /// Constructs the I/O Variable plugin, building the unified signal table.
  /// @param container  Service container for accessing domain services
  /// @param parent     Qt parent object (typically MainWindow)
  explicit IoVariablePlugin(ServiceContainer *container,
                            QObject *parent = nullptr);

  // WorkspacePlugin identity
  QString id() const override;           ///< Returns "iovariable"
  QString displayName() const override;  ///< Returns "I/O Variables"
  QString displayNameZh() const override; ///< Returns "IO变量"
  QIcon icon() const override;           ///< Returns the I/O variable theme icon
  QWidget *widget() override;            ///< Returns the root container widget
  int defaultOrder() const override;     ///< Returns 40
  bool visible() const override;         ///< Returns true (always visible)

  // Lifecycle
  void activate() override;              ///< Called when user switches to this tab
  void deactivate() override;            ///< Called when user switches away
  void onSettingsChanged(const AppSettings &settings) override; ///< Reacts to settings changes
  void onConnectionChanged(bool connected) override; ///< Reacts to daemon connection state

  // UI update surface — MainWindow calls these with pre-computed data.
  /// Populates the table with headers and row data.
  /// @param headers Column header strings
  /// @param rows    List of row data (each row is a list of cell strings)
  void setRows(const QStringList &headers, const QList<QStringList> &rows);
  /// Sets the summary label text and optional severity color.
  /// @param text      Summary text (e.g. "42 signals, 3 errors")
  /// @param severity  Optional severity for coloring ("ok", "warning", "error", "info")
  void setSummary(const QString &text, const QString &severity = QString());
  /// Sets the tooltip for the summary label.
  /// @param tip  Tooltip text
  void setSummaryToolTip(const QString &tip);
  /// Sets the detail label text and optional severity color.
  /// @param text      Detail text for the selected row
  /// @param severity  Optional severity for coloring
  void setDetail(const QString &text, const QString &severity = QString());
  /// Sets the tooltip for the detail label.
  /// @param tip  Tooltip text
  void setDetailToolTip(const QString &tip);

  // Selection
  int currentRow() const;                ///< Returns the currently selected row, or -1
  /// Sets the active cell selection.
  /// @param row     Row index
  /// @param column  Column index
  void setCurrentCell(int row, int column);
  int rowCount() const;                  ///< Returns the number of rows in the table
  /// Returns whether a row is hidden by the current filter.
  /// @param row  Row index to check
  /// @return true if the row is hidden
  bool isRowHidden(int row) const;
  /// Sets the hidden state of a row.
  /// @param row     Row index
  /// @param hidden  true to hide the row
  void setRowHidden(int row, bool hidden);
  void resizeColumnsToContents();        ///< Auto-resizes columns to fit content

  // Table accessor for MainWindow integration.
  QTableWidget *ioVariableTable() const;       ///< Returns the main I/O variable table
  QLineEdit *ioVariableFilter() const;         ///< Returns the text filter input
  QComboBox *ioVariableScopeFilter() const;    ///< Returns the scope filter combo (input/output/config)
  QLabel *ioVariableSummaryLabel() const;      ///< Returns the summary statistics label
  QLabel *ioVariableDetailLabel() const;       ///< Returns the per-entry detail label

  // Color scheme for I/O variable rows.
  static constexpr const char *kOkColor = "#22c55e";         ///< Row color for OK quality
  static constexpr const char *kWarningColor = "#f59e0b";    ///< Row color for warning quality
  static constexpr const char *kErrorColor = "#ef4444";      ///< Row color for error quality
  static constexpr const char *kInfoColor = "#60a5fa";       ///< Row color for info severity
  static constexpr const char *kChangedColor = "#854d0e";    ///< Row color for changed signals (light)
  static constexpr const char *kChangedColorDark = "#fde68a"; ///< Row color for changed signals (dark)

signals:
  void filterChanged();                  ///< Emitted when the text filter changes
  void scopeFilterChanged(int index);    ///< Emitted when the scope filter selection changes
  void rowSelectionChanged(int row);     ///< Emitted when the selected row changes

private:
  void buildUi();  ///< Builds the table, filter, scope combo, and detail panel layout

  ServiceContainer *container_;
  QWidget *containerWidget_ = nullptr;
  QTableWidget *table_ = nullptr;
  QLineEdit *filter_ = nullptr;
  QComboBox *scopeFilter_ = nullptr;
  QLabel *summaryLabel_ = nullptr;
  QLabel *detailLabel_ = nullptr;
};
