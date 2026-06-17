// Updates workspace tab badge text and severity indicators.
#include "WorkspaceTabBadgeTableAdapter.h"

#include "ConsistencyTableAdapter.h"
#include "ui_state/DiagnosticsEventUiState.h"
#include "models/ProcessDataRowModel.h"
#include "ProcessDataTableAdapter.h"
#include "SlaveEvidenceTableAdapter.h"
#include "helpers/StudioTableHelpers.h"
#include "models/WatchStartupModel.h"
#include "WatchStartupTableAdapter.h"

#include <QStringList>
#include <QTableWidget>

// Determines if a watch delta value represents a real mismatch (not "match" or "pending").
bool workspaceTabBadgeWatchDeltaIsIssue(const QString &delta) {
  const QString normalized = delta.trimmed().toLower();
  return !normalized.isEmpty() && normalized != QStringLiteral("match") &&
         normalized != QStringLiteral("匹配") &&
         normalized != QStringLiteral("pending") &&
         normalized != QStringLiteral("待比较");
}

// Counts watch rows with non-matching startup deltas for the Watch tab badge.
int countWorkspaceTabBadgeWatchStartupDiffs(QTableWidget *watchTable) {
  int diffs = 0;
  if (!watchTable) {
    return diffs;
  }
    // Iterate over collection
  for (int row = 0; row < watchTable->rowCount(); ++row) {
    if (workspaceTabBadgeWatchDeltaIsIssue(
            tableText(watchTable, row, kWorkspaceTabBadgeWatchDeltaColumn))) {
      ++diffs;
    }
  }
  return diffs;
}

// Counts I/O variable rows with startup diffs, PDO map issues, missing values, or PLC errors.
int countWorkspaceTabBadgeIoIssues(QTableWidget *ioVariableTable) {
  int issues = 0;
  if (!ioVariableTable) {
    return issues;
  }

    // Iterate over collection
  for (int row = 0; row < ioVariableTable->rowCount(); ++row) {
    const IoVariableTableRow variable =
        ioVariableTableRowFromTable(ioVariableTable, row);
    if (ioVariableTableRowHasStartupDiff(variable) ||
        ioVariableTableRowHasPdoMapIssue(variable) ||
        ioVariableTableRowHasMissingValue(variable) ||
        ioVariableTableRowHasPlcIssue(variable, QStringLiteral("Ready"))) {
      ++issues;
    }
  }
  return issues;
}

// Aggregates issue counts from all workspace tables to drive tab badge text and severity.
WorkspaceTabBadgeCounts
workspaceTabBadgeCounts(const WorkspaceTabBadgeTables &tables) {
  WorkspaceTabBadgeCounts counts;
  counts.watchRows = tables.watchTable ? tables.watchTable->rowCount() : 0;
  counts.watchStartupDiffs =
      countWorkspaceTabBadgeWatchStartupDiffs(tables.watchTable);
  counts.startupRows =
      tables.startupSdoTable ? tables.startupSdoTable->rowCount() : 0;
  counts.startupDiffs = startupRowsWithWatchDiffs(
                            evaluateStartupWatchDeltas(
                                watchStartupStartupRows(tables.startupSdoTable),
                                watchStartupWatchRows(tables.watchTable)))
                            .size();
  counts.freeRunRows =
      tables.freeRunEntryTable ? tables.freeRunEntryTable->rowCount() : 0;
  counts.ioRows =
      tables.ioVariableTable ? tables.ioVariableTable->rowCount() : 0;
  counts.ioIssues = countWorkspaceTabBadgeIoIssues(tables.ioVariableTable);
  counts.consistencyRows =
      tables.consistencyTable ? tables.consistencyTable->rowCount() : 0;

  const ConsistencyIssueCounts consistency =
      consistencyTableIssueCounts(tables.consistencyTable);
  counts.consistencyErrors = consistency.errors;
  counts.consistencyWarnings = consistency.warnings;
  counts.consistencyInfos = consistency.infos;
  counts.consistencyReady = consistency.ready;

  if (tables.stateMachineTable) {
    // Iterate over collection
    for (int row = 0; row < tables.stateMachineTable->rowCount(); ++row) {
      if (!tableText(tables.stateMachineTable, row,
                     kWorkspaceTabBadgeStateRiskColumn)
               .trimmed()
               .isEmpty()) {
        ++counts.stateRiskRows;
      }
    }
  }

  QStringList diagnosticLevels;
  if (tables.diagnosticsTable) {
    counts.diagnosticRows = tables.diagnosticsTable->rowCount();
    diagnosticLevels.reserve(tables.diagnosticsTable->rowCount());
    // Iterate over collection
    for (int row = 0; row < tables.diagnosticsTable->rowCount(); ++row) {
      diagnosticLevels.append(
          tableText(tables.diagnosticsTable, row,
                    kWorkspaceTabBadgeDiagnosticsLevelColumn));
    }
  }
  const DiagnosticsEventSummary diagnostics =
      diagnosticsEventCounts(diagnosticLevels);
  counts.diagnosticErrors = diagnostics.errors;
  counts.diagnosticWarnings = diagnostics.warnings;
  counts.diagnosticInfos = diagnostics.infos;

  const SlaveEvidenceMatrixPriorityCounts matrix =
      slaveEvidenceMatrixPriorityCounts(tables.slaveEvidenceMatrixTable);
  counts.matrixP0 = matrix.p0;
  counts.matrixP1 = matrix.p1;
  counts.matrixP2 = matrix.p2;
  counts.matrixP3 = matrix.p3;
  return counts;
}
