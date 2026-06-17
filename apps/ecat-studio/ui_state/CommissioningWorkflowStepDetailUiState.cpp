// Detail panel text for a selected commissioning workflow step.
#include "CommissioningWorkflowStepDetailUiState.h"

namespace {

// Checks if text represents an absent/none value using normalized comparison.
bool commissioningWorkflowNoneText(
    const QString &text, const CommissioningWorkflowStepDetailTexts &texts) {
  const QString normalized = text.trimmed().toLower();
  return normalized.isEmpty() || normalized == QStringLiteral("none") ||
         normalized == texts.none.trimmed().toLower();
}

// Whether the row status indicates completion (key-based or localized text match).
bool commissioningWorkflowReadyStatus(const CommissioningWorkflowTableRow &row,
                                      const CommissioningWorkflowTexts &texts) {
  return row.statusKey == QStringLiteral("ready") ||
         row.status.trimmed() == texts.ready;
}

// Whether the row status indicates an action is required.
bool commissioningWorkflowActionStatus(
    const CommissioningWorkflowTableRow &row,
    const CommissioningWorkflowTexts &texts) {
  return row.statusKey == QStringLiteral("action") ||
         row.status.trimmed() == texts.action;
}

// Whether the row status indicates a blocked/waiting state.
bool commissioningWorkflowBlockedStatus(
    const CommissioningWorkflowTableRow &row,
    const CommissioningWorkflowTexts &texts) {
  return row.statusKey == QStringLiteral("blocked") ||
         row.status.trimmed() == texts.blocked;
}

// Detects high-severity risk keywords (failed/error) in both English and Chinese.
bool commissioningWorkflowSevereRiskText(const QString &risk) {
  return risk.contains(QStringLiteral("failed"), Qt::CaseInsensitive) ||
         risk.contains(QStringLiteral("error"), Qt::CaseInsensitive) ||
         risk.contains(QStringLiteral("失败")) ||
         risk.contains(QStringLiteral("错误"));
}

} // namespace

// Returns a neutral state when the commissioning workflow table is not available.
CommissioningWorkflowStepDetailUiState
commissioningWorkflowStepDetailUnavailableState(
    const CommissioningWorkflowStepDetailTexts &texts) {
  return {.text = texts.unavailableText,
    // Set severityKey field
          .severityKey = QStringLiteral("neutral"),
    // Set tooltip field
          .tooltip = texts.unavailableTip};
}

// Returns a neutral state prompting the user to select a row.
CommissioningWorkflowStepDetailUiState
commissioningWorkflowStepDetailNoSelectionState(
    const CommissioningWorkflowStepDetailTexts &texts) {
  return {.text = texts.noSelectionText,
    // Set severityKey field
          .severityKey = QStringLiteral("neutral"),
    // Set tooltip field
          .tooltip = texts.noSelectionTip};
}

// Maps the row's status and risk level to a severity key for styling (error, ok, action, warning).
QString commissioningWorkflowStepDetailSeverityKey(
    const CommissioningWorkflowTableRow &row,
    const CommissioningWorkflowTexts &workflowTexts,
    const CommissioningWorkflowStepDetailTexts &texts) {
  const bool ready = commissioningWorkflowReadyStatus(row, workflowTexts);
  const bool action = commissioningWorkflowActionStatus(row, workflowTexts);
  const bool blocked = commissioningWorkflowBlockedStatus(row, workflowTexts);
  const bool hasRisk = !commissioningWorkflowNoneText(row.risk, texts);
  const bool severeRisk =
      hasRisk && commissioningWorkflowSevereRiskText(row.risk);

  if (severeRisk) {
    return QStringLiteral("error");
  }
  if (ready) {
    return QStringLiteral("ok");
  }
  if (action) {
    return QStringLiteral("action");
  }
  if (blocked) {
    return QStringLiteral("warning");
  }
  return QStringLiteral("neutral");
}

// Assembles the full detail panel state: summary text, severity, boundary info, and tooltip.
CommissioningWorkflowStepDetailUiState
buildCommissioningWorkflowStepDetailUiState(
    const CommissioningWorkflowTableRow &row,
    const CommissioningWorkflowTexts &workflowTexts,
    const CommissioningWorkflowStepDetailTexts &texts) {
  CommissioningWorkflowStepDetailUiState state;
  state.ready = commissioningWorkflowReadyStatus(row, workflowTexts);
  state.action = commissioningWorkflowActionStatus(row, workflowTexts);
  state.blocked = commissioningWorkflowBlockedStatus(row, workflowTexts);
  state.hasRisk = !commissioningWorkflowNoneText(row.risk, texts);
  state.severeRisk =
      state.hasRisk && commissioningWorkflowSevereRiskText(row.risk);
  state.displayRisk = state.hasRisk ? row.risk : texts.noRisk;
  state.severityKey =
      commissioningWorkflowStepDetailSeverityKey(row, workflowTexts, texts);

  const CommissioningWorkflowStepBoundary boundary =
      commissioningWorkflowStepBoundary(
          commissioningWorkflowStepForIndex(row.row), workflowTexts);
  state.boundaryKind = boundary.kind;
  state.boundaryDetail = boundary.detail;

  state.text = texts.summaryPattern.arg(row.row + 1)
                   .arg(row.phase)
                   .arg(row.status)
                   .arg(state.boundaryKind)
                   .arg(state.displayRisk)
                   .arg(row.evidence)
                   .arg(row.nextAction);

  state.tooltipLines << texts.selectedTitle;
  state.tooltipLines << QString("%1: %2").arg(texts.phaseLabel, row.phase);
  state.tooltipLines << QString("%1: %2").arg(texts.statusLabel, row.status);
  state.tooltipLines << QString("%1: %2").arg(texts.stepLabel, row.step);
  state.tooltipLines << QString("%1: %2").arg(texts.riskLabel,
                                              state.displayRisk);
  state.tooltipLines << QString("%1: %2").arg(texts.evidenceLabel,
                                              row.evidence);
  state.tooltipLines << QString("%1: %2").arg(texts.nextActionLabel,
                                              row.nextAction);
  state.tooltipLines << QString("%1: %2").arg(texts.boundaryLabel,
                                              state.boundaryKind);
  state.tooltipLines << state.boundaryDetail;
  state.tooltipLines << texts.localReviewBoundary;
  state.tooltip = state.tooltipLines.join('\n');
  return state;
}
