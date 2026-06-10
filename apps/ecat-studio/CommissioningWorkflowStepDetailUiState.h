#pragma once

#include "CommissioningWorkflowTableAdapter.h"
#include "CommissioningWorkflowUiState.h"

#include <QString>
#include <QStringList>

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

CommissioningWorkflowStepDetailUiState
commissioningWorkflowStepDetailUnavailableState(
    const CommissioningWorkflowStepDetailTexts &texts);
CommissioningWorkflowStepDetailUiState
commissioningWorkflowStepDetailNoSelectionState(
    const CommissioningWorkflowStepDetailTexts &texts);
QString commissioningWorkflowStepDetailSeverityKey(
    const CommissioningWorkflowTableRow &row,
    const CommissioningWorkflowTexts &workflowTexts,
    const CommissioningWorkflowStepDetailTexts &texts);
CommissioningWorkflowStepDetailUiState
buildCommissioningWorkflowStepDetailUiState(
    const CommissioningWorkflowTableRow &row,
    const CommissioningWorkflowTexts &workflowTexts,
    const CommissioningWorkflowStepDetailTexts &texts);
