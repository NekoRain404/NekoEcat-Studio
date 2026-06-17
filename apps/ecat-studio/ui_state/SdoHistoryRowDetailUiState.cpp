// Detail panel text for a selected SDO history row.
#include "SdoHistoryRowDetailUiState.h"

namespace {

// Detects "failed" status using English and Chinese keywords.
bool historyFailed(const QString &status) {
  return status.toLower().contains(QStringLiteral("failed")) ||
         status.contains(QStringLiteral("失败"));
}

// Detects "requested" (pending) status.
bool historyRequested(const QString &status) {
  return status.toLower().contains(QStringLiteral("requested")) ||
         status.contains(QStringLiteral("已请求"));
}

// Detects successful completion status.
bool historyComplete(const QString &status) {
  const QString lowered = status.toLower();
  return lowered.contains(QStringLiteral("complete")) ||
         lowered.contains(QStringLiteral("ok")) ||
         status.contains(QStringLiteral("完成")) ||
         status.contains(QStringLiteral("成功"));
}

// Whether the action is a write operation.
bool historyWriteAction(const QString &action) {
  return action.toLower().contains(QStringLiteral("write")) ||
         action.contains(QStringLiteral("写入"));
}

// Whether the action is a verify operation.
bool historyVerifyAction(const QString &action) {
  return action.toLower().contains(QStringLiteral("verify")) ||
         action.contains(QStringLiteral("校验"));
}

// Whether the action is a read operation.
bool historyReadAction(const QString &action) {
  return action.toLower().contains(QStringLiteral("read")) ||
         action.contains(QStringLiteral("读取"));
}

} // namespace

// Neutral state when the SDO history table is not available.
SdoHistoryRowDetailUiState
sdoHistoryRowDetailUnavailableState(const SdoHistoryRowDetailTexts &texts) {
  return {.text = texts.unavailableText,
    // Set severityKey field
          .severityKey = QStringLiteral("neutral"),
    // Set tooltip field
          .tooltip = texts.unavailableTip};
}

// Neutral state prompting the user to select a history row.
SdoHistoryRowDetailUiState
sdoHistoryRowDetailNoSelectionState(const SdoHistoryRowDetailTexts &texts) {
  return {.text = texts.noSelectionText,
    // Set severityKey field
          .severityKey = QStringLiteral("neutral"),
    // Set tooltip field
          .tooltip = texts.noSelectionTip};
}

// Maps status and action type to a severity key for styling.
QString sdoHistoryRowDetailSeverityKey(const SdoHistoryRow &row,
                                       const SdoHistoryRowDetailTexts &) {
  const bool failed = historyFailed(row.status);
  const bool requested = historyRequested(row.status);
  const bool complete = historyComplete(row.status);
  const bool writeAction = historyWriteAction(row.action);
  const bool verifyAction = historyVerifyAction(row.action);
  const bool readAction = historyReadAction(row.action);
  const bool hasReusableValue = !row.value.isEmpty() && !failed && !requested;
  const bool hasTarget = sdoHistoryRowHasTarget(row);

  if (failed) {
    return QStringLiteral("error");
  }
  if (!hasTarget || requested) {
    return QStringLiteral("warning");
  }
  if (writeAction || verifyAction) {
    return QStringLiteral("action");
  }
  if (complete || readAction || hasReusableValue) {
    return QStringLiteral("ok");
  }
  return QStringLiteral("neutral");
}

// Assembles the full history detail state: severity, reuse guidance, summary, and tooltip.
SdoHistoryRowDetailUiState
buildSdoHistoryRowDetailUiState(const SdoHistoryRow &row,
                                const SdoHistoryRowDetailTexts &texts) {
  SdoHistoryRowDetailUiState state;
  state.failed = historyFailed(row.status);
  state.requested = historyRequested(row.status);
  state.complete = historyComplete(row.status);
  state.writeAction = historyWriteAction(row.action);
  state.verifyAction = historyVerifyAction(row.action);
  state.readAction = historyReadAction(row.action);
  state.hasReusableValue =
      !row.value.isEmpty() && !state.failed && !state.requested;
  state.hasTarget = sdoHistoryRowHasTarget(row);
  state.severityKey = sdoHistoryRowDetailSeverityKey(row, texts);

  state.reuse = texts.fillTargetOnly;
  if (state.hasReusableValue) {
    state.reuse = texts.reusableValue;
  } else if (state.failed) {
    state.reuse = texts.reviewFailure;
  } else if (state.requested) {
    state.reuse = texts.waitingRuntime;
  }

  state.text =
      texts.summaryPattern
          .arg(row.time.isEmpty() ? texts.timeFallback : row.time)
          .arg(row.action.isEmpty() ? texts.actionFallback : row.action)
          .arg(row.positionText.isEmpty() ? QStringLiteral("?")
                                          : row.positionText)
          .arg(row.index.isEmpty() ? QStringLiteral("----") : row.index)
          .arg(row.subIndex.isEmpty() ? QStringLiteral("--") : row.subIndex)
          .arg(row.type.isEmpty() ? texts.typeFallback : row.type)
          .arg(row.value.isEmpty() ? texts.noValue : row.value)
          .arg(row.status.isEmpty() ? texts.noStatus : row.status)
          .arg(state.reuse);

  state.tooltipLines << texts.selectedTitle;
  state.tooltipLines << QString("%1: %2").arg(texts.timeLabel, row.time);
  state.tooltipLines << QString("%1: %2").arg(texts.actionLabel, row.action);
  state.tooltipLines << QString("%1: #%2").arg(texts.slaveLabel,
                                               row.positionText);
  state.tooltipLines
      << QString("%1: %2:%3").arg(texts.objectLabel, row.index, row.subIndex);
  state.tooltipLines << QString("%1: %2").arg(texts.typeLabel, row.type);
  state.tooltipLines << QString("%1: %2").arg(texts.valueLabel, row.value);
  state.tooltipLines << QString("%1: %2").arg(texts.statusLabel, row.status);
  state.tooltipLines << QString("%1: %2").arg(texts.detailLabel, row.detail);
  state.tooltipLines << QString("%1: %2").arg(texts.reuseLabel, state.reuse);
  state.tooltipLines << texts.localBoundary;
  state.tooltipLines << texts.executionBoundary;
  state.tooltip = state.tooltipLines.join('\n');
  return state;
}
