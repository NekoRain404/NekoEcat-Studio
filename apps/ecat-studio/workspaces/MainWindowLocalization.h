// MainWindowLocalization.h — Localization, master selection, status bar, and
//   text struct factories (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.
//
// Provides bilingual (English/Chinese) UI text lookup, EtherCAT master
// selection and refresh, status bar updates, logging, slave selection
// helpers, workspace boundary and next-best-action logic, and factory
// functions that build the localized text structs consumed by every detail
// panel in the application.

// ── Localization & Master Selection ───────────────────────────
QString uiText(const QString& english, const QString& zh) const;
void applyLanguage(const QString& localeCode);
void rebuildWorkbench();
QString activeMasterName() const;
void refreshMasterSelector();
void setActiveMaster(const QString& target);
bool confirmDangerousOperation(const QString& title, const QString& summary, const QStringList& details,
                               const QString& confirmText);

// ── Status Bar & Logging ──────────────────────────────────────
void updateStatusBar();
void log(const QString& message);
int selectedPosition() const;
QVector<int> selectedDictionaryRows() const;
QVector<int> selectedSdoHistoryRows() const;
QVector<int> selectedStartupSdoRows() const;

// ── Slave Selection ───────────────────────────────────────────
void setSelectedSlave(int position);

// ── Workspace Boundary & Badges ───────────────────────────────
void updateWorkspaceBoundary();
void updateNextBestAction();
void updateTabBadges();
void runNextBestAction();

// ── About Dialog ─────────────────────────────────────────────
void showAboutDialog();

// ── Text Structs ──────────────────────────────────────────────
WorkflowStepDetailTexts workflowStepDetailTexts() const;
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
SlaveEvidenceSummaryTexts slaveEvidenceSummaryTexts() const;
SlaveEvidenceUiTexts slaveEvidenceUiTexts() const;
WorkspaceBoundaryTexts workspaceBoundaryTexts() const;
WorkspaceTabBadgeTexts workspaceTabBadgeTexts() const;
