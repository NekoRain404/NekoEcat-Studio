#pragma once

/// @brief Workspace plugin for the Overview summary page.
///
/// @details The Overview workspace serves as the commissioning cockpit,
/// providing a high-level view of the EtherCAT bus state, selected slave
/// context, and commissioning progress. It is organized into 4 sub-tabs:
///
///   - **Details** (index 0): Metric cards, slave identity, port status,
///     and mailbox protocol tables.
///   - **Brief** (index 1): Session brief table summarizing current
///     engineering state and pending actions.
///   - **Workflow** (index 2): Commissioning workflow table with step-by-step
///     guidance and next-best-action recommendations.
///   - **Matrix** (index 3): Slave evidence matrix showing completeness
///     of evidence collection per slave.
///
/// MainWindow delegates all table data population through this plugin's
/// public accessor methods. The plugin owns the UI widgets but does not
/// manage the data — that responsibility lies with MainWindow's workspace
/// partials (MainWindowCommissioning.cpp, MainWindowConsistency.cpp, etc.).
///
/// @par Constructor
///   OverviewPlugin(ServiceContainer *container, QObject *parent = nullptr)
///
/// @par Plugin Identity
///   - id: "overview"
///   - defaultOrder: 5 (leftmost tab)
///   - visible: always true
///
/// @see WorkspacePlugin, PluginRegistry, MainWindow

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QTabWidget;
class QTableWidget;
class ServiceContainer;

class OverviewPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  /// Constructs the Overview plugin, building the 4-tab UI layout.
  /// @param container  Service container for accessing domain services
  /// @param parent     Qt parent object (typically MainWindow)
  explicit OverviewPlugin(ServiceContainer *container,
                          QObject *parent = nullptr);

  // ── WorkspacePlugin Identity ──────────────────────────────────
  QString id() const override;           ///< Returns "overview"
  QString displayName() const override;  ///< Returns "Overview"
  QString displayNameZh() const override; ///< Returns "总览"
  QIcon icon() const override;           ///< Returns "view-list-details" theme icon
  QWidget *widget() override;            ///< Returns the root container widget
  int defaultOrder() const override;     ///< Returns 5 (leftmost tab position)
  bool visible() const override;         ///< Returns true (always visible)

  // ── Lifecycle Hooks ───────────────────────────────────────────
  void activate() override;              ///< Called when user switches to this tab
  void deactivate() override;            ///< Called when user switches away
  void onSettingsChanged(const AppSettings &settings) override; ///< Reacts to settings changes
  void onConnectionChanged(bool connected) override; ///< Reacts to daemon connection state

  // ── Sub-tab Accessors ─────────────────────────────────────────
  /// Returns the QTabWidget containing the 4 sub-tabs.
  QTabWidget *overviewTabs() const;
  int detailsTabIndex() const;  ///< Returns 0 (Details sub-tab index)
  int briefTabIndex() const;    ///< Returns 1 (Brief sub-tab index)
  int workflowTabIndex() const; ///< Returns 2 (Workflow sub-tab index)
  int matrixTabIndex() const;   ///< Returns 3 (Matrix sub-tab index)

  // ── Table Accessors ───────────────────────────────────────────
  /// Metric cards table (master state, slave count, link state, etc.)
  QTableWidget *metricTable() const;
  /// Slave identity table (vendor ID, product code, revision number)
  QTableWidget *identityTable() const;
  /// Slave port status table (link state, Rx/Tx error counters per port)
  QTableWidget *portTable() const;
  /// Mailbox protocol support table (CoE, EoE, FoE, SoE, VoE)
  QTableWidget *mailboxTable() const;
  /// Session brief table (engineering state summary, pending actions)
  QTableWidget *sessionBriefTable() const;
  /// Commissioning workflow table (step-by-step guidance)
  QTableWidget *workflowTable() const;
  /// Slave evidence matrix (evidence completeness per slave)
  QTableWidget *slaveEvidenceMatrixTable() const;

private:
  /// Builds the 4-tab UI layout with metric cards and tables.
  void buildUi();

  ServiceContainer *container_;      ///< Service container for domain access
  QWidget *containerWidget_ = nullptr; ///< Root container widget
  QTabWidget *tabs_ = nullptr;       ///< Main tab widget with 4 sub-tabs

  QTableWidget *metricTable_ = nullptr;              ///< Metric cards table
  QTableWidget *identityTable_ = nullptr;            ///< Slave identity table
  QTableWidget *portTable_ = nullptr;                ///< Port status table
  QTableWidget *mailboxTable_ = nullptr;             ///< Mailbox protocol table
  QTableWidget *sessionBriefTable_ = nullptr;        ///< Session brief table
  QTableWidget *workflowTable_ = nullptr;             ///< Commissioning workflow table
  QTableWidget *slaveEvidenceMatrixTable_ = nullptr;  ///< Evidence matrix table
};
