#include "IoVariableFilterModel.h"

#include "ProcessDataRowModel.h"

IoVariableFilterRowState ioVariableFilterRowState(const IoVariableTableRow &row,
                                                  int selectedPosition,
                                                  const QString &readyText) {
  IoVariableFilterRowState state;
  state.selected = row.positionValid && row.position == selectedPosition;
  state.process = ioVariableTableRowHasProcessSource(row);
  state.pdo = ioVariableTableRowHasPdoSource(row);
  state.watchEvidence = ioVariableTableRowHasWatchEvidence(row);
  state.startupDiff = ioVariableTableRowHasStartupDiff(row);
  state.missingValue = ioVariableTableRowHasMissingValue(row);
  state.changed = ioVariableTableRowHasChangedValue(row);
  state.plcIssue = ioVariableTableRowHasPlcIssue(row, readyText);
  state.rx = ioVariableTableRowIsRx(row);
  state.tx = ioVariableTableRowIsTx(row);
  state.cia402 = ioVariableTableRowIsCia402(row);
  return state;
}

bool ioVariableFilterScopeMatches(const IoVariableFilterRowState &state,
                                  const QString &scope) {
  if (scope == QString::fromLatin1(kIoVariableScopeSelected)) {
    return state.selected;
  }
  if (scope == QString::fromLatin1(kIoVariableScopeProcess)) {
    return state.process;
  }
  if (scope == QString::fromLatin1(kIoVariableScopePdo)) {
    return state.pdo && !state.process;
  }
  if (scope == QString::fromLatin1(kIoVariableScopeWatch)) {
    return state.watchEvidence;
  }
  if (scope == QString::fromLatin1(kIoVariableScopeStartupDiff)) {
    return state.startupDiff;
  }
  if (scope == QString::fromLatin1(kIoVariableScopeMissingValue)) {
    return state.missingValue;
  }
  if (scope == QString::fromLatin1(kIoVariableScopeRx)) {
    return state.rx;
  }
  if (scope == QString::fromLatin1(kIoVariableScopeTx)) {
    return state.tx;
  }
  if (scope == QString::fromLatin1(kIoVariableScopeCia402)) {
    return state.cia402;
  }
  if (scope == QString::fromLatin1(kIoVariableScopeChanged)) {
    return state.changed;
  }
  if (scope == QString::fromLatin1(kIoVariableScopePlcIssues)) {
    return state.plcIssue;
  }
  return true;
}

bool ioVariableFilterTextMatches(const QStringList &cells,
                                 const QString &needle) {
  const QString trimmedNeedle = needle.trimmed();
  if (trimmedNeedle.isEmpty()) {
    return true;
  }
  for (const QString &cell : cells) {
    if (cell.contains(trimmedNeedle, Qt::CaseInsensitive)) {
      return true;
    }
  }
  return false;
}

IoVariableFilterDecision
evaluateIoVariableFilterRow(const IoVariableTableRow &row,
                            const QStringList &cells, const QString &scope,
                            const QString &needle, int selectedPosition,
                            const QString &readyText) {
  IoVariableFilterDecision decision;
  decision.state = ioVariableFilterRowState(row, selectedPosition, readyText);
  decision.visible = ioVariableFilterScopeMatches(decision.state, scope) &&
                     ioVariableFilterTextMatches(cells, needle);
  return decision;
}

void accumulateIoVariableFilterStats(IoVariableFilterStats *stats,
                                     const IoVariableFilterDecision &decision) {
  if (!stats) {
    return;
  }

  ++stats->total;
  if (decision.state.process) {
    ++stats->processRows;
  }
  if (decision.state.watchEvidence) {
    ++stats->watchRows;
  }
  if (decision.state.startupDiff) {
    ++stats->startupDiffs;
  }
  if (decision.state.missingValue) {
    ++stats->missingValues;
  }
  if (decision.state.changed) {
    ++stats->changedRows;
  }
  if (decision.state.plcIssue) {
    ++stats->plcIssues;
  }
  if (decision.visible) {
    ++stats->visible;
  }
}

QString ioVariableFilterSummaryText(const IoVariableFilterStats &stats,
                                    const QString &scopeLabel,
                                    const QString &summaryPattern) {
  return QString(summaryPattern)
      .arg(stats.visible)
      .arg(stats.total)
      .arg(scopeLabel)
      .arg(stats.processRows)
      .arg(stats.watchRows)
      .arg(stats.startupDiffs)
      .arg(stats.missingValues)
      .arg(stats.changedRows)
      .arg(stats.plcIssues);
}
