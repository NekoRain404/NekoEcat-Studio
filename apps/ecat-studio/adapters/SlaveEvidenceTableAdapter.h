#pragma once

// Populates and queries the slave evidence matrix QTableWidget.


#include "models/SlaveEvidenceModel.h"

class QTableWidget;

inline constexpr int kSlaveEvidenceWatchPositionColumn = 1;
inline constexpr int kSlaveEvidenceWatchIndexColumn = 2;
inline constexpr int kSlaveEvidenceWatchValueColumn = 4;
inline constexpr int kSlaveEvidenceWatchDecodedColumn = 5;

inline constexpr int kSlaveEvidenceStartupPositionColumn = 0;
inline constexpr int kSlaveEvidenceStartupDeltaColumn = 8;

inline constexpr int kSlaveEvidenceProcessPositionColumn = 0;
inline constexpr int kSlaveEvidenceProcessMapColumn = 13;

inline constexpr int kSlaveEvidenceMatrixPriorityColumn = 0;
inline constexpr int kSlaveEvidenceMatrixPositionColumn = 1;
inline constexpr int kSlaveEvidenceMatrixNameColumn = 2;
inline constexpr int kSlaveEvidenceMatrixStateColumn = 3;
inline constexpr int kSlaveEvidenceMatrixReadinessColumn = 4;
inline constexpr int kSlaveEvidenceMatrixOdColumn = 5;
inline constexpr int kSlaveEvidenceMatrixPdoColumn = 6;
inline constexpr int kSlaveEvidenceMatrixWatchColumn = 7;
inline constexpr int kSlaveEvidenceMatrixStartupColumn = 8;
inline constexpr int kSlaveEvidenceMatrixProcessColumn = 9;
inline constexpr int kSlaveEvidenceMatrixRiskColumn = 10;
inline constexpr int kSlaveEvidenceMatrixNextColumn = 11;

inline constexpr const char* kSlaveEvidenceScopePriorityP0 = "priorityP0";
inline constexpr const char* kSlaveEvidenceScopePriorityP1 = "priorityP1";
inline constexpr const char* kSlaveEvidenceScopePriorityP2 = "priorityP2";
inline constexpr const char* kSlaveEvidenceScopePriorityP3 = "priorityP3";
inline constexpr const char* kSlaveEvidenceScopeAll = "all";
inline constexpr const char* kSlaveEvidenceScopeRisk = "risk";
inline constexpr const char* kSlaveEvidenceScopeAction = "action";
inline constexpr const char* kSlaveEvidenceScopeReady = "ready";
inline constexpr const char* kSlaveEvidenceScopeMissingOd = "missingOd";
inline constexpr const char* kSlaveEvidenceScopeMissingPdo = "missingPdo";
inline constexpr const char* kSlaveEvidenceScopeMissingWatch = "missingWatch";
inline constexpr const char* kSlaveEvidenceScopeStartupDiff = "startupDiff";
inline constexpr const char* kSlaveEvidenceScopeProcessMissing = "processMissing";

// Table pointers provided after lazy-loading completes for watch, startup, and process tables.
struct SlaveEvidenceLoadedTables {
    QTableWidget* watchTable = nullptr;
    QTableWidget* startupTable = nullptr;
    QTableWidget* processTable = nullptr;
};

// Tracks which slave position each evidence table was loaded for, to avoid redundant scans.
struct SlaveEvidenceLoadedPositions {
    int identityPosition = -1;
    int identityRows = 0;
    int odPosition = -1;
    int odRows = 0;
    int pdoPosition = -1;
    int pdoRows = 0;
};

// Derived boolean flags describing a single slave evidence matrix row.
struct SlaveEvidenceMatrixRowState {
    bool hasRisk = false;
    bool isReady = false;
    bool isAction = false;
    bool reviewIssue = false;
    bool priorityP0 = false;
    bool priorityP1 = false;
    bool priorityP2 = false;
    bool priorityP3 = false;
    bool missingOd = false;
    bool missingPdo = false;
    bool missingWatch = false;
    bool startupDiff = false;
    bool processMissing = false;
};

// Aggregate counts after filtering, for badges and status summaries.
struct SlaveEvidenceMatrixFilterStats {
    int visible = 0;
    int risk = 0;
    int action = 0;
    int ready = 0;
    int p0 = 0;
    int p1 = 0;
    int p2 = 0;
    int p3 = 0;
    bool hasVisibleIssue = false;
};

// Per-priority-tier row counts (P0 through P3).
struct SlaveEvidenceMatrixPriorityCounts {
    int p0 = 0;
    int p1 = 0;
    int p2 = 0;
    int p3 = 0;
};

// Aggregates all evidence sources into the slave input model.
void applyLoadedSlaveEvidence(SlaveEvidenceInput* input, const SlaveEvidenceLoadedPositions& positions,
                              const SlaveEvidenceLoadedTables& tables);
// Derives boolean flags for a matrix row.
SlaveEvidenceMatrixRowState slaveEvidenceMatrixRowState(QTableWidget* table, int row);
// Tests whether a row passes the active filter scope.
bool slaveEvidenceMatrixScopeMatches(const SlaveEvidenceMatrixRowState& state, const QString& scope);
// Case-insensitive full-row text search.
bool slaveEvidenceMatrixSearchMatches(QTableWidget* table, int row, const QString& needle);
// Applies scope + text filters and returns aggregate counts.
SlaveEvidenceMatrixFilterStats filterSlaveEvidenceMatrixTable(QTableWidget* table, const QString& scope,
                                                              const QString& needle);
// Counts rows in each priority tier.
SlaveEvidenceMatrixPriorityCounts slaveEvidenceMatrixPriorityCounts(QTableWidget* table);
// Outputs per-priority counts via out-parameters.
void countSlaveEvidenceMatrixPriorities(QTableWidget* table, int* p0, int* p1, int* p2, int* p3);
// Stores navigation route target on the position cell.
void setSlaveEvidenceMatrixRouteTarget(QTableWidget* table, int row, SlaveEvidenceRouteTarget target);
// Retrieves the stored route target with Overview as default.
SlaveEvidenceRouteTarget slaveEvidenceMatrixRouteTargetForRow(QTableWidget* table, int row);
// First row matching a slave position in a given column.
int firstSlaveEvidenceRowForPosition(QTableWidget* table, int position, int positionColumn);
// First startup diff row for a specific slave.
int firstSlaveEvidenceStartupDiffRow(QTableWidget* startupTable, int position);
// First process data issue row for a specific slave.
int firstSlaveEvidenceProcessIssueRow(QTableWidget* processTable, int position);
// First drive-related watch row for a specific slave.
int firstSlaveEvidenceDriveWatchRow(QTableWidget* watchTable, int position);
