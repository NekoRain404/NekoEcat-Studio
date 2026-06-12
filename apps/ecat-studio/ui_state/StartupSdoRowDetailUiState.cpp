#include "StartupSdoRowDetailUiState.h"

#include <initializer_list>

namespace {

bool containsAny(const QString &text,
                 std::initializer_list<const char *> keys) {
  const QString lowered = text.toLower();
  for (const char *key : keys) {
    if (lowered.contains(QLatin1String(key))) {
      return true;
    }
  }
  return false;
}

bool statusHasValidationIssue(const QString &status) {
  return containsAny(status, {"error", "failed"}) ||
         status.contains(QStringLiteral("错误")) ||
         status.contains(QStringLiteral("失败"));
}

bool statusIsApplying(const QString &status) {
  return containsAny(status, {"applying", "verifying"}) ||
         status.contains(QStringLiteral("应用中")) ||
         status.contains(QStringLiteral("校验中"));
}

bool deltaIsWatchDiff(const QString &delta) {
  return containsAny(delta, {"diff"}) ||
         delta.contains(QStringLiteral("不一致"));
}

bool deltaIsNoWatch(const QString &delta) {
  return containsAny(delta, {"no watch"}) ||
         delta.contains(QStringLiteral("无监视"));
}

bool deltaIsPending(const QString &delta) {
  return containsAny(delta, {"pending"}) ||
         delta.contains(QStringLiteral("待比较"));
}

bool deltaIsMatch(const QString &delta) {
  return containsAny(delta, {"match"}) ||
         delta.contains(QStringLiteral("匹配"));
}

QString startupPositionText(const WatchStartupStartupRow &row) {
  if (!row.positionText.isEmpty()) {
    return row.positionText;
  }
  return row.position >= 0 ? QString::number(row.position) : QString();
}

} // namespace

StartupSdoRowDetailUiState
startupSdoRowDetailUnavailableState(const StartupSdoRowDetailTexts &texts) {
  return {.text = texts.unavailableText,
          .severityKey = QStringLiteral("neutral"),
          .tooltip = texts.unavailableTip};
}

StartupSdoRowDetailUiState
startupSdoRowDetailNoSelectionState(const StartupSdoRowDetailTexts &texts) {
  return {.text = texts.noSelectionText,
          .severityKey = QStringLiteral("neutral"),
          .tooltip = texts.noSelectionTip};
}

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
