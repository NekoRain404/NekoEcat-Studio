#pragma once

// MainWindow — central coordinator for the NekoEcat Studio GUI.
//
// This class owns the tabbed workspace bar, the EcatClient (TCP link to ecatd),
// all domain services (EventBus, SdoService, WatchService, TopologyService),
// and the PluginRegistry that holds every workspace plugin.
//
// MainWindow is a partial class: method declarations are split across 16
// partial headers in workspaces/ (MainWindowSdo.h, MainWindowWatch.h, etc.),
// and the implementation is split across 31 .cpp files in workspaces/.
// All partials share MainWindowIncludes.h as their precompiled header.
//
// Member variable groups:
//   - EcatClient & daemon:  client_, daemon_, retry/refresh timers, slave list
//   - Services:             eventBus_, container_ (ServiceContainer)
//   - Plugin system:        pluginRegistry_ (populated in buildUi)
//   - UI widgets:           ~60 QLabel/QTableWidget/QTabWidget/QWidget pointers,
//                           one set per workspace, plus status-bar labels
//   - Workspace widgets:    aggregated structs (session_, stateMachine_,
//                           consistency_, diagnostics_, ioVar_, watch_, sdo_,
//                           workflow_, slaveEvidence_, freeRunWidgets_,
//                           bookmark_, sdoInspector_, rawText_, rtTest_)
//   - Workspace navigation: back/forward stacks, per-tab index cache
//   - Cached state:         last-parsed text blobs + loaded-position sentinels
//   - Watch & SDO data:     live values, evidence, pending reads/writes
//   - Topology & project:   baseline snapshot, project path, recent-files list


#include "EcatClient.h"
#include "SdoEvidenceModel.h"
#include "SettingsDialog.h"
#include "services/EventBus.h"
#include "plugins/PluginRegistry.h"
#include "workspaces/WorkspaceWidgets.h"

class QMenu;
class QShortcut;
class ServiceContainer;

#include <QHash>
#include <QMainWindow>
#include <QProcess>
#include <QSet>
#include <QStringList>
#include <QVector>

class QLabel;
class QJsonObject;
class QCheckBox;
class QComboBox;
class QFrame;
class QLineEdit;
class QPoint;
class QPlainTextEdit;
class QPushButton;
class QTableWidget;
class QTabWidget;
class QTimer;
class QTreeWidget;
class QTreeWidgetItem;
class QWidget;
class RtTestLatencyChart;
class RtTestJitterSpark;
struct RtTestWidgets;
class RealtimeChartDialog;
struct CommissioningWorkflowInput;
struct WorkflowStepDetailTexts;
struct CommissioningWorkflowTexts;
struct ConsistencyDetailTexts;
struct DiagnosticsEventTexts;
struct HostHealthTexts;
struct NextBestActionTexts;
struct ObjectBookmarkDetailTexts;
struct SelectedDriveSummaryTexts;
struct SlaveEvidenceSummaryTexts;
struct SlaveEvidenceUiTexts;
struct SdoHistoryRowDetailTexts;
struct SdoTargetTrailDetailTexts;
struct SessionBriefUiTexts;
struct StateMachineRowDetailTexts;
struct StartupSdoRowDetailTexts;
struct WatchStartupDeltaTexts;
enum class WorkspaceBoundaryKind;
struct WorkspaceBoundaryTexts;
struct WorkspaceTabBadgeTexts;

/// @brief Central coordinator for the NekoEcat Studio GUI application.
///
/// @details MainWindow is a partial-class implementation split across 31 .cpp
/// files in workspaces/, each covering a different workspace or concern.
/// All partials share MainWindowIncludes.h as their precompiled header.
///
/// Architecture overview:
///   MainWindow owns:
///     - EcatClient (TCP link to ecatd daemon on 127.0.0.1:5877)
///     - Core services (EventBus, ServiceContainer providing SdoService,
///       WatchService, TopologyService, and 80+ other services)
///     - PluginRegistry holding all workspace plugins (50+ plugins)
///     - All UI widgets, workspace navigation state, and cached data
///
/// Partial implementation files:
///   MainWindow.cpp          — Constructor, destructor, clearOnlineViews, daemon
///   MainWindowUiBuild.cpp   — buildUi(), rebuildUi(), main layout
///   MainWindowWiring.cpp    — wire() signal/slot connections
///   MainWindowWorkspaceNav.cpp — Tab navigation, back/forward stacks
///   MainWindowExport.cpp    — CSV/raw text data exports
///   MainWindowSdo*.cpp      — SDO workspace operations (5 files)
///   MainWindowWatch*.cpp    — Watch workspace operations
///   MainWindowFreeRun*.cpp  — Free Run workspace + chart
///   MainWindowTopologyUi.cpp — Topology tree, slave updates
///   MainWindowStateMachine.cpp — State machine view
///   MainWindowConsistency.cpp — Consistency checks
///   MainWindowIoVariable*.cpp — I/O variable workspace
///   MainWindowDiagnostics*.cpp — Diagnostics workspace
///   MainWindowCommissioning.cpp — Commissioning workflow
///   MainWindowTheme.cpp     — Theme application
///   MainWindowSettings.cpp  — Settings load/save
///   MainWindowContextMenus.cpp — Right-click menus
///   MainWindowCommandPalette.cpp — Fuzzy action search
///   MainWindowTexts.cpp     — Bilingual text structs
///   MainWindowManual.cpp    — User manual
///   MainWindowProjectIo.cpp — Project file I/O
class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  /// Constructs the main window: loads settings, creates services, builds UI,
  /// registers all plugins, wires signals, restores geometry, and starts ecatd.
  explicit MainWindow(QWidget *parent = nullptr);

  /// Saves settings, persists window geometry, and terminates ecatd daemon.
  ~MainWindow() override;

protected:
  /// Global event filter for keyboard shortcuts and context menu interception.
  bool eventFilter(QObject *watched, QEvent *event) override;

private:
  // ════════════════════════════════════════════════════════════════════
  // Partial method declarations — included from workspaces/ partial headers
  // ════════════════════════════════════════════════════════════════════

#include "workspaces/MainWindowUi.h"
#include "workspaces/MainWindowWiring.h"
#include "workspaces/MainWindowLocalization.h"
#include "workspaces/MainWindowProject.h"
#include "workspaces/MainWindowSdo.h"
#include "workspaces/MainWindowWatch.h"
#include "workspaces/MainWindowStartupSdo.h"
#include "workspaces/MainWindowFreeRun.h"
#include "workspaces/MainWindowTopology.h"
#include "workspaces/MainWindowStateMachine.h"
#include "workspaces/MainWindowConsistency.h"
#include "workspaces/MainWindowDiagnostics.h"
#include "workspaces/MainWindowSession.h"
#include "workspaces/MainWindowExport.h"
#include "workspaces/MainWindowBookmarks.h"
#include "workspaces/MainWindowPdoMap.h"
#include "workspaces/MainWindowIoVariable.h"

  // ════════════════════════════════════════════════════════════════════
  // Settings & Actions (kept here — core lifecycle)
  // ════════════════════════════════════════════════════════════════════

  void loadSettings();
  void saveSettings();
  void openSettings();

  // ════════════════════════════════════════════════════════════════════
  // Workspace Navigation (kept here — core window management)
  // ════════════════════════════════════════════════════════════════════

  bool activateWorkspaceTab(int index);
  bool activateWorkspacePage(QWidget *page);
  bool activateObjectDictionaryPaneFor(QWidget *widget);
  WorkspaceBoundaryKind workspaceBoundaryKindForPage(const QWidget *page) const;
  void recordWorkspaceHistory(int index);
  void goWorkspaceBack();
  void goWorkspaceForward();
  void updateWorkspaceNavigationActions();

  // ════════════════════════════════════════════════════════════════════
  // Member Variables — organized by functional group
  // ════════════════════════════════════════════════════════════════════

  // ── EcatClient & Daemon ───────────────────────────────────────
  EcatClient client_;
  QProcess daemon_;
  QTimer *refreshTimer_ = nullptr;
  QTimer *watchRefreshTimer_ = nullptr;
  QVector<SlaveInfo> slaves_;

  // ── UI Widgets ────────────────────────────────────────────────
  QLabel *connectionLabel_ = nullptr;
  QLabel *masterStateLabel_ = nullptr;
  QLabel *slaveCountLabel_ = nullptr;
  QLabel *linkStateLabel_ = nullptr;
  QLabel *lossLabel_ = nullptr;
  QLabel *freeRunLabel_ = nullptr;
  QLabel *selectedLabel_ = nullptr;
  QLabel *selectedSlaveNameLabel_ = nullptr;
  QLabel *selectedSlaveStateLabel_ = nullptr;
  QLabel *selectedSlaveFlagsLabel_ = nullptr;
  QLabel *selectedSlaveEvidenceLabel_ = nullptr;
  QLabel *selectedDriveSummaryLabel_ = nullptr;
  QLabel *selectedSlaveHintLabel_ = nullptr;
  QComboBox *masterCombo_ = nullptr;
  QTreeWidget *topologyTree_ = nullptr;
  QTableWidget *metricTable_ = nullptr;

  // ── Workspace Widget Aggregates ───────────────────────────────
  SessionWorkspaceWidgets *session_ = nullptr;
  StateMachineWorkspaceWidgets *stateMachine_ = nullptr;
  ConsistencyWorkspaceWidgets *consistency_ = nullptr;
  DiagnosticsWorkspaceWidgets *diagnostics_ = nullptr;
  IoVariableWorkspaceWidgets *ioVar_ = nullptr;
  WatchWorkspaceWidgets *watch_ = nullptr;
  SdoWorkspaceWidgets *sdo_ = nullptr;
  WorkflowWorkspaceWidgets *workflow_ = nullptr;
  SlaveEvidenceWorkspaceWidgets *slaveEvidence_ = nullptr;
  FreeRunWorkspaceWidgets *freeRunWidgets_ = nullptr;
  BookmarkWorkspaceWidgets *bookmark_ = nullptr;
  SdoInspectorWidgets *sdoInspector_ = nullptr;
  RawTextWidgets *rawText_ = nullptr;

  // ── Standalone Table/Label Widgets ────────────────────────────
  QTableWidget *identityTable_ = nullptr;
  QTableWidget *portTable_ = nullptr;
  QTableWidget *mailboxTable_ = nullptr;
  QTableWidget *sdoTargetTrailTable_ = nullptr;
  QLabel *sdoTargetTrailDetailLabel_ = nullptr;
  QLabel *objectBookmarkDetailLabel_ = nullptr;
  QTableWidget *sdoHistoryTable_ = nullptr;
  QLabel *sdoHistoryDetailLabel_ = nullptr;
  QTableWidget *hostHealthTable_ = nullptr;
  QTableWidget *esiTable_ = nullptr;
  QTableWidget *startupSdoTable_ = nullptr;

  // ── Tab Containers ───────────────────────────────────────────
  QTabWidget *tabs_ = nullptr;
  QTabWidget *sdoModeTabs_ = nullptr;

  // ── Workspace Page Widgets ────────────────────────────────────
  QWidget *overviewPage_ = nullptr;
  QWidget *objectDictionaryPage_ = nullptr;
  QWidget *pdoMapPage_ = nullptr;
  QWidget *watchPage_ = nullptr;
  QWidget *startupSdoPage_ = nullptr;
  QWidget *freeRunPage_ = nullptr;
  QWidget *ioVariablePage_ = nullptr;
  QWidget *consistencyPage_ = nullptr;
  QWidget *stateMachinePage_ = nullptr;
  QWidget *diagnosticsPage_ = nullptr;
  QWidget *esiRepositoryPage_ = nullptr;
  QWidget *notesPage_ = nullptr;
  QWidget *rtTestPage_ = nullptr;
  QWidget *esiXmlPage_ = nullptr;
  QWidget *masterRawPage_ = nullptr;
  QWidget *slaveRawPage_ = nullptr;
  QWidget *pdoRawPage_ = nullptr;
  QWidget *sdoRawPage_ = nullptr;

  // ── Workspace Navigation State ────────────────────────────────
  QVector<QWidget *> workspaceBackStack_;
  QVector<QWidget *> workspaceForwardStack_;
  bool suppressWorkspaceHistory_ = false;
  int overviewTabIndex_ = -1;
  int objectDictionaryTabIndex_ = -1;
  int pdoMapTabIndex_ = -1;
  int watchTabIndex_ = -1;
  int startupSdoTabIndex_ = -1;
  int freeRunTabIndex_ = -1;
  int ioVariableTabIndex_ = -1;
  int consistencyTabIndex_ = -1;
  int stateMachineTabIndex_ = -1;
  int diagnosticsTabIndex_ = -1;
  int esiRepositoryTabIndex_ = -1;
  int notesTabIndex_ = -1;
  int rtTestTabIndex_ = -1;
  int esiXmlTabIndex_ = -1;
  int masterRawTabIndex_ = -1;
  int slaveRawTabIndex_ = -1;
  int pdoRawTabIndex_ = -1;
  int sdoRawTabIndex_ = -1;

  // ── Status Bar ────────────────────────────────────────────────
  QLabel *statusSummaryLabel_ = nullptr;
  QLabel *workspaceBoundaryLabel_ = nullptr;
  QPushButton *nextBestActionButton_ = nullptr;

  // ── RT Test & Charts ──────────────────────────────────────────
  RtTestWidgets *rtTest_ = nullptr;
  bool rtTestRunning_ = false;
  QVector<RealtimeChartDialog *> openCharts_;
  QVector<QShortcut *> tabSwitchShortcuts_;

  // ── Cached State ──────────────────────────────────────────────
  QString lastMasterText_;
  QString lastSlaveInfoText_;
  QString lastPdoText_;
  QString lastSdoText_;
  QString lastXmlText_;
  int loadedSlaveInfoPosition_ = -1;
  int loadedPdoPosition_ = -1;
  int loadedSdoPosition_ = -1;
  int loadedXmlPosition_ = -1;
  QString lastFreeRunStatus_ = "Stopped";
  QHash<QString, QString> freeRunEntryNames_;
  QHash<QString, QString> freeRunObjectNames_;
  QHash<QString, QString> freeRunEntryValues_;

  // ── Watch & SDO Data ──────────────────────────────────────────
  QHash<QString, QString> watchValues_;
  QHash<QString, QStringList> sdoEvidence_;
  QHash<QString, QStringList> ioVariableMetadata_;
  QHash<QString, QString> pendingSdoReads_;
  QHash<QString, QString> pendingSdoReadTypes_;
  QHash<QString, QStringList> pendingSdoWrites_;
  QHash<QString, QStringList> pendingSdoVerifications_;
  QHash<QString, QVector<int>> pendingStartupSdoChecks_;
  QSet<QString> watchChangedKeys_;
  QSet<QString> rememberedSdoTargetTrailKeys_;

  // ── Topology & Project ────────────────────────────────────────
  QVector<SlaveInfo> topologyBaseline_;
  QString projectPath_;
  QString projectName_ = "Untitled";
  QStringList recentProjectPaths_;
  QMenu *recentProjectsMenu_ = nullptr;
  static constexpr int kMaxRecentProjects = 10;

  // ── Services ──────────────────────────────────────────────────
  EventBus *eventBus_ = nullptr;
  ServiceContainer *container_ = nullptr;
  PluginRegistry *pluginRegistry_ = nullptr;
  AppSettings settings_;

  // ── State Flags ───────────────────────────────────────────────
  bool freeRun_ = false;
  bool consistencyFresh_ = false;
  bool selectedSdoWritable_ = true;
};
