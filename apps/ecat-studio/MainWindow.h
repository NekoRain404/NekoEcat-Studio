#pragma once

#include "EcatClient.h"
#include "SdoEvidenceModel.h"
#include "SettingsDialog.h"

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
struct CommissioningWorkflowInput;
struct CommissioningWorkflowStepDetailTexts;
struct CommissioningWorkflowTexts;
struct ConsistencyDetailTexts;
struct DiagnosticsEventTexts;
struct HostHealthTexts;
struct NextBestActionTexts;
struct ObjectBookmarkDetailTexts;
struct SelectedDriveSummaryTexts;
struct SelectedSlaveEvidenceSummaryTexts;
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
  void buildUi();
  void rebuildUi();
  void applyTheme();
  void applySettings();
  void updateActionAvailability();
  void loadSettings();
  void saveSettings();
  void openSettings();
  void showCommandPalette();
  void showManual();
  void showAbout();
  bool activateWorkspaceTab(int index);
  bool activateWorkspacePage(QWidget *page);
  bool activateObjectDictionaryPaneFor(QWidget *widget);
  WorkspaceBoundaryKind workspaceBoundaryKindForPage(const QWidget *page) const;
  void recordWorkspaceHistory(int index);
  void goWorkspaceBack();
  void goWorkspaceForward();
  void updateWorkspaceNavigationActions();
  void showTopologyContextMenu(const QPoint &position);
  void showTableContextMenu(QTableWidget *table, const QPoint &position);
  void showSdoTargetPanelContextMenu(const QPoint &position);
  bool runLocalEvidenceAction(QTableWidget *table);
  void copyTableToClipboard(QTableWidget *table, bool selectedOnly);
  QString uiText(const QString &english, const QString &zh) const;
  QString activeMasterName() const;
  void refreshMasterSelector();
  void setActiveMaster(const QString &target);
  bool confirmDangerousOperation(const QString &title, const QString &summary,
                                 const QStringList &details,
                                 const QString &confirmText);
  QStringList stateTransitionImpactDetails(int position,
                                           const QString &requestedState) const;
  QString recommendedEthercatState(const SlaveInfo &slave) const;
  void updateStateMachineView();
  void updateStateMachineRowDetail();
  void updateSessionBrief();
  void openSessionBriefRow(int row);
  void requestSlaveStateWithConfirmation(int position, const QString &state);
  void requestAllSlaveState(const QString &state);
  void runHostDiagnostics();
  void copySelectedHostCommand();
  void prepareSelectedSlaveSnapshot();
  void beginSelectedSlaveOnlineLoad(int position);
  QStringList sdoWriteImpactDetails(int position, const QString &index,
                                    const QString &subIndex,
                                    const QString &targetValue,
                                    const QString &type) const;
  QString startupSdoImpactLine(int row) const;
  QStringList startupSdoBatchImpactDetails(const QVector<int> &rows,
                                           int previewLimit) const;
  void clearOnlineViews();
  void newProject();
  void openProject();
  void saveProject();
  void saveProjectAs();
  bool writeProjectFile(const QString &path);
  bool readProjectFile(const QString &path);
  void importEsiFiles();
  void refreshEsiRepository();
  void ensureWatchTable();
  QString decodeWatchValue(const QString &index, const QString &subIndex,
                           const QString &type, const QString &value,
                           const QString &mode) const;
  void addCurrentSdoToWatch(bool requestRead = true);
  void addSelectedDictionaryRowsToWatch();
  void addVisibleDictionaryRowsToWatch();
  void addDictionaryRowsToWatch(const QVector<int> &rows,
                                const QString &sourceLabel);
  void addSelectedPdoEntriesToWatch();
  void addSelectedHistoryRowsToWatch();
  void readSelectedDictionaryRows();
  void readVisibleDictionaryRows();
  void readFailedDictionaryRows();
  void readDictionaryRows(const QVector<int> &rows, const QString &sourceLabel,
                          bool confirmLargeBatch);
  void addCia402WatchPreset();
  void addStartupSdoFromWatchRow(int row);
  void addStartupSdoFromSelectedWatchRows();
  void syncWatchRowsToStartupSdo(const QVector<int> &rows);
  void syncSelectedWatchRowsToStartupSdo();
  void addSelectedHistoryRowsToStartupSdo();
  void addSelectedDictionaryEvidenceToStartupSdo();
  void addDictionaryEvidenceRowsToStartupSdo(const QVector<int> &rows);
  void refreshWatchList(bool quiet = false);
  void captureWatchBaseline();
  void clearWatchBaseline();
  void updateWatchBaselineDelta(int row);
  void updateWatchBaselineDeltas();
  void updateWatchStartupDelta(int row);
  void updateWatchStartupDeltas();
  void updateWatchAutoRefresh();
  void clearWatchList();
  void ensureStartupSdoTable();
  CommissioningWorkflowStepDetailTexts
  commissioningWorkflowStepDetailTexts() const;
  CommissioningWorkflowTexts commissioningWorkflowTexts() const;
  ConsistencyDetailTexts consistencyDetailTexts() const;
  DiagnosticsEventTexts diagnosticsEventTexts() const;
  HostHealthTexts hostHealthTexts() const;
  NextBestActionTexts nextBestActionTexts() const;
  ObjectBookmarkDetailTexts objectBookmarkDetailTexts() const;
  SdoHistoryRowDetailTexts sdoHistoryRowDetailTexts() const;
  SdoTargetTrailDetailTexts sdoTargetTrailDetailTexts() const;
  StateMachineRowDetailTexts stateMachineRowDetailTexts() const;
  StartupSdoRowDetailTexts startupSdoRowDetailTexts() const;
  WatchStartupDeltaTexts watchStartupDeltaTexts() const;
  SessionBriefUiTexts sessionBriefUiTexts() const;
  SelectedDriveSummaryTexts selectedDriveSummaryTexts() const;
  SelectedSlaveEvidenceSummaryTexts selectedSlaveEvidenceSummaryTexts() const;
  SlaveEvidenceUiTexts slaveEvidenceUiTexts() const;
  WorkspaceBoundaryTexts workspaceBoundaryTexts() const;
  WorkspaceTabBadgeTexts workspaceTabBadgeTexts() const;
  void updateStartupSdoWatchEvidence();
  void filterStartupSdoTable();
  void updateStartupSdoRowDetail();
  void focusStartupSdoWatchDiffs();
  void addStartupSdo();
  void removeStartupSdo();
  void moveStartupSdoRow(int delta);
  void applyStartupSdoList();
  void applyStartupSdoRow(int row);
  void applySelectedStartupSdoRows();
  void applyStartupSdoRows(const QVector<int> &rows,
                           const QString &operationLabel,
                           const QString &summary, const QString &confirmText);
  QVector<int> startupSdoRowsWithWatchDiffs() const;
  void applyStartupSdoWatchDiffRows();
  void verifyStartupSdoList();
  void verifyStartupSdoRow(int row);
  void verifySelectedStartupSdoRows();
  void addStartupSdoRowToWatch(int row);
  bool preflightStartupSdoList(bool showSuccess);
  void updateStartupSdoControls();
  void captureTopologyBaseline();
  void clearTopologyBaseline();
  void wire();
  void startEmbeddedDaemon();
  void requestRefresh();
  QStringList freeRunImpactDetails() const;
  void updateFreeRunEntryDetail();
  void setFreeRun(bool enabled);
  void exportDiagnosticsReport();
  void restoreManualSdoWriteMode();
  bool isCurrentSdoTarget(int position, const QString &index,
                          const QString &subIndex) const;
  QString sdoObjectCategory(const QString &index, const QString &name,
                            const QString &object, const QString &detail) const;
  void updateSdoTargetPanel(const QString &source, const QString &detail,
                            const QString &status, const QStringList &problems);
  void updateSdoTargetRowActionButton();
  void updateSdoTargetRowCopyButton();
  void updateSdoInspector(const QString &source = QString(),
                          const QString &detail = QString());
  void ensureSdoTargetTrailTable();
  void updateSdoTargetTrailRowDetail();
  void rememberCurrentSdoTarget(const QString &source, const QString &detail);
  bool prepareSdoTargetTrailRow(int row, bool reportRestoreSuccess);
  QString sdoTargetTrailRowStartupValue(int row) const;
  bool sdoTargetTrailRowCanCreateStartup(int row) const;
  void restoreSdoTargetTrailRow(int row);
  void addSdoTargetTrailRowToWatch();
  void bookmarkSdoTargetTrailRow();
  void addSdoTargetTrailRowToStartup();
  void removeSelectedSdoTargetTrailRows();
  void clearSdoTargetTrail();
  int currentSdoDictionaryRow() const;
  int currentSdoWatchRow() const;
  int currentSdoStartupRow() const;
  int currentSdoBookmarkRow() const;
  int currentSdoTargetTrailRow() const;
  QString currentSdoPreferredEvidenceValue(QString *source = nullptr) const;
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
  void ensureSdoHistoryTable();
  void appendSdoHistory(const QString &action, int position,
                        const QString &index, const QString &subIndex,
                        const QString &type, const QString &value,
                        const QString &status, const QString &detail);
  void updateSdoHistoryRowDetail();
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
  void addSelectedObjectBookmarksToWatch();
  void addObjectBookmarkRowsToWatch(const QVector<int> &rows);
  void addSelectedObjectBookmarksToStartupSdo();
  void addObjectBookmarkRowsToStartupSdo(const QVector<int> &rows);
  void removeSelectedObjectBookmarks();
  void filterPdoTable();
  void updatePdoRowDetail();
  void setSdoFilterPreset(const QString &query);
  bool hasFailedSdoEvidence() const;
  int firstFailedSdoEvidenceRow() const;
  void focusFailedSdoEvidence();
  void filterSdoTable(const QString &text);
  void filterFreeRunEntryTable();
  void filterWatchTable();
  void updateWatchRowDetail();
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
  void addSelectedIoVariablesToWatch();
  void addVisibleIoVariablesToWatch();
  void addIoVariableRowsToWatch(const QVector<int> &rows,
                                const QString &sourceLabel);
  void addSelectedIoVariablesToStartupSdo();
  void addVisibleIoVariablesToStartupSdo();
  void addIoVariableRowsToStartupSdo(const QVector<int> &rows,
                                     const QString &sourceLabel);
  void filterDiagnosticsTable();
  void updateSelectedSlavePanel();
  void updateSelectedSlaveEvidenceSummary();
  void updateSelectedDriveSummary();
  void updateDriveNextButton();
  void updateSessionBriefCopyButton();
  bool copySessionBriefRowDigest(int row);
  void updateWorkflowStepCopyButton();
  void updateWorkflowStepDetail();
  bool copyWorkflowStepDigest(int row);
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
  void updateWorkspaceBoundary();
  void updateNextBestAction();
  void updateTabBadges();
  void runNextBestAction();
  void updateTopologyBaselineSummary();
  QStringList topologyBaselineIssues() const;
  void updateCommissioningWorkflow();
  int nextCommissioningWorkflowStep() const;
  void runNextCommissioningWorkflowStep();
  void runCommissioningWorkflowStep(int row);
  void updateHostHealth(const QJsonArray &checks);
  void updateDiagnostics(const QString &level, const QString &source,
                         const QString &message);
  void updateDiagnosticsSummary();
  void styleDiagnosticsRow(int row, const QString &level);
  void updateStatusBar();
  void log(const QString &message);
  int selectedPosition() const;
  QVector<int> selectedDictionaryRows() const;
  QVector<int> selectedSdoHistoryRows() const;
  QVector<int> selectedStartupSdoRows() const;
  bool watchRowHasValue(int row) const;
  bool selectedWatchRowsHaveValue() const;
  void setSelectedSlave(int position);
  void reportTopologyChanges(const QVector<SlaveInfo> &previous,
                             const QVector<SlaveInfo> &current);
  void updateSlaves(const QVector<SlaveInfo> &slaves);
  void updateMasterSummary(const QString &text);
  void updateSlaveInfo(const QString &text);
  void updatePdoTable(const QString &text);
  void updateSdoTable(const QString &text);
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
  QTableWidget *sessionBriefTable_ = nullptr;
  QTableWidget *workflowTable_ = nullptr;
  QTableWidget *slaveEvidenceMatrixTable_ = nullptr;
  QTableWidget *stateMachineTable_ = nullptr;
  QTableWidget *identityTable_ = nullptr;
  QTableWidget *portTable_ = nullptr;
  QTableWidget *mailboxTable_ = nullptr;
  QTableWidget *pdoTable_ = nullptr;
  QTableWidget *sdoTable_ = nullptr;
  QTableWidget *sdoTargetTrailTable_ = nullptr;
  QLabel *sdoTargetTrailDetailLabel_ = nullptr;
  QTableWidget *objectBookmarkTable_ = nullptr;
  QLabel *objectBookmarkDetailLabel_ = nullptr;
  QTableWidget *sdoHistoryTable_ = nullptr;
  QLabel *sdoHistoryDetailLabel_ = nullptr;
  QTableWidget *freeRunTable_ = nullptr;
  QTableWidget *freeRunEntryTable_ = nullptr;
  QTableWidget *ioVariableTable_ = nullptr;
  QTableWidget *consistencyTable_ = nullptr;
  QTableWidget *hostHealthTable_ = nullptr;
  QTableWidget *diagnosticsTable_ = nullptr;
  QTableWidget *watchTable_ = nullptr;
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
  QWidget *esiXmlPage_ = nullptr;
  QWidget *masterRawPage_ = nullptr;
  QWidget *slaveRawPage_ = nullptr;
  QWidget *pdoRawPage_ = nullptr;
  QWidget *sdoRawPage_ = nullptr;
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
  int esiXmlTabIndex_ = -1;
  int masterRawTabIndex_ = -1;
  int slaveRawTabIndex_ = -1;
  int pdoRawTabIndex_ = -1;
  int sdoRawTabIndex_ = -1;
  QPlainTextEdit *masterText_ = nullptr;
  QPlainTextEdit *infoText_ = nullptr;
  QPlainTextEdit *pdoText_ = nullptr;
  QPlainTextEdit *sdoText_ = nullptr;
  QPlainTextEdit *xmlText_ = nullptr;
  QPlainTextEdit *logText_ = nullptr;
  QPlainTextEdit *projectNotes_ = nullptr;
  QLabel *statusSummaryLabel_ = nullptr;
  QLabel *workspaceBoundaryLabel_ = nullptr;
  QPushButton *nextBestActionButton_ = nullptr;
  QLabel *sdoInspectorLabel_ = nullptr;
  QTableWidget *sdoTargetTable_ = nullptr;
  QLineEdit *sdoIndex_ = nullptr;
  QLineEdit *sdoSubIndex_ = nullptr;
  QLineEdit *sdoValue_ = nullptr;
  QLineEdit *sdoWriteValue_ = nullptr;
  QPushButton *useSdoValueButton_ = nullptr;
  QComboBox *sdoType_ = nullptr;
  QLineEdit *pdoFilter_ = nullptr;
  QLabel *sdoSummaryLabel_ = nullptr;
  QLabel *pdoSummaryLabel_ = nullptr;
  QLabel *pdoDetailLabel_ = nullptr;
  QLabel *workflowSummaryLabel_ = nullptr;
  QLabel *workflowStepDetailLabel_ = nullptr;
  QComboBox *workflowScopeFilter_ = nullptr;
  QLineEdit *workflowFilter_ = nullptr;
  QPushButton *workflowReviewButton_ = nullptr;
  QPushButton *workflowReviewNextButton_ = nullptr;
  QPushButton *workflowStepCopyButton_ = nullptr;
  QPushButton *sessionBriefCopyButton_ = nullptr;
  QLabel *slaveEvidenceMatrixSummaryLabel_ = nullptr;
  QPushButton *slaveEvidenceMatrixReviewButton_ = nullptr;
  QPushButton *slaveEvidenceMatrixReviewNextButton_ = nullptr;
  QPushButton *slaveEvidenceMatrixCopyButton_ = nullptr;
  QVector<QPushButton *> slaveEvidenceMatrixTriageButtons_;
  QLineEdit *slaveEvidenceMatrixFilter_ = nullptr;
  QComboBox *slaveEvidenceMatrixScopeFilter_ = nullptr;
  QLabel *stateMachineSummaryLabel_ = nullptr;
  QLabel *stateMachineDetailLabel_ = nullptr;
  QLineEdit *sdoFilter_ = nullptr;
  QLineEdit *watchFilter_ = nullptr;
  QCheckBox *watchAutoRefresh_ = nullptr;
  QCheckBox *watchChangedOnly_ = nullptr;
  QComboBox *watchScopeFilter_ = nullptr;
  QComboBox *watchRefreshInterval_ = nullptr;
  QLabel *watchSummaryLabel_ = nullptr;
  QLabel *watchDetailLabel_ = nullptr;
  QCheckBox *startupWatchDiffsOnly_ = nullptr;
  QLabel *startupWatchSummaryLabel_ = nullptr;
  QLabel *startupSdoDetailLabel_ = nullptr;
  QLineEdit *freeRunFilter_ = nullptr;
  QCheckBox *freeRunChangedOnly_ = nullptr;
  QLineEdit *ioVariableFilter_ = nullptr;
  QComboBox *ioVariableScopeFilter_ = nullptr;
  QLabel *ioVariableSummaryLabel_ = nullptr;
  QLabel *ioVariableDetailLabel_ = nullptr;
  QLineEdit *consistencyFilter_ = nullptr;
  QComboBox *consistencyScopeFilter_ = nullptr;
  QLabel *consistencySummaryLabel_ = nullptr;
  QLabel *consistencyDetailLabel_ = nullptr;
  QLineEdit *diagnosticsFilter_ = nullptr;
  QComboBox *diagnosticsLevelFilter_ = nullptr;
  QLabel *hostHealthSummaryLabel_ = nullptr;
  QLabel *diagnosticsSummaryLabel_ = nullptr;
  QLabel *freeRunEntrySummaryLabel_ = nullptr;
  QLabel *freeRunEntryDetailLabel_ = nullptr;
  QLabel *topologyBaselineLabel_ = nullptr;
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
  QVector<SlaveInfo> topologyBaseline_;
  QString projectPath_;
  QString projectName_ = "Untitled";
  AppSettings settings_;
  bool freeRun_ = false;
  bool consistencyFresh_ = false;
  bool selectedSdoWritable_ = true;
};
