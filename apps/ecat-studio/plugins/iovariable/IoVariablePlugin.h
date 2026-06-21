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

class IoVariablePlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit IoVariablePlugin(ServiceContainer *container,
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

  // UI update surface — MainWindow calls these with pre-computed data.
  void setRows(const QStringList &headers, const QList<QStringList> &rows);
  void setSummary(const QString &text, const QString &severity = QString());
  void setSummaryToolTip(const QString &tip);
  void setDetail(const QString &text, const QString &severity = QString());
  void setDetailToolTip(const QString &tip);

  // Selection
  int currentRow() const;
  void setCurrentCell(int row, int column);
  int rowCount() const;
  bool isRowHidden(int row) const;
  void setRowHidden(int row, bool hidden);
  void resizeColumnsToContents();

  // Table accessor for MainWindow integration.
  QTableWidget *ioVariableTable() const;
  QLineEdit *ioVariableFilter() const;
  QComboBox *ioVariableScopeFilter() const;
  QLabel *ioVariableSummaryLabel() const;
  QLabel *ioVariableDetailLabel() const;

  // Color scheme for I/O variable rows.
  static constexpr const char *kOkColor = "#22c55e";
  static constexpr const char *kWarningColor = "#f59e0b";
  static constexpr const char *kErrorColor = "#ef4444";
  static constexpr const char *kInfoColor = "#60a5fa";
  static constexpr const char *kChangedColor = "#854d0e";
  static constexpr const char *kChangedColorDark = "#fde68a";

signals:
  void filterChanged();
  void scopeFilterChanged(int index);
  void rowSelectionChanged(int row);

private:
  void buildUi();

  ServiceContainer *container_;
  QWidget *containerWidget_ = nullptr;
  QTableWidget *table_ = nullptr;
  QLineEdit *filter_ = nullptr;
  QComboBox *scopeFilter_ = nullptr;
  QLabel *summaryLabel_ = nullptr;
  QLabel *detailLabel_ = nullptr;
};
