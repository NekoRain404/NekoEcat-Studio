// Detail panel text for a selected state machine recommendation row.
#include "StateMachineRowDetailUiState.h"

namespace {

// Detects high-severity risk keywords (fault/error/offline) in English and Chinese.
bool stateMachineSevereRiskText(const QString &risk) {
  const QString lowered = risk.toLower();
  return lowered.contains(QStringLiteral("fault")) ||
         lowered.contains(QStringLiteral("error")) ||
         lowered.contains(QStringLiteral("offline")) ||
         risk.contains(QStringLiteral("故障")) ||
         risk.contains(QStringLiteral("错误")) ||
         risk.contains(QStringLiteral("离线"));
}

// Uppercases the recommended state for boundary classification.
QString stateMachineRecommendedUpper(const StateMachineTableRow &row) {
  return row.recommended.toUpper();
}

} // namespace

// Neutral state when the state machine table is not available.
StateMachineRowDetailUiState
stateMachineRowDetailUnavailableState(const StateMachineRowDetailTexts &texts) {
  return {.text = texts.unavailableText,
          .severityKey = QStringLiteral("neutral"),
          .tooltip = texts.unavailableTip};
}

// Neutral state prompting the user to select a row.
StateMachineRowDetailUiState
stateMachineRowDetailNoSelectionState(const StateMachineRowDetailTexts &texts) {
  return {.text = texts.noSelectionText,
          .severityKey = QStringLiteral("neutral"),
          .tooltip = texts.noSelectionTip};
}

// Maps risk severity, recommendation, and current state to a severity key.
QString stateMachineRowDetailSeverityKey(const StateMachineTableRow &row,
                                         const StateMachineRowDetailTexts &) {
  const bool hasRecommendation = !row.recommended.isEmpty();
  const bool hasRisk = !row.risk.isEmpty();
  const QString currentUpper = row.current.toUpper();
  const QString recommendedUpper = stateMachineRecommendedUpper(row);

  if (hasRisk && stateMachineSevereRiskText(row.risk)) {
    return QStringLiteral("error");
  }
  if (hasRisk || recommendedUpper == QStringLiteral("OP")) {
    return QStringLiteral("warning");
  }
  if (hasRecommendation) {
    return QStringLiteral("action");
  }
  if (currentUpper == QStringLiteral("OP") ||
      currentUpper.startsWith(QStringLiteral("OP "))) {
    return QStringLiteral("ok");
  }
  return QStringLiteral("neutral");
}

// Assembles the full state machine detail: boundary classification, display fields, and tooltip.
StateMachineRowDetailUiState
buildStateMachineRowDetailUiState(const StateMachineTableRow &row,
                                  const StateMachineRowDetailTexts &texts) {
  StateMachineRowDetailUiState state;
  state.hasRecommendation = !row.recommended.isEmpty();
  state.hasRisk = !row.risk.isEmpty();
  state.severeRisk = state.hasRisk && stateMachineSevereRiskText(row.risk);
  state.severityKey = stateMachineRowDetailSeverityKey(row, texts);
  state.displayPosition =
      row.position.isEmpty() ? QString::number(row.row) : row.position;
  state.displayName = row.name.isEmpty() ? texts.unnamed : row.name;
  state.displayCurrent = row.current.isEmpty() ? texts.unknown : row.current;
  state.displayRecommended =
      row.recommended.isEmpty() ? texts.none : row.recommended;
  state.displayRisk = row.risk.isEmpty() ? texts.noRisk : row.risk;
  state.displayAction =
      row.action.isEmpty() ? texts.reviewEvidence : row.action;

  const QString recommendedUpper = stateMachineRecommendedUpper(row);
  state.boundary = state.hasRecommendation ? texts.confirmedRequestBoundary
                                           : texts.localReviewBoundaryLabel;
  if (recommendedUpper == QStringLiteral("OP")) {
    state.boundary = texts.opBoundary;
  } else if (recommendedUpper == QStringLiteral("SAFEOP")) {
    state.boundary = texts.safeopBoundary;
  } else if (recommendedUpper == QStringLiteral("PREOP")) {
    state.boundary = texts.preopBoundary;
  }

  state.text = texts.summaryPattern.arg(state.displayPosition)
                   .arg(state.displayName)
                   .arg(state.displayCurrent)
                   .arg(state.displayRecommended)
                   .arg(state.boundary)
                   .arg(state.displayRisk)
                   .arg(state.displayAction);

  state.tooltipLines << texts.selectedTitle;
  state.tooltipLines << QString("%1: #%2 %3")
                            .arg(texts.slaveLabel, state.displayPosition,
                                 state.displayName);
  state.tooltipLines << QString("%1: %2").arg(texts.currentStateLabel,
                                              state.displayCurrent);
  state.tooltipLines << QString("%1: %2").arg(texts.recommendedStateLabel,
                                              state.displayRecommended);
  state.tooltipLines << QString("%1: %2").arg(texts.evidenceLabel,
                                              row.evidence);
  state.tooltipLines << QString("%1: %2").arg(texts.driveLabel, row.drive);
  state.tooltipLines << QString("%1: %2").arg(texts.startupLabel, row.startup);
  state.tooltipLines << QString("%1: %2").arg(texts.processLabel, row.process);
  state.tooltipLines << QString("%1: %2").arg(texts.riskLabel,
                                              state.displayRisk);
  state.tooltipLines << QString("%1: %2").arg(texts.boundaryLabel,
                                              state.boundary);
  state.tooltipLines << texts.localBoundary;
  state.tooltipLines << texts.executionBoundary;
  state.tooltip = state.tooltipLines.join('\n');
  return state;
}
