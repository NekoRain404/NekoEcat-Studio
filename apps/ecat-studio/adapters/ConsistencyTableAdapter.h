#pragma once

// Populates and queries the consistency issue QTableWidget.


#include "models/ConsistencyModel.h"

#include <QString>

class QTableWidget;

inline constexpr int kConsistencyLevelColumn = 0;
inline constexpr int kConsistencyScopeColumn = 1;
inline constexpr int kConsistencyTargetColumn = 2;
inline constexpr int kConsistencyEvidenceColumn = 3;
inline constexpr int kConsistencyExpectedColumn = 4;
inline constexpr int kConsistencyActualColumn = 5;
inline constexpr int kConsistencyActionColumn = 6;

inline constexpr const char* kConsistencyScopeAll = "all";
inline constexpr const char* kConsistencyScopeError = "error";
inline constexpr const char* kConsistencyScopeWarning = "warning";
inline constexpr const char* kConsistencyScopeTopology = "topology";
inline constexpr const char* kConsistencyScopeStartup = "startup";
inline constexpr const char* kConsistencyScopeIo = "io";
inline constexpr const char* kConsistencyScopeReady = "ready";

// Level and scope text for a single consistency issue row.
struct ConsistencyTableRowState {
    QString level;
    QString scope;
};

// Counts returned after filtering, for status bar and badge updates.
struct ConsistencyTableFilterStats {
    int visible = 0;
    int total = 0;
};

// Extracts level and scope for filtering decisions.
ConsistencyTableRowState consistencyTableRowState(QTableWidget* table, int row);
// Tallies severity levels across all rows.
ConsistencyIssueCounts consistencyTableIssueCounts(QTableWidget* table);
// Tests whether a row matches the active filter scope.
bool consistencyTableScopeMatches(const ConsistencyTableRowState& state, const QString& scope);
// Case-insensitive full-row text search.
bool consistencyTableSearchMatches(QTableWidget* table, int row, const QString& needle);
// Applies scope + text filters and returns aggregate counts.
ConsistencyTableFilterStats filterConsistencyTableRows(QTableWidget* table, const QString& scope,
                                                       const QString& needle);
// First error-level row for auto-scroll to the most critical issue.
int firstConsistencyTableBlockingIssueRow(QTableWidget* table);
// First I/O-specific issue row for targeted navigation.
int firstConsistencyTableIoIssueRow(QTableWidget* table);
// Whether the consistency table has any data rows.
bool consistencyTableAvailable(QTableWidget* table);
