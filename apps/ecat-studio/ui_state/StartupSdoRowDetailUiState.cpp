// Detail panel text for a selected Startup SDO row.
#include "StartupSdoRowDetailUiState.h"

#include <initializer_list>

namespace {

// Case-insensitive check for any keyword presence in the text.
bool containsAny(const QString &text,
                 std::initializer_list<const char *> keys) {
  const QString lowered = text.toLower();
    // Iterate over collection
  for (const char *key : keys) {
    if (lowered.contains(QLatin1String(key))) {
      return true;
    }
  }
  return false;
}

// Detects error/failed status using English and Chinese keywords.
bool statusHasValidationIssue(const QString &status) {
  return containsAny(status, {"error", "failed"}) ||
         status.contains(QStringLiteral("错误")) ||
         status.contains(QStringLiteral("失败"));
}

// Detects in-progress status (applying/verifying).
bool statusIsApplying(const QString &status) {
  return containsAny(status, {"applying", "verifying"}) ||
         status.contains(QStringLiteral("应用中")) ||
         status.contains(QStringLiteral("校验中"));
}

// Whether the watch delta indicates a value mismatch.
bool deltaIsWatchDiff(const QString &delta) {
  return containsAny(delta, {"diff"}) ||
         delta.contains(QStringLiteral("不一致"));
}

// Whether the delta indicates no watch entry was found.
bool deltaIsNoWatch(const QString &delta) {
  return containsAny(delta, {"no watch"}) ||
         delta.contains(QStringLiteral("无监视"));
}

// Whether the comparison is still pending.
bool deltaIsPending(const QString &delta) {
  return containsAny(delta, {"pending"}) ||
         delta.contains(QStringLiteral("待比较"));
}

// Whether the startup value matches the watch value.
bool deltaIsMatch(const QString &delta) {
  return containsAny(delta, {"match"}) ||
         delta.contains(QStringLiteral("匹配"));
}

// Returns the position text, falling back to the integer position.
QString startupPositionText(const WatchStartupStartupRow &row) {
  if (!row.positionText.isEmpty()) {
    return row.positionText;
  }
  return row.position >= 0 ? QString::number(row.position) : QString();
}

} // namespace

// Neutral state when the startup SDO table is not available.
StartupSdoRowDetailUiState
startupSdoRowDetailUnavailableState(const StartupSdoRowDetailTexts &texts) {
  return {.text = texts.unavailableText,
    // Set severityKey field
          .severityKey = QStringLiteral("neutral"),
    // Set tooltip field
          .tooltip = texts.unavailableTip};
}

// Neutral state prompting the user to select a startup row.
StartupSdoRowDetailUiState
startupSdoRowDetailNoSelectionState(const StartupSdoRowDetailTexts &texts) {
  return {.text = texts.noSelectionText,
    // Set severityKey field
          .severityKey = QStringLiteral("neutral"),
    // Set tooltip field
          .tooltip = texts.noSelectionTip};
}

// Maps validation status, watch delta, and target completeness to a severity key.
QString startupSdoRowDetailSeverityKey(const WatchStartupStartupRow &row,
                                       const StartupSdoRowDetailTexts &) {
  const bool validationIssue = statusHasValidationIssue(row.status);
  const bool applying = statusIsApplying(row.status);
  const bool watchDiff = deltaIsWatchDiff(row.watchDelta);
  const bool noWatch = deltaIsNoWatch(row.watchDelta);
  const bool pending = deltaIsPending(row.watchDelta);
  const bool match = deltaIsMatch(row.watchDelta);
  const bool missingTarget = startupPositionText(row).isEmpty() ||
                             row.index.isEmpty() || row.subIndex.isEmpty() ||
                             row.value.isEmpty();

  if (validationIssue || watchDiff) {
    return QStringLiteral("error");
  }
  if (missingTarget || noWatch || pending) {
    return QStringLiteral("warning");
  }
  if (applying) {
    return QStringLiteral("action");
  }
  if (match) {
    return QStringLiteral("ok");
  }
  return QStringLiteral("neutral");
}

// Assembles the full startup detail state: status flags, evidence text, summary, and tooltip.
StartupSdoRowDetailUiState
buildStartupSdoRowDetailUiState(const WatchStartupStartupRow &row,
                                const StartupSdoRowDetailTexts &texts) {
  StartupSdoRowDetailUiState state;
  state.validationIssue = statusHasValidationIssue(row.status);
  state.applying = statusIsApplying(row.status);
  state.watchDiff = deltaIsWatchDiff(row.watchDelta);
  state.noWatch = deltaIsNoWatch(row.watchDelta);
  state.pending = deltaIsPending(row.watchDelta);
  state.match = deltaIsMatch(row.watchDelta);

  const QString slave = startupPositionText(row);
  state.missingTarget = slave.isEmpty() || row.index.isEmpty() ||
                        row.subIndex.isEmpty() || row.value.isEmpty();
  state.severityKey = startupSdoRowDetailSeverityKey(row, texts);

  state.evidence =
      state.watchDiff
          ? texts.watchMismatch
          : (state.noWatch ? texts.noWatchEvidence
                           : (state.pending ? texts.pendingComparison
                                            : (state.match ? texts.watchMatches
                                                           : texts.reviewRow)));

  state.text =
      texts.summaryPattern.arg(row.row + 1)
          .arg(slave.isEmpty() ? QStringLiteral("?") : slave)
          .arg(row.index.isEmpty() ? QStringLiteral("----") : row.index)
          .arg(row.subIndex.isEmpty() ? QStringLiteral("--") : row.subIndex)
          .arg(row.type.isEmpty() ? texts.defaultType : row.type)
          .arg(row.value.isEmpty() ? texts.emptyValue : row.value)
          .arg(row.status.isEmpty() ? texts.pendingStatus : row.status)
          .arg(row.watchValue.isEmpty() ? texts.noWatchValue : row.watchValue)
          .arg(state.evidence);

  state.tooltipLines << texts.selectedTitle;
  state.tooltipLines << QString("%1: %2").arg(texts.rowLabel,
                                              QString::number(row.row + 1));
  state.tooltipLines << QString("%1: #%2").arg(texts.slaveLabel, slave);
  state.tooltipLines
      << QString("%1: %2:%3").arg(texts.objectLabel, row.index, row.subIndex);
  state.tooltipLines << QString("%1: %2").arg(texts.valueLabel, row.value);
  state.tooltipLines << QString("%1: %2").arg(texts.typeLabel, row.type);
  state.tooltipLines << QString("%1: %2").arg(texts.statusLabel, row.status);
  state.tooltipLines << QString("%1: %2").arg(texts.detailLabel, row.detail);
  state.tooltipLines << QString("%1: %2").arg(texts.watchValueLabel,
                                              row.watchValue);
  state.tooltipLines << QString("%1: %2").arg(texts.watchDeltaLabel,
                                              row.watchDelta);
  state.tooltipLines << texts.localBoundary;
  state.tooltipLines << texts.executionBoundary;
  state.tooltip = state.tooltipLines.join('\n');
  return state;
}
