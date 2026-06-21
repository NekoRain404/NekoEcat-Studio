#pragma once

/// @brief Workspace plugin for session brief, next-best-action, and workspace badges.
///
/// @details The Session workspace provides a high-level summary of the current
/// engineering session, including:
///
///   - **Session brief table**: Shows the status of each engineering area
///     (topology, evidence, SDO, watch, startup, I/O variables, consistency)
///     with color-coded severity.
///   - **Next-best-action recommendation**: Suggests the most impactful next
///     step based on current evidence and missing data.
///   - **Workspace badges**: Tab badges showing pending issue counts per
///     workspace.
///   - **Copy functionality**: Copy session brief digests for documentation
///     or handoff.
///
/// @par Constructor
///   SessionPlugin(ServiceContainer *container, QObject *parent = nullptr)
///
/// @par Plugin Identity
///   - id: "session"
///   - defaultOrder: 80
///   - visible: always true
///
/// @par Signals
///   - sessionBriefRowActivated(row): Emitted when a session brief row is double-clicked
///   - sessionBriefRowCopyRequested(row): Emitted when the copy button is clicked
///
/// @see WorkspacePlugin, MainWindow, SessionBriefModel, NextBestActionModel

#include "plugins/WorkspacePlugin.h"

#include <QList>
#include <QStringList>

class QHeaderView;
class QLabel;
class QPushButton;
class QTableWidget;
class ServiceContainer;

struct SessionBriefRow;
struct SessionBriefUiRow;

class SessionPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit SessionPlugin(ServiceContainer *container,
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

  // Session brief table
  QTableWidget *sessionBriefTable() const;

  // Session brief copy button
  QPushButton *sessionBriefCopyButton() const;

  // Summary label
  QLabel *sessionBriefSummaryLabel() const;

  // Populate session brief table from MainWindow-gathered data.
  // Each row is a QStringList of cells: area, status, evidence, next.
  void updateSessionBrief(const QStringList &headers,
                          const QList<QStringList> &rows,
                          const QList<QColor> &rowColors);

  // Update copy button state based on current selection.
  void updateCopyButtonState();

  // Get the area text for the current row (for copy button label).
  QString currentRowArea() const;

signals:
  void sessionBriefRowActivated(int row);
  void sessionBriefRowCopyRequested(int row);

private:
  void buildUi();

  ServiceContainer *container_;
  QWidget *containerWidget_ = nullptr;
  QTableWidget *table_ = nullptr;
  QPushButton *copyButton_ = nullptr;
  QLabel *summaryLabel_ = nullptr;
};
