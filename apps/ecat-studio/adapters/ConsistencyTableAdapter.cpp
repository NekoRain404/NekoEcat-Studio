// Populates and queries the consistency issue QTableWidget.
#include "ConsistencyTableAdapter.h"

#include "helpers/StudioTableHelpers.h"

#include <QTableWidget>

// Extracts level and scope text for a single consistency issue row.
ConsistencyTableRowState consistencyTableRowState(QTableWidget *table,
                                                  int row) {
  ConsistencyTableRowState state;
  if (!table || row < 0 || row >= table->rowCount()) {
    return state;
  }

  state.level = tableText(table, row, kConsistencyLevelColumn);
  state.scope = tableText(table, row, kConsistencyScopeColumn);
  return state;
}

// Tallies issue severity levels across all rows for badge and summary display.
ConsistencyIssueCounts consistencyTableIssueCounts(QTableWidget *table) {
  ConsistencyIssueCounts counts;
  if (!table) {
    return counts;
  }

  for (int row = 0; row < table->rowCount(); ++row) {
    addConsistencyIssueLevel(
        &counts, consistencyIssueLevelFromText(
                     tableText(table, row, kConsistencyLevelColumn)));
  }
  return counts;
}

// Maps a filter scope string to the corresponding row-level predicate (error, warning, topology, etc.).
bool consistencyTableScopeMatches(const ConsistencyTableRowState &state,
                                  const QString &scope) {
  if (scope == QString::fromLatin1(kConsistencyScopeError)) {
    return consistencyIssueLevelFromText(state.level) ==
           ConsistencyIssueLevel::Error;
  }
  if (scope == QString::fromLatin1(kConsistencyScopeWarning)) {
    return consistencyIssueLevelFromText(state.level) ==
           ConsistencyIssueLevel::Warning;
  }
  if (scope == QString::fromLatin1(kConsistencyScopeTopology)) {
    const QString normalized = state.scope.toLower();
    return normalized.contains("topology") || state.scope.contains("拓扑");
  }
  if (scope == QString::fromLatin1(kConsistencyScopeStartup)) {
    const QString normalized = state.scope.toLower();
    return normalized.contains("startup") || state.scope.contains("启动");
  }
  if (scope == QString::fromLatin1(kConsistencyScopeIo)) {
    const QString normalized = state.scope.toLower();
    return normalized.contains("i/o") || state.scope.contains("变量");
  }
  if (scope == QString::fromLatin1(kConsistencyScopeReady)) {
    return consistencyIssueLevelFromText(state.level) ==
           ConsistencyIssueLevel::Ready;
  }
  return true;
}

// Case-insensitive search across all columns in a row.
bool consistencyTableSearchMatches(QTableWidget *table, int row,
                                   const QString &needle) {
  if (needle.trimmed().isEmpty()) {
    return true;
  }
  if (!table || row < 0 || row >= table->rowCount()) {
    return false;
  }

  for (int column = 0; column < table->columnCount(); ++column) {
    const auto *item = table->item(row, column);
    if (item && item->text().contains(needle, Qt::CaseInsensitive)) {
      return true;
    }
  }
  return false;
}

// Applies scope + text filters to every row, toggles visibility, and returns aggregate stats.
ConsistencyTableFilterStats filterConsistencyTableRows(QTableWidget *table,
                                                       const QString &scope,
                                                       const QString &needle) {
  ConsistencyTableFilterStats stats;
  if (!table) {
    return stats;
  }

  stats.total = table->rowCount();
  for (int row = 0; row < table->rowCount(); ++row) {
    const bool show = consistencyTableScopeMatches(
                          consistencyTableRowState(table, row), scope) &&
                      consistencyTableSearchMatches(table, row, needle);
    table->setRowHidden(row, !show);
    if (show) {
      ++stats.visible;
    }
  }
  return stats;
}

// Locates the first row with an error-level issue, for auto-scroll to the most critical problem.
int firstConsistencyTableBlockingIssueRow(QTableWidget *table) {
  if (!table) {
    return -1;
  }

  for (int row = 0; row < table->rowCount(); ++row) {
    ConsistencyIssueCounts counts;
    addConsistencyIssueLevel(
        &counts, consistencyIssueLevelFromText(
                     tableText(table, row, kConsistencyLevelColumn)));
    if (consistencyHasBlockingIssues(counts)) {
      return row;
    }
  }
  return -1;
}

// Finds the first row flagged as an I/O variable issue for targeted navigation.
int firstConsistencyTableIoIssueRow(QTableWidget *table) {
  if (!table) {
    return -1;
  }

  for (int row = 0; row < table->rowCount(); ++row) {
    const ConsistencyTableRowState state = consistencyTableRowState(table, row);
    if (consistencyTableScopeMatches(
            state, QString::fromLatin1(kConsistencyScopeIo)) &&
        consistencyIssueLevelFromText(state.level) !=
            ConsistencyIssueLevel::Ready) {
      return row;
    }
  }
  return -1;
}

// Quick check: whether the consistency table has any rows at all.
bool consistencyTableAvailable(QTableWidget *table) {
  return table && table->rowCount() > 0;
}
