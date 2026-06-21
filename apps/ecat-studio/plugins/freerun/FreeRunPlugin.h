#pragma once

/// @brief Workspace plugin for Free Run real-time process data monitoring.
///
/// @details The Free Run workspace provides real-time monitoring of EtherCAT
/// process data (PDO inputs/outputs) during Free Run mode. It displays a
/// table of process data entries cross-referenced against the PDO map for
/// naming and context.
///
/// Features:
///   - **Real-time process data entry table**: Shows current input/output
///     process values with PDO map cross-reference for naming.
///   - **Text filtering**: Filter entries by name or index with text search.
///   - **Changed-only filter**: Highlight entries that have drifted from
///     their baseline values.
///   - **Summary and detail panels**: Aggregate statistics and per-entry
///     information display.
///   - **Real-time chart dialogs**: Open multiple chart windows for
///     continuous visualization of selected entries.
///   - **Entry caching**: Fast lookup of entry names and values via
///     QHash caches.
///   - **Free Run state tracking**: Tracks enabled/disabled state and
///     last telemetry status.
///
/// @par Constructor
///   FreeRunPlugin(ServiceContainer *container, QObject *parent = nullptr)
///
/// @par Plugin Identity
///   - id: "freerun"
///   - defaultOrder: 35
///   - visible: always true
///
/// @par Signals
///   - chartOpened(dialog): Emitted when a new chart dialog is opened
///   - chartClosed(dialog): Emitted when a chart dialog is closed
///   - pollingIntervalChanged(ms): Emitted when the polling interval changes
///
/// @see WorkspacePlugin, MainWindow, FreeRunController

#include "plugins/WorkspacePlugin.h"

#include <QHash>
#include <QStringList>

class QCheckBox;
class QLabel;
class QLineEdit;
class QTableWidget;
class RealtimeChartDialog;
class ServiceContainer;

class FreeRunPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit FreeRunPlugin(ServiceContainer *container,
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

  // Table access
  QTableWidget *entryTable() const;
  QLineEdit *filter() const;
  QCheckBox *changedOnly() const;
  QLabel *summaryLabel() const;
  QLabel *detailLabel() const;

  // UI update surface — MainWindow calls these with pre-computed data.
  void setEntryRows(const QStringList &headers, const QList<QStringList> &rows);
  void setSummary(const QString &text);
  void setDetail(const QString &text);

  // Entry lookup
  QString entryName(int row) const;
  QString entryValue(int row) const;
  void setEntryNames(const QHash<QString, QString> &names);
  void setEntryValues(const QHash<QString, QString> &values);

  // Chart management
  void addOpenChart(RealtimeChartDialog *dialog);
  void removeOpenChart(RealtimeChartDialog *dialog);
  QVector<RealtimeChartDialog *> openCharts() const;

  // Free Run state
  bool freeRunEnabled() const;
  void setFreeRunEnabled(bool enabled);
  QString lastStatus() const;
  void setLastStatus(const QString &status);

signals:
  void chartOpened(RealtimeChartDialog *dialog);
  void chartClosed(RealtimeChartDialog *dialog);
  void pollingIntervalChanged(int ms);

private:
  void buildUi();

  ServiceContainer *container_;
  QWidget *containerWidget_ = nullptr;
  QTableWidget *entryTable_ = nullptr;
  QLineEdit *filter_ = nullptr;
  QCheckBox *changedOnly_ = nullptr;
  QLabel *summaryLabel_ = nullptr;
  QLabel *detailLabel_ = nullptr;

  QHash<QString, QString> entryNames_;
  QHash<QString, QString> entryValues_;
  QVector<RealtimeChartDialog *> openCharts_;
  bool freeRunEnabled_ = false;
  QString lastStatus_ = "Stopped";
};
