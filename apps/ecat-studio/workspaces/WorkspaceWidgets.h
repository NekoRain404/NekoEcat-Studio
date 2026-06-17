#pragma once

// Aggregated widget pointers for each workspace — keeps MainWindow.h lean.
// Each struct groups the QTableWidget, filter, summary/detail labels, and
// action buttons that belong to a single workspace tab.  MainWindow owns one
// pointer to each struct; workspace partial-class files allocate and populate
// them during buildUi().

#include <QVector>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableWidget;

// ── SDO / PDO object dictionary workspace ──────────────────────────────────
struct SdoWorkspaceWidgets {
    QTableWidget *sdoTable = nullptr;
    QTableWidget *pdoTable = nullptr;
    QLineEdit    *sdoFilter = nullptr;
    QLineEdit    *pdoFilter = nullptr;
    QLabel       *sdoSummaryLabel = nullptr;
    QLabel       *pdoSummaryLabel = nullptr;
    QLabel       *pdoDetailLabel = nullptr;
};

// ── Watch (live SDO polling) workspace ─────────────────────────────────────
struct WatchWorkspaceWidgets {
    QTableWidget *watchTable = nullptr;
    QLineEdit    *watchFilter = nullptr;
    QCheckBox    *watchAutoRefresh = nullptr;
    QCheckBox    *watchChangedOnly = nullptr;
    QComboBox    *watchScopeFilter = nullptr;
    QComboBox    *watchRefreshInterval = nullptr;
    QLabel       *watchSummaryLabel = nullptr;
    QLabel       *watchDetailLabel = nullptr;
    QCheckBox    *startupWatchDiffsOnly = nullptr;
    QLabel       *startupWatchSummaryLabel = nullptr;
    QLabel       *startupSdoDetailLabel = nullptr;
};

// ── Free Run entry workspace ───────────────────────────────────────────────
struct FreeRunWorkspaceWidgets {
    QTableWidget *freeRunEntryTable = nullptr;
    QTableWidget    *freeRunTable = nullptr;
    QLineEdit    *freeRunFilter = nullptr;
    QCheckBox    *freeRunChangedOnly = nullptr;
    QLabel       *freeRunEntrySummaryLabel = nullptr;
    QLabel       *freeRunEntryDetailLabel = nullptr;
};

// ── I/O Variable workspace ─────────────────────────────────────────────────
struct IoVariableWorkspaceWidgets {
    QTableWidget *ioVariableTable = nullptr;
    QLineEdit    *ioVariableFilter = nullptr;
    QComboBox    *ioVariableScopeFilter = nullptr;
    QLabel       *ioVariableSummaryLabel = nullptr;
    QLabel       *ioVariableDetailLabel = nullptr;
};

// ── Consistency / commissioning gate workspace ─────────────────────────────
struct ConsistencyWorkspaceWidgets {
    QTableWidget *consistencyTable = nullptr;
    QLineEdit    *consistencyFilter = nullptr;
    QComboBox    *consistencyScopeFilter = nullptr;
    QLabel       *consistencySummaryLabel = nullptr;
    QLabel       *consistencyDetailLabel = nullptr;
};

// ── Commissioning workflow workspace ───────────────────────────────────────
struct WorkflowWorkspaceWidgets {
    QTableWidget *workflowTable = nullptr;
    QLabel       *workflowSummaryLabel = nullptr;
    QLabel       *workflowStepDetailLabel = nullptr;
    QComboBox    *workflowScopeFilter = nullptr;
    QLineEdit    *workflowFilter = nullptr;
    QPushButton  *workflowReviewButton = nullptr;
    QPushButton  *workflowReviewNextButton = nullptr;
    QPushButton  *workflowStepCopyButton = nullptr;
};

// ── Session brief workspace ────────────────────────────────────────────────
struct SessionWorkspaceWidgets {
    QTableWidget *sessionBriefTable = nullptr;
    QPushButton  *sessionBriefCopyButton = nullptr;
};

// ── Slave evidence matrix workspace ────────────────────────────────────────
struct SlaveEvidenceWorkspaceWidgets {
    QTableWidget *slaveEvidenceMatrixTable = nullptr;
    QLabel       *slaveEvidenceMatrixSummaryLabel = nullptr;
    QPushButton  *slaveEvidenceMatrixReviewButton = nullptr;
    QPushButton  *slaveEvidenceMatrixReviewNextButton = nullptr;
    QPushButton  *slaveEvidenceMatrixCopyButton = nullptr;
    QVector<QPushButton *> slaveEvidenceMatrixTriageButtons;
    QLineEdit    *slaveEvidenceMatrixFilter = nullptr;
    QComboBox    *slaveEvidenceMatrixScopeFilter = nullptr;
};

// ── State machine recommendation workspace ─────────────────────────────────
struct StateMachineWorkspaceWidgets {
    QTableWidget *stateMachineTable = nullptr;
    QLabel       *stateMachineSummaryLabel = nullptr;
    QLabel       *stateMachineDetailLabel = nullptr;
};

// ── Diagnostics / host health workspace ────────────────────────────────────
struct DiagnosticsWorkspaceWidgets {
    QTableWidget *diagnosticsTable = nullptr;
    QLineEdit    *diagnosticsFilter = nullptr;
    QComboBox    *diagnosticsLevelFilter = nullptr;
    QLabel       *hostHealthSummaryLabel = nullptr;
    QLabel       *diagnosticsSummaryLabel = nullptr;
    QLabel       *topologyBaselineLabel = nullptr;
};

// ── Object bookmark workspace ──────────────────────────────────────────────
struct BookmarkWorkspaceWidgets {
    QTableWidget *objectBookmarkTable = nullptr;
};
