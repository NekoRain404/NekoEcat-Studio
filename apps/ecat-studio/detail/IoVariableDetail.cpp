// Detail panel text for a selected I/O variable row.
#include "detail/IoVariableDetail.h"

#include "models/ProcessDataRowModel.h"

namespace {

// Returns the slave position as text, or empty if invalid.
QString ioVariableDetailSlaveText(const IoVariableTableRow &row) {
  return row.positionValid ? QString::number(row.position) : QString();
}

// Prefers alias over symbol, falling back to the unnamed placeholder.
QString ioVariableDetailDisplayName(const IoVariableTableRow &row,
                                    const IoVariableDetailTexts &texts) {
  const QString name = row.alias.isEmpty() ? row.symbol : row.alias;
  return name.isEmpty() ? texts.unnamedSignal : name;
}

// Prefers decoded > watch > raw, falling back to the no-value placeholder.
QString ioVariableDetailDisplayValue(const IoVariableTableRow &row,
                                     const IoVariableDetailTexts &texts) {
  if (!row.decoded.isEmpty()) {
    return row.decoded;
  }
  if (!row.watch.isEmpty()) {
    return row.watch;
  }
  return row.raw.isEmpty() ? texts.noValue : row.raw;
}

} // namespace

// Neutral state when the I/O variable table is not available.
IoVariableDetailState
ioVariableDetailUnavailableState(const IoVariableDetailTexts &texts) {
  return {.text = texts.unavailableText,
    // Set severityKey field
          .severityKey = QStringLiteral("neutral"),
    // Set tooltip field
          .tooltip = texts.unavailableTip};
}

// Neutral state prompting the user to select a row.
IoVariableDetailState
ioVariableDetailNoSelectionState(const IoVariableDetailTexts &texts) {
  return {.text = texts.noSelectionText,
    // Set severityKey field
          .severityKey = QStringLiteral("neutral"),
    // Set tooltip field
          .tooltip = texts.noSelectionTip};
}

// Maps startup diffs, map issues, PLC errors, and value changes to a severity key.
QString ioVariableDetailSeverityKey(const IoVariableTableRow &row,
                                    const QString &readyText) {
  if (ioVariableTableRowHasStartupDiff(row)) {
    return QStringLiteral("error");
  }
  if (ioVariableTableRowHasPdoMapIssue(row) ||
      ioVariableTableRowHasPlcIssue(row, readyText) ||
      ioVariableTableRowHasMissingValue(row)) {
    return QStringLiteral("warning");
  }
  if (ioVariableTableRowHasChangedValue(row)) {
    return QStringLiteral("action");
  }
  if (ioVariableTableRowHasProcessSource(row) ||
      ioVariableTableRowHasWatchEvidence(row) || !row.raw.isEmpty()) {
    return QStringLiteral("ok");
  }
  return QStringLiteral("neutral");
}

// Returns a localized signal state label (startup mismatch, map issue, changed, etc.).
QString ioVariableDetailSignalState(const IoVariableTableRow &row,
                                    const IoVariableDetailTexts &texts) {
  if (ioVariableTableRowHasStartupDiff(row)) {
    return texts.startupMismatch;
  }
  if (ioVariableTableRowHasPdoMapIssue(row)) {
    return texts.mapIssue;
  }
  if (ioVariableTableRowHasPlcIssue(row, texts.readyText)) {
    return texts.plcReview;
  }
  if (ioVariableTableRowHasMissingValue(row)) {
    return texts.missingValue;
  }
  if (ioVariableTableRowHasChangedValue(row)) {
    return texts.changed;
  }
  return texts.readyEvidence;
}

// Assembles the full detail panel state: summary text, severity, signal state, and tooltip.
IoVariableDetailState
buildIoVariableDetailState(const IoVariableTableRow &row,
                             const IoVariableDetailTexts &texts) {
  IoVariableDetailState state;
  const QString slave = ioVariableDetailSlaveText(row);
  state.severityKey = ioVariableDetailSeverityKey(row, texts.readyText);
  state.signalState = ioVariableDetailSignalState(row, texts);
  state.text =
      texts.summaryPattern.arg(slave.isEmpty() ? QStringLiteral("?") : slave)
          .arg(row.index.isEmpty() ? QStringLiteral("----") : row.index)
          .arg(row.subIndex.isEmpty() ? QStringLiteral("--") : row.subIndex)
          .arg(row.direction.isEmpty() ? texts.directionFallback
                                       : row.direction)
          .arg(ioVariableDetailDisplayName(row, texts))
          .arg(ioVariableDetailDisplayValue(row, texts))
          .arg(row.startup.isEmpty() ? texts.noComparison : row.startup)
          .arg(row.map.isEmpty() ? texts.noMapEvidence : row.map)
          .arg(row.plcQuality.isEmpty() ? texts.notReviewed : row.plcQuality);

  state.tooltipLines << texts.selectedTitle;
  state.tooltipLines << QString("%1: #%2").arg(texts.slaveLabel, slave);
  state.tooltipLines << QString("%1: %2").arg(texts.directionLabel,
                                              row.direction);
  state.tooltipLines << QString("%1: %2").arg(texts.symbolLabel, row.symbol);
  state.tooltipLines << QString("%1: %2").arg(texts.aliasLabel, row.alias);
  state.tooltipLines
      << QString("%1: %2:%3").arg(texts.objectLabel, row.index, row.subIndex);
  state.tooltipLines << QString("%1: %2").arg(texts.bitsLabel, row.bits);
  state.tooltipLines << QString("%1: %2").arg(texts.pdoLabel, row.pdo);
  state.tooltipLines << QString("%1: %2").arg(texts.sourceLabel, row.source);
  state.tooltipLines << QString("%1: %2").arg(texts.rawLabel, row.raw);
  state.tooltipLines << QString("%1: %2").arg(texts.decodedLabel, row.decoded);
  state.tooltipLines << QString("%1: %2").arg(texts.meaningLabel, row.meaning);
  state.tooltipLines << QString("%1: %2").arg(texts.watchLabel, row.watch);
  state.tooltipLines << QString("%1: %2").arg(texts.startupLabel, row.startup);
  state.tooltipLines << QString("%1: %2").arg(texts.mapLabel, row.map);
  state.tooltipLines << QString("%1: %2").arg(texts.changedLabel, row.changed);
  state.tooltipLines << QString("%1: %2").arg(texts.plcLabel, row.plcQuality);
  state.tooltipLines << QString("%1: %2").arg(texts.tagsLabel, row.tags);
  state.tooltipLines << QString("%1: %2").arg(texts.noteLabel, row.note);
  state.tooltipLines << QString("%1: %2").arg(texts.signalStateLabel,
                                              state.signalState);
  state.tooltipLines << texts.localBoundary;
  state.tooltipLines << texts.executionBoundary;
  state.tooltip = state.tooltipLines.join('\n');
  return state;
}
