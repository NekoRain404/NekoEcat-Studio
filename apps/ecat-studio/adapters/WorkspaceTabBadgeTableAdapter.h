#pragma once

// Updates workspace tab badge text and severity indicators.


#include "ui_state/WorkspaceTabBadgeUiState.h"

#include <QString>

class QTableWidget;

inline constexpr int kWorkspaceTabBadgeWatchDeltaColumn = 11;
inline constexpr int kWorkspaceTabBadgeStateRiskColumn = 8;
inline constexpr int kWorkspaceTabBadgeDiagnosticsLevelColumn = 1;

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

bool workspaceTabBadgeWatchDeltaIsIssue(const QString &delta);
int countWorkspaceTabBadgeWatchStartupDiffs(QTableWidget *watchTable);
int countWorkspaceTabBadgeIoIssues(QTableWidget *ioVariableTable);
WorkspaceTabBadgeCounts
workspaceTabBadgeCounts(const WorkspaceTabBadgeTables &tables);
