#pragma once

// Detail panel text for a selected commissioning workflow step.


#include "adapters/CommissioningWorkflowTableAdapter.h"
#include "CommissioningWorkflowUiState.h"

#include <QString>
#include <QStringList>

// Localized text templates for the step detail panel.
struct CommissioningWorkflowStepDetailTexts {
  QString unavailableText;
  QString unavailableTip;
  QString noSelectionText;
  QString noSelectionTip;
  QString none;
  QString noRisk;
  QString summaryPattern;
  QString selectedTitle;
  QString phaseLabel;
  QString statusLabel;
  QString stepLabel;
  QString riskLabel;
  QString evidenceLabel;
  QString nextActionLabel;
  QString boundaryLabel;
  QString localReviewBoundary;
};

// Resolved detail panel state with severity, boundary, and tooltip.
struct CommissioningWorkflowStepDetailUiState {
  QString text;
  QString severityKey;
  QString boundaryKind;
  QString boundaryDetail;
  QString displayRisk;
  bool ready = false;
  bool action = false;
  bool blocked = false;
  bool hasRisk = false;
  bool severeRisk = false;
  QStringList tooltipLines;
  QString tooltip;
};

// Neutral state when the workflow table is unavailable.
CommissioningWorkflowStepDetailUiState
commissioningWorkflowStepDetailUnavailableState(
    const CommissioningWorkflowStepDetailTexts &texts);
// Neutral state prompting user to select a row.
CommissioningWorkflowStepDetailUiState
commissioningWorkflowStepDetailNoSelectionState(
    const CommissioningWorkflowStepDetailTexts &texts);
// Maps status + risk to a severity key for styling.
QString commissioningWorkflowStepDetailSeverityKey(
    const CommissioningWorkflowTableRow &row,
    const CommissioningWorkflowTexts &workflowTexts,
    const CommissioningWorkflowStepDetailTexts &texts);
// Assembles the full detail panel state.
CommissioningWorkflowStepDetailUiState
buildCommissioningWorkflowStepDetailUiState(
    const CommissioningWorkflowTableRow &row,
    const CommissioningWorkflowTexts &workflowTexts,
    const CommissioningWorkflowStepDetailTexts &texts);
