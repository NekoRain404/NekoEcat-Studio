#include "IoVariableDetailUiState.h"

#include "ProcessDataRowModel.h"

namespace {

QString ioVariableDetailSlaveText(const IoVariableTableRow &row) {
  return row.positionValid ? QString::number(row.position) : QString();
}

QString ioVariableDetailDisplayName(const IoVariableTableRow &row,
                                    const IoVariableDetailTexts &texts) {
  const QString name = row.alias.isEmpty() ? row.symbol : row.alias;
  return name.isEmpty() ? texts.unnamedSignal : name;
}

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

IoVariableDetailUiState
ioVariableDetailUnavailableState(const IoVariableDetailTexts &texts) {
  return {.text = texts.unavailableText,
          .severityKey = QStringLiteral("neutral"),
          .tooltip = texts.unavailableTip};
}

IoVariableDetailUiState
ioVariableDetailNoSelectionState(const IoVariableDetailTexts &texts) {
  return {.text = texts.noSelectionText,
          .severityKey = QStringLiteral("neutral"),
          .tooltip = texts.noSelectionTip};
}

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

IoVariableDetailUiState
buildIoVariableDetailUiState(const IoVariableTableRow &row,
                             const IoVariableDetailTexts &texts) {
  IoVariableDetailUiState state;
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
