#pragma once

// Updates workspace tab badge text and severity indicators.


#include "detail/WorkspaceTabBadgeDetail.h"

#include <QString>

class QTableWidget;

inline constexpr int kWorkspaceTabBadgeWatchDeltaColumn = 11;
inline constexpr int kWorkspaceTabBadgeStateRiskColumn = 8;
inline constexpr int kWorkspaceTabBadgeDiagnosticsLevelColumn = 1;

// Aggregates pointers to all workspace tables needed for badge count computation.
struct WorkspaceTabBadgeTables {
  QTableWidget *watchTable = nullptr;
  QTableWidget *startupSdoTable = nullptr;
  QTableWidget *freeRunEntryTable = nullptr;
  QTableWidget *ioVariableTable = nullptr;
  QTableWidget *consistencyTable = nullptr;
  QTableWidget *stateMachineTable = nullptr;
  QTableWidget *diagnosticsTable = nullptr;
  QTableWidget *slaveEvidenceMatrixTable = nullptr;
};

// Whether a delta value represents a real mismatch.
bool workspaceTabBadgeWatchDeltaIsIssue(const QString &delta);
// Count of watch rows with non-matching startup deltas.
int countWorkspaceTabBadgeWatchStartupDiffs(QTableWidget *watchTable);
// Count of I/O variable rows with any issue type.
int countWorkspaceTabBadgeIoIssues(QTableWidget *ioVariableTable);
// Aggregates all issue counts for tab badge rendering.
WorkspaceTabBadgeCounts
workspaceTabBadgeCounts(const WorkspaceTabBadgeTables &tables);
