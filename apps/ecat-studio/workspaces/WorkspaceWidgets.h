#pragma once

// WorkspaceWidgets.h — aggregated widget-pointer structs for each workspace.
//
// Keeps MainWindow.h lean by grouping the QTableWidget, filter controls,
// summary/detail labels, and action buttons that belong to a single workspace
// tab into its own struct. MainWindow owns one pointer to each struct; the
// workspace partial-class .cpp files allocate and populate them during
// buildUi(). Separate structs exist for the SDO/PDO dictionary, Watch,
// Free Run, I/O variables, Consistency, Workflow, Session, Slave Evidence,
// State Machine, Diagnostics, Bookmarks, SDO Inspector, and raw-text panels.

#include <QVector>

class QCheckBox;
class QPlainTextEdit;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableWidget;

// ── SDO / PDO object dictionary workspace ──────────────────────────────────
struct SdoWorkspaceWidgets {
    QTableWidget* sdoTable = nullptr;
    QTableWidget* pdoTable = nullptr;
    QLineEdit* sdoFilter = nullptr;
    QLineEdit* pdoFilter = nullptr;
    QLabel* sdoSummaryLabel = nullptr;
    QLabel* pdoSummaryLabel = nullptr;
    QLabel* pdoDetailLabel = nullptr;
};

// ── Watch (live SDO polling) workspace ─────────────────────────────────────
struct WatchWorkspaceWidgets {
    QTableWidget* watchTable = nullptr;
    QLineEdit* watchFilter = nullptr;
    QCheckBox* watchAutoRefresh = nullptr;
    QCheckBox* watchChangedOnly = nullptr;
    QComboBox* watchScopeFilter = nullptr;
    QComboBox* watchRefreshInterval = nullptr;
    QLabel* watchSummaryLabel = nullptr;
    QLabel* watchDetailLabel = nullptr;
    QCheckBox* startupWatchDiffsOnly = nullptr;
    QLabel* startupWatchSummaryLabel = nullptr;
    QLabel* startupSdoDetailLabel = nullptr;
};

// ── Free Run entry workspace ───────────────────────────────────────────────
struct FreeRunWorkspaceWidgets {
    QTableWidget* freeRunEntryTable = nullptr;
    QTableWidget* freeRunTable = nullptr;
    QLineEdit* freeRunFilter = nullptr;
    QCheckBox* freeRunChangedOnly = nullptr;
    QLabel* freeRunEntrySummaryLabel = nullptr;
    QLabel* freeRunEntryDetailLabel = nullptr;
};

// ── I/O Variable workspace ─────────────────────────────────────────────────
struct IoVariableWorkspaceWidgets {
    QTableWidget* ioVariableTable = nullptr;
    QLineEdit* ioVariableFilter = nullptr;
    QComboBox* ioVariableScopeFilter = nullptr;
    QLabel* ioVariableSummaryLabel = nullptr;
    QLabel* ioVariableDetailLabel = nullptr;
};

// ── Consistency / commissioning gate workspace ─────────────────────────────
struct ConsistencyWorkspaceWidgets {
    QTableWidget* consistencyTable = nullptr;
    QLineEdit* consistencyFilter = nullptr;
    QComboBox* consistencyScopeFilter = nullptr;
    QLabel* consistencySummaryLabel = nullptr;
    QLabel* consistencyDetailLabel = nullptr;
};

// ── Commissioning workflow workspace ───────────────────────────────────────
struct WorkflowWorkspaceWidgets {
    QTableWidget* workflowTable = nullptr;
    QLabel* workflowSummaryLabel = nullptr;
    QLabel* workflowStepDetailLabel = nullptr;
    QComboBox* workflowScopeFilter = nullptr;
    QLineEdit* workflowFilter = nullptr;
    QPushButton* workflowReviewButton = nullptr;
    QPushButton* workflowReviewNextButton = nullptr;
    QPushButton* workflowStepCopyButton = nullptr;
};

// ── Session brief workspace ────────────────────────────────────────────────
struct SessionWorkspaceWidgets {
    QTableWidget* sessionBriefTable = nullptr;
    QPushButton* sessionBriefCopyButton = nullptr;
};

// ── Slave evidence matrix workspace ────────────────────────────────────────
struct SlaveEvidenceWorkspaceWidgets {
    QTableWidget* slaveEvidenceMatrixTable = nullptr;
    QLabel* slaveEvidenceMatrixSummaryLabel = nullptr;
    QPushButton* slaveEvidenceMatrixReviewButton = nullptr;
    QPushButton* slaveEvidenceMatrixReviewNextButton = nullptr;
    QPushButton* slaveEvidenceMatrixCopyButton = nullptr;
    QVector<QPushButton*> slaveEvidenceMatrixTriageButtons;
    QLineEdit* slaveEvidenceMatrixFilter = nullptr;
    QComboBox* slaveEvidenceMatrixScopeFilter = nullptr;
};

// ── State machine recommendation workspace ─────────────────────────────────
struct StateMachineWorkspaceWidgets {
    QTableWidget* stateMachineTable = nullptr;
    QLabel* stateMachineSummaryLabel = nullptr;
    QLabel* stateMachineDetailLabel = nullptr;
};

// ── Diagnostics / host health workspace ────────────────────────────────────
struct DiagnosticsWorkspaceWidgets {
    QTableWidget* diagnosticsTable = nullptr;
    QLineEdit* diagnosticsFilter = nullptr;
    QComboBox* diagnosticsLevelFilter = nullptr;
    QLabel* hostHealthSummaryLabel = nullptr;
    QLabel* diagnosticsSummaryLabel = nullptr;
    QLabel* topologyBaselineLabel = nullptr;
};

// ── Object bookmark workspace ──────────────────────────────────────────────
struct BookmarkWorkspaceWidgets {
    QTableWidget* objectBookmarkTable = nullptr;
};

// ── SDO Inspector panel (right side of Object Dictionary page) ─────────────
struct SdoInspectorWidgets {
    QLabel* sdoInspectorLabel = nullptr;
    QTableWidget* sdoTargetTable = nullptr;
    QLineEdit* sdoIndex = nullptr;
    QLineEdit* sdoSubIndex = nullptr;
    QLineEdit* sdoValue = nullptr;
    QLineEdit* sdoWriteValue = nullptr;
    QPushButton* useSdoValueButton = nullptr;
    QComboBox* sdoType = nullptr;
};

// ── Raw text panels (master, slave info, PDO, SDO, XML, log, notes) ───────
struct RawTextWidgets {
    QPlainTextEdit* masterText = nullptr;
    QPlainTextEdit* infoText = nullptr;
    QPlainTextEdit* pdoText = nullptr;
    QPlainTextEdit* sdoText = nullptr;
    QPlainTextEdit* xmlText = nullptr;
    QPlainTextEdit* logText = nullptr;
    QPlainTextEdit* projectNotes = nullptr;
};
