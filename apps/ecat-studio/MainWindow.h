#pragma once

// Main application window: workspace tabs, toolbars, wiring, and all workspace methods.


#include "EcatClient.h"
#include "SdoEvidenceModel.h"
#include "SettingsDialog.h"
#include "services/EventBus.h"
#include "services/SdoService.h"
#include "services/WatchService.h"
#include "services/TopologyService.h"
#include "workspaces/WorkspaceWidgets.h"

class QMenu;
class QShortcut;

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

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;

private:
  
  // ── UI Construction ───────────────────────────────────────────
  void buildUi();
  void rebuildUi();
  void applyTheme();
  void applySettings();
  void updateActionAvailability();
  
  // ── Settings & Actions ────────────────────────────────────────
  void loadSettings();
  void saveSettings();
  void openSettings();
  
  // ── Command Palette ───────────────────────────────────────────
  void showCommandPalette();
  
  // ── RT Stability Test ──────────────────────────────────────────
  QWidget *buildRtTestPage();
  void updateRtTestTelemetry(const QJsonObject &telemetry);
  void appendRtTestTimeline(const QJsonArray &recent, double avgUsec);
  void updateRtTestActionAvailability();
  QString formatDuration(double seconds) const;

  // ── Help ──────────────────────────────────────────────────────
  void showManual();
  void showAbout();
  
  // ── Workspace Navigation ──────────────────────────────────────
  bool activateWorkspaceTab(int index);
  bool activateWorkspacePage(QWidget *page);
  bool activateObjectDictionaryPaneFor(QWidget *widget);
  WorkspaceBoundaryKind workspaceBoundaryKindForPage(const QWidget *page) const;
  void recordWorkspaceHistory(int index);
  void goWorkspaceBack();
  void goWorkspaceForward();
  void updateWorkspaceNavigationActions();
  // Build the main UI layout — tabs, toolbars, panels, metric cards
  
  // Tear down and rebuild the entire UI when language changes
  // ── Context Menus ─────────────────────────────────────────────
  void showTopologyContextMenu(const QPoint &position);
  void showTableContextMenu(QTableWidget *table, const QPoint &position);
  void showSdoTargetPanelContextMenu(const QPoint &position);
  bool runLocalEvidenceAction(QTableWidget *table);
  // Central guard: enable/disable all actions based on current selection and connection state
  void copyTableToClipboard(QTableWidget *table, bool selectedOnly);
  
  // ── Localization & Master Selection ────────────────────────────
  QString uiText(const QString &english, const QString &zh) const;
  QString activeMasterName() const;
  void refreshMasterSelector();
  // Restore preferences from QSettings
  void setActiveMaster(const QString &target);
  // Persist preferences to QSettings
  bool confirmDangerousOperation(const QString &title, const QString &summary,
  // Open the settings dialog and apply changes
                                 const QStringList &details,
                                 const QString &confirmText);
  
  // ── State Machine ─────────────────────────────────────────────
  QStringList stateTransitionImpactDetails(int position,
                                           const QString &requestedState) const;
  // Command palette: fuzzy-search all available actions
  QString recommendedEthercatState(const SlaveInfo &slave) const;
  void updateStateMachineView();
  void updateStateMachineRowDetail();
  void updateSessionBrief();
  void openSessionBriefRow(int row);
  
  // ── Slave Operations ──────────────────────────────────────────
  void requestSlaveStateWithConfirmation(int position, const QString &state);
  void requestAllSlaveState(const QString &state);
  void runHostDiagnostics();
  // Map workspace page widget to its logical boundary kind for boundary-aware features
  void copySelectedHostCommand();
  
  // ── Evidence & Snapshots ──────────────────────────────────────
  void prepareSelectedSlaveSnapshot();
  void beginSelectedSlaveOnlineLoad(int position);
  
  // ── SDO Impact Preview ────────────────────────────────────────
  QStringList sdoWriteImpactDetails(int position, const QString &index,
                                    const QString &subIndex,
                                    const QString &targetValue,
                                    const QString &type) const;
  // Show context menus for topology tree, tables, and SDO target panel
  QString startupSdoImpactLine(int row) const;
  QStringList startupSdoBatchImpactDetails(const QVector<int> &rows,
  // Trigger a local evidence action (e.g. read SDO) on the focused table
                                           int previewLimit) const;
  
  // ── Online Data Lifecycle ─────────────────────────────────────
  void clearOnlineViews();
  
  // Bilingual selector: returns Chinese or English string based on active language
  // ── Project Management ────────────────────────────────────────
  void newProject();
  void openProject();
  void saveProject();
  // Full-featured confirmation dialog for potentially dangerous operations (state change, Free Run, etc.)
  void saveProjectAs();
  bool writeProjectFile(const QString &path);
  bool readProjectFile(const QString &path);
  
  // ── ESI Repository ────────────────────────────────────────────
  void importEsiFiles();
  void refreshEsiRepository();
  // Heuristic recommendation for the next EtherCAT state based on collected evidence
  
  // ── Watch Workspace ───────────────────────────────────────────
  void ensureWatchTable();
  QString decodeWatchValue(const QString &index, const QString &subIndex,
                           const QString &type, const QString &value,
                           const QString &mode) const;
  void addCurrentSdoToWatch(bool requestRead = true);
  void addSelectedDictionaryRowsToWatch();
  void addVisibleDictionaryRowsToWatch();
  // Request a state change for a single slave, with confirmation dialog
  void addDictionaryRowsToWatch(const QVector<int> &rows,
  // Broadcast a state request to all detected slaves, with confirmation
                                const QString &sourceLabel);
  // Run host-level health checks (kernel modules, NIC, IgH)
  void addSelectedPdoEntriesToWatch();
  // Copy the fix command for the selected host health issue
  void addSelectedHistoryRowsToWatch();
  void readSelectedDictionaryRows();
  void readVisibleDictionaryRows();
  void readFailedDictionaryRows();
  void readDictionaryRows(const QVector<int> &rows, const QString &sourceLabel,
  // Collect a full read-only evidence snapshot for the selected slave
                          bool confirmLargeBatch);
  // Reset all cached slave data and begin fresh online loading
  void addCia402WatchPreset();
  // Clear all cached online state — called on disconnect or master switch
  
  // Connect all Qt signals/slots between UI widgets, client, and timers
  // ── Startup SDO Workspace ─────────────────────────────────────
  void addStartupSdoFromWatchRow(int row);
  void addStartupSdoFromSelectedWatchRows();
  void syncWatchRowsToStartupSdo(const QVector<int> &rows);
  // Build a human-readable list of Free Run impact details for the confirmation dialog
  void syncSelectedWatchRowsToStartupSdo();
  // Start or stop Free Run with a safety confirmation dialog
  void addSelectedHistoryRowsToStartupSdo();
  void addSelectedDictionaryEvidenceToStartupSdo();
  void addDictionaryEvidenceRowsToStartupSdo(const QVector<int> &rows);
  void refreshWatchList(bool quiet = false);
  // Highlight a slave in the topology tree by position
  void captureWatchBaseline();
  // Process a fresh slave list — detect topology changes, update tree, refresh views
  void clearWatchBaseline();
  // Update overview metrics from the daemon's master status text
  void updateWatchBaselineDelta(int row);
  // Parse identity text and populate identity/port/mailbox tables
  void updateWatchBaselineDeltas();
  // Parse PDO map text and populate the PDO table
  void updateWatchStartupDelta(int row);
  // Parse Object Dictionary text and populate the SDO table with evidence merging
  void updateWatchStartupDeltas();
  void updateWatchAutoRefresh();
  void clearWatchList();
  // Process Free Run telemetry JSON — update signals table and entry detail
  void ensureStartupSdoTable();
  WorkflowStepDetailTexts
  workflowStepDetailTexts() const;
  // Append a timestamped line to the diagnostics log panel
  CommissioningWorkflowTexts commissioningWorkflowTexts() const;
  // Return the position of the currently selected slave, or -1
  ConsistencyDetailTexts consistencyDetailTexts() const;
  // Return selected row indices from the Object Dictionary table
  DiagnosticsEventTexts diagnosticsEventTexts() const;
  // Return selected row indices from the SDO history table
  HostHealthTexts hostHealthTexts() const;
  // Return selected row indices from the Startup SDO table
  NextBestActionTexts nextBestActionTexts() const;
  // Check whether a watch table row has a non-empty value cell
  ObjectBookmarkDetailTexts objectBookmarkDetailTexts() const;
  // Check whether any selected watch rows contain values
  SdoHistoryRowDetailTexts sdoHistoryRowDetailTexts() const;
  SdoTargetTrailDetailTexts sdoTargetTrailDetailTexts() const;
  StateMachineRowDetailTexts stateMachineRowDetailTexts() const;
  StartupSdoRowDetailTexts startupSdoRowDetailTexts() const;
  // Initialize the embedded ecatd daemon process
  WatchStartupDeltaTexts watchStartupDeltaTexts() const;
  // Initiate an SDO read for the selected row or target panel entry
  SessionBriefUiTexts sessionBriefUiTexts() const;
  SelectedDriveSummaryTexts selectedDriveSummaryTexts() const;
  // Initiate an SDO write with optional read-back verification
  SlaveEvidenceSummaryTexts slaveEvidenceSummaryTexts() const;
  // Process a completed SDO read result — update evidence, history, and tables
  SlaveEvidenceUiTexts slaveEvidenceUiTexts() const;
  WorkspaceBoundaryTexts workspaceBoundaryTexts() const;
  WorkspaceTabBadgeTexts workspaceTabBadgeTexts() const;
  
  // Compare live watch values against the captured baseline to detect drift
  // ── Startup SDO Operations ────────────────────────────────────
  void updateStartupSdoWatchEvidence();
  void filterStartupSdoTable();
  // Capture the current watch values as the drift baseline
  void updateStartupSdoRowDetail();
  void focusStartupSdoWatchDiffs();
  void addStartupSdo();
  void removeStartupSdo();
  void moveStartupSdoRow(int delta);
  void applyStartupSdoList();
  // Compare watch values against startup SDO expectations
  void applyStartupSdoRow(int row);
  void applySelectedStartupSdoRows();
  void applyStartupSdoRows(const QVector<int> &rows,
                           const QString &operationLabel,
                           const QString &summary, const QString &confirmText);
  // Build cross-reference entries for the Free Run entry table, matching runtime data against PDO map
  QVector<int> startupSdoRowsWithWatchDiffs() const;
  void applyStartupSdoWatchDiffRows();
  void verifyStartupSdoList();
  void verifyStartupSdoRow(int row);
  void verifySelectedStartupSdoRows();
  void addStartupSdoRowToWatch(int row);
  // Get the recommended next commissioning workflow step index
  bool preflightStartupSdoList(bool showSuccess);
  // Run a specific commissioning workflow step
  void updateStartupSdoControls();
  // Review the first blocking issue in the commissioning workflow
  
  // ── Topology Baseline ─────────────────────────────────────────
  void captureTopologyBaseline();
  void clearTopologyBaseline();
  
  // ── Signal Wiring & Daemon ────────────────────────────────────
  void wire();
  // Check whether consistency gate has blocking issues
  void startEmbeddedDaemon();
  // Count errors/warnings/total items in the consistency check
  void requestRefresh();
  // Update the next-best-action recommendation based on current evidence
  
  // Update tab badges with pending issue counts
  // ── Free Run Workspace ────────────────────────────────────────
  QStringList freeRunImpactDetails() const;
  // Update the diagnostics event table from the log
  void updateFreeRunEntryDetail();
  void setFreeRun(bool enabled);
  
  // ── Diagnostics Export ────────────────────────────────────────
  void exportDiagnosticsReport();

  // ── Data Exports (MainWindowExport.cpp) ─────────────────────────
  void exportPdoMapCsv();
  void exportSdoDictionaryCsv();
  void exportSdoHistoryCsv();
  void exportEsiRepositoryCsv();
  void exportEsiXml();
  void exportWatchCsv();
  void exportStartupSdoCsv();
  void exportTopologyCsv();
  void exportHostHealthCsv();
  void exportPdoRawText();
  void exportSdoRawText();
  void exportMasterRawText();
  void exportSlaveRawText();

  // ── Free Run Real-time Chart ─────────────────────────────────
  void openFreeRunChart();
  void addSelectedOdToFreeRunChart();
  
  // Load a project file (.ecat.json) and restore all tables
  // ── SDO Inspector & Target Panel ──────────────────────────────
  void restoreManualSdoWriteMode();
  // Auto-save the current project if a path is set
  bool isCurrentSdoTarget(int position, const QString &index,
  // Open a project via file dialog
                          const QString &subIndex) const;
  // Save the current project, prompting for a path if needed
  QString sdoObjectCategory(const QString &index, const QString &name,
  // Save the current project to a new file path
                            const QString &object, const QString &detail) const;
  void updateSdoTargetPanel(const QString &source, const QString &detail,
                            const QString &status, const QStringList &problems);
  void updateSdoTargetRowActionButton();
  void updateSdoTargetRowCopyButton();
  // Read/write operations for selected/visible/failed Object Dictionary rows
  void updateSdoInspector(const QString &source = QString(),
                          const QString &detail = QString());
  void ensureSdoTargetTrailTable();
  void updateSdoTargetTrailRowDetail();
  // Capture the current topology as the baseline for change detection
  void rememberCurrentSdoTarget(const QString &source, const QString &detail);
  // Clear the captured topology baseline
  bool prepareSdoTargetTrailRow(int row, bool reportRestoreSuccess);
  QString sdoTargetTrailRowStartupValue(int row) const;
  // Add CiA 402 drive-specific watch entries for the selected slave
  bool sdoTargetTrailRowCanCreateStartup(int row) const;
  void restoreSdoTargetTrailRow(int row);
  void addSdoTargetTrailRowToWatch();
  void bookmarkSdoTargetTrailRow();
  void addSdoTargetTrailRowToStartup();
  // Track which session operations are in-flight to avoid duplicate requests
  void removeSelectedSdoTargetTrailRows();
  // Async completion callback for session operations
  void clearSdoTargetTrail();
  int currentSdoDictionaryRow() const;
  int currentSdoWatchRow() const;
  int currentSdoStartupRow() const;
  int currentSdoBookmarkRow() const;
  int currentSdoTargetTrailRow() const;
  QString currentSdoPreferredEvidenceValue(QString *source = nullptr) const;
  // Analyze the full selected-slave evidence set and produce a diagnostic summary
  SdoEvidenceCandidates currentSdoEvidenceCandidates() const;
  bool currentSdoEvidenceHasConflict() const;
  bool currentSdoWriteDeltaReviewAvailable() const;
  void reviewCurrentSdoWriteDelta();
  void copyCurrentSdoEvidenceDigest();
  bool copySdoTargetPanelRowDigest(int row);
  bool openSdoTargetPanelRow(int row);
  void openCurrentSdoWatchLink();
  void openCurrentSdoStartupLink();
  void openCurrentSdoBookmarkLink();
  void openCurrentSdoTargetTrailLink();
  
  // ── SDO Evidence Updates ──────────────────────────────────────
  void updateSdoTableEvidence(int position, const QString &index,
                              const QString &subIndex, const QString &value,
                              const QString &status, const QString &detail);
  void useReadSdoValueForWrite();
  void usePreferredSdoEvidenceForWrite();
  void pickSdoEvidenceForWrite();
  void writeCurrentSdo();
  void prepareCia402Controlword(const QString &label, const QString &value);
  bool recommendedCia402Controlword(QString *label, QString *value,
                                    QString *reason) const;
  bool validateSdoAddressAndValue(const QString &index, const QString &subIndex,
                                  const QString &value, const QString &type,
                                  QStringList *errors,
                                  QStringList *warnings) const;
  
  // ── SDO History ───────────────────────────────────────────────
  void ensureSdoHistoryTable();
  void appendSdoHistory(const QString &action, int position,
                        const QString &index, const QString &subIndex,
                        const QString &type, const QString &value,
                        const QString &status, const QString &detail);
  void updateSdoHistoryRowDetail();
  
  // ── SDO Selection from Other Workspaces ───────────────────────
  void requestSdoRead(int position, const QString &index,
                      const QString &subIndex, const QString &source,
                      const QString &type = QString());
  void applySdoSelectionFromDictionary(int row, bool readAfterFill);
  void applySdoSelectionFromPdoMap(int row, bool readAfterFill);
  void applySdoSelectionFromFreeRunEntry(int row, bool readAfterFill);
  void applySdoSelectionFromIoVariable(int row, bool readAfterFill);
  void applySdoSelectionFromWatch(int row, bool readAfterFill);
  void applySdoSelectionFromHistory(int row, bool readAfterFill);
  void applySdoSelectionFromStartup(int row, bool readAfterFill);
  
  // ── Object Bookmarks ──────────────────────────────────────────
  void ensureObjectBookmarkTable();
  void updateObjectBookmarkRowDetail();
  QVector<int> selectedObjectBookmarkRows() const;
  bool selectSlaveForLocalEvidence(int position);
  bool selectObjectBookmarkSlave(int position);
  void addCurrentSdoBookmark();
  void addSelectedDictionaryRowsToBookmarks();
  void addDictionaryRowsToBookmarks(const QVector<int> &rows,
                                    const QString &sourceLabel);
  void addObjectBookmark(int position, const QString &index,
                         const QString &subIndex, const QString &access,
                         const QString &type, const QString &bits,
                         const QString &name, const QString &lastValue,
                         const QString &source);
  void applySdoSelectionFromBookmark(int row, bool readAfterFill);
  
  // ── Bookmark Actions ──────────────────────────────────────────
  void addSelectedObjectBookmarksToWatch();
  void addObjectBookmarkRowsToWatch(const QVector<int> &rows);
  void addSelectedObjectBookmarksToStartupSdo();
  void addObjectBookmarkRowsToStartupSdo(const QVector<int> &rows);
  void removeSelectedObjectBookmarks();
  
  // ── PDO Map Workspace ─────────────────────────────────────────
  void filterPdoTable();
  void updatePdoRowDetail();
  
  // ── SDO Table Filtering ───────────────────────────────────────
  void setSdoFilterPreset(const QString &query);
  bool hasFailedSdoEvidence() const;
  int firstFailedSdoEvidenceRow() const;
  void focusFailedSdoEvidence();
  void filterSdoTable(const QString &text);
  
  // ── Free Run Entry Table ──────────────────────────────────────
  void filterFreeRunEntryTable();
  
  // ── Watch Table ───────────────────────────────────────────────
  void filterWatchTable();
  void updateWatchRowDetail();
  
  // ── I/O Variables Workspace ───────────────────────────────────
  void updateIoVariableTable();
  void filterIoVariableTable();
  void updateIoVariableRowDetail();
  QString ioVariableRowKey(int row) const;
  QVector<int> selectedIoVariableRows(bool visibleOnly) const;
  QVector<int> visibleIoVariableRows() const;
  QString ioVariableHandoffIssueLabel(const QString &key) const;
  QStringList ioVariableHandoffIssueLabels(const QStringList &keys) const;
  QString ioVariablePlcQuality(int row, const QSet<QString> *duplicateSymbols,
                               QString *symbol = nullptr) const;
  QSet<QString> duplicateIoVariablePlcSymbols() const;
  QVector<int> plcHandoffIssueRows(const QVector<int> &rows) const;
  QStringList plcHandoffIssueDetails(const QVector<int> &rows, int previewLimit,
                                     int totalRows = -1) const;
  QString plcDeclarationBlock(const QVector<int> &rows) const;
  void editSelectedIoVariableMetadata();
  void bulkNameIoVariables();
  void reviewPlcHandoffIssues();
  void focusPlcHandoffIssueRows(const QVector<int> &issueRows,
                                bool showReadyMessage);
  void copyIoVariablePlcDeclarations(bool selectedOnly);
  void clearSelectedIoVariableMetadata();
  void exportIoVariablesCsv();
  void exportIoVariablesPlcCsv();
  void exportIoVariablesPlcDeclarationsSt();
  bool confirmPlcHandoffOperation(const QVector<int> &rows,
                                  const QString &operation,
                                  const QString &continueText);
  
  // ── Consistency Workspace ─────────────────────────────────────
  void updateConsistencyView();
  void filterConsistencyTable();
  void updateConsistencyRowDetail();
  void openConsistencyView();
  void focusEvidenceFromConsistency(int row = -1);
  void focusIoVariablesFromConsistency(int row = -1);
  int firstConsistencyBlockingIssueRow() const;
  int firstConsistencyIoIssueRow() const;
  bool consistencyCheckAvailable() const;
  void consistencyIssueCounts(int *errors, int *warnings, int *infos,
                              int *ready) const;
  QStringList consistencyGateDetails(const QString &operation) const;
  
  // ── I/O Variable Actions ──────────────────────────────────────
  void addSelectedIoVariablesToWatch();
  void addVisibleIoVariablesToWatch();
  void addIoVariableRowsToWatch(const QVector<int> &rows,
                                const QString &sourceLabel);
  void addSelectedIoVariablesToStartupSdo();
  void addVisibleIoVariablesToStartupSdo();
  void addIoVariableRowsToStartupSdo(const QVector<int> &rows,
                                     const QString &sourceLabel);
  
  // ── Diagnostics Workspace ─────────────────────────────────────
  void filterDiagnosticsTable();
  void updateSelectedSlavePanel();
  void updateSlaveEvidenceSummary();
  void updateSelectedDriveSummary();
  void updateDriveNextButton();
  
  // ── Session Brief ─────────────────────────────────────────────
  void updateSessionBriefCopyButton();
  bool copySessionBriefRowDigest(int row);
  void updateWorkflowStepCopyButton();
  void updateWorkflowStepDetail();
  bool copyWorkflowStepDigest(int row);
  
  // ── Commissioning Workflow ────────────────────────────────────
  void filterCommissioningWorkflow();
  void reviewFirstCommissioningWorkflowIssue();
  void reviewNextCommissioningWorkflowIssue();
  void updateSlaveEvidenceMatrix();
  void filterSlaveEvidenceMatrix();
  void openSlaveEvidenceMatrixRow(int row);
  void reviewFirstSlaveEvidenceMatrixIssue();
  void reviewNextSlaveEvidenceMatrixIssue();
  bool copySlaveEvidenceMatrixRowDigest(int row);
  int slaveEvidenceMatrixRowForPosition(int position) const;
  CommissioningWorkflowInput commissioningWorkflowInput() const;
  void updateSlaveEvidenceMatrixTriageButtons();
  
  // ── Workspace Boundary & Badges ───────────────────────────────
  void updateWorkspaceBoundary();
  void updateNextBestAction();
  void updateTabBadges();
  void runNextBestAction();
  
  // ── Topology Summary ──────────────────────────────────────────
  void updateTopologyBaselineSummary();
  QStringList topologyBaselineIssues() const;
  
  // ── Commissioning State ───────────────────────────────────────
  void updateCommissioningWorkflow();
  int nextCommissioningWorkflowStep() const;
  void runNextCommissioningWorkflowStep();
  void runCommissioningWorkflowStep(int row);
  
  // ── Host Health & Diagnostics ─────────────────────────────────
  void updateHostHealth(const QJsonArray &checks);
  void updateDiagnostics(const QString &level, const QString &source,
                         const QString &message);
  void updateDiagnosticsSummary();
  void styleDiagnosticsRow(int row, const QString &level);
  
  // ── Status Bar & Logging ──────────────────────────────────────
  void updateStatusBar();
  void log(const QString &message);
  int selectedPosition() const;
  QVector<int> selectedDictionaryRows() const;
  QVector<int> selectedSdoHistoryRows() const;
  QVector<int> selectedStartupSdoRows() const;
  bool watchRowHasValue(int row) const;
  bool selectedWatchRowsHaveValue() const;
  
  // ── Slave Selection ───────────────────────────────────────────
  void setSelectedSlave(int position);
  
  // ── Topology & Data Updates ───────────────────────────────────
  void reportTopologyChanges(const QVector<SlaveInfo> &previous,
                             const QVector<SlaveInfo> &current);
  void updateSlaves(const QVector<SlaveInfo> &slaves);
  void updateMasterSummary(const QString &text);
  void updateSlaveInfo(const QString &text);
  void updatePdoTable(const QString &text);
  void updateSdoTable(const QString &text);
  
  // ── Free Run Telemetry ────────────────────────────────────────
  void updateFreeRunTelemetry(const QJsonObject &telemetry);
  void updateFreeRunEntryTable(const QList<QStringList> &rows);
  void setTableRows(QTableWidget *table, const QStringList &headers,
                    const QList<QStringList> &rows);
  void setMetricCard(QLabel *label, const QString &title, const QString &value);

  EcatClient client_;
  QProcess daemon_;
  QTimer *connectRetryTimer_ = nullptr;
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
  QTabWidget *tabs_ = nullptr;
  QTabWidget *sdoModeTabs_ = nullptr;
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
  
  // ── Raw Text Panels ───────────────────────────────────────────
  QLabel *statusSummaryLabel_ = nullptr;
  QLabel *workspaceBoundaryLabel_ = nullptr;
  QPushButton *nextBestActionButton_ = nullptr;
  
  // ── SDO Panel Widgets ─────────────────────────────────────────
  
  // ── PDO Panel Widgets ─────────────────────────────────────────
  
  // ── SDO Filter Widgets ────────────────────────────────────────
  
  // ── Watch Panel Widgets ───────────────────────────────────────
  
  // ── RT Test Panel Widgets (aggregated in workspace file) ─────
  RtTestWidgets *rtTest_ = nullptr;
  bool rtTestRunning_ = false;
  QVector<RealtimeChartDialog *> openCharts_;
  QVector<QShortcut *> tabSwitchShortcuts_;  /* Ctrl+1~9, Ctrl+Tab, Ctrl+Shift+Tab */
  void applyCustomShortcuts();

  // ── Free Run Panel Widgets ────────────────────────────────────
  
  // ── I/O Variable Panel Widgets ────────────────────────────────
  
  // ── Consistency Panel Widgets ─────────────────────────────────
  
  // ── Diagnostics Panel Widgets ─────────────────────────────────
  
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
  void updateRecentProjectsMenu();
  void addToRecentProjects(const QString &path);
  EventBus *eventBus_ = nullptr;
  SdoService *sdoService_ = nullptr;
  WatchService *watchService_ = nullptr;
  TopologyService *topologyService_ = nullptr;
  AppSettings settings_;
  bool freeRun_ = false;
  bool consistencyFresh_ = false;
  bool selectedSdoWritable_ = true;
};
