#include "CommissioningWorkflowUiState.h"

QString
commissioningWorkflowStatusText(CommissioningWorkflowStatus status,
                                const CommissioningWorkflowTexts &texts) {
  switch (status) {
  case CommissioningWorkflowStatus::Ready:
    return texts.ready;
  case CommissioningWorkflowStatus::Action:
    return texts.action;
  case CommissioningWorkflowStatus::Blocked:
    return texts.blocked;
  }
  return texts.blocked;
}

QString commissioningWorkflowColorKey(CommissioningWorkflowStatus status) {
  switch (status) {
  case CommissioningWorkflowStatus::Ready:
    return QStringLiteral("ready");
  case CommissioningWorkflowStatus::Action:
    return QStringLiteral("action");
  case CommissioningWorkflowStatus::Blocked:
    return QStringLiteral("blocked");
  }
  return QStringLiteral("blocked");
}

CommissioningWorkflowStepBoundary
commissioningWorkflowStepBoundary(CommissioningWorkflowStep step,
                                  const CommissioningWorkflowTexts &texts) {
  switch (step) {
  case CommissioningWorkflowStep::ConnectRuntime:
    return {texts.onlineRuntimeAction, texts.connectRuntimeBoundary};
  case CommissioningWorkflowStep::ScanTopology:
    return {texts.onlineTopologyAction, texts.scanTopologyBoundary};
  case CommissioningWorkflowStep::SelectSlave:
    return {texts.localTargetSelection, texts.selectSlaveBoundary};
  case CommissioningWorkflowStep::InspectObjectDictionary:
    return {texts.onlineOdRead, texts.inspectObjectDictionaryBoundary};
  case CommissioningWorkflowStep::ReviewObjectDictionaryEvidence:
    return {texts.localEvidenceReview,
            texts.reviewObjectDictionaryEvidenceBoundary};
  case CommissioningWorkflowStep::ReviewPdoMap:
    return {texts.onlinePdoRead, texts.reviewPdoMapBoundary};
  case CommissioningWorkflowStep::MonitorWatch:
    return {texts.localWatchEdit, texts.monitorWatchBoundary};
  case CommissioningWorkflowStep::ReviewStartupDiffs:
    return {texts.localStartupReview, texts.reviewStartupDiffsBoundary};
  case CommissioningWorkflowStep::RunConsistencyGate:
    return {texts.consistencyGate, texts.runConsistencyGateBoundary};
  case CommissioningWorkflowStep::ValidateProcessImage:
    return {texts.processDataAction, texts.validateProcessImageBoundary};
  }
  return {texts.localEvidenceReview,
          texts.reviewObjectDictionaryEvidenceBoundary};
}

QStringList
commissioningWorkflowHeaders(const CommissioningWorkflowTexts &texts) {
  return {texts.phaseHeader, texts.statusHeader,   texts.stepHeader,
          texts.riskHeader,  texts.evidenceHeader, texts.nextActionHeader};
}

CommissioningWorkflowUiRow
commissioningWorkflowUiRow(const CommissioningWorkflowRow &row,
                           const CommissioningWorkflowTexts &texts) {
  CommissioningWorkflowUiRow uiRow;
  const QString status = commissioningWorkflowStatusText(row.status, texts);
  uiRow.cells = {row.phase, status,       row.step,
                 row.risk,  row.evidence, row.action};
  uiRow.tooltip = texts.tooltipPattern.arg(row.phase, status, row.risk,
                                           row.evidence, row.action);
  uiRow.colorKey = commissioningWorkflowColorKey(row.status);
  uiRow.status = row.status;
  return uiRow;
}

QList<QStringList> commissioningWorkflowTableRows(
    const QVector<CommissioningWorkflowUiRow> &rows) {
  QList<QStringList> tableRows;
  tableRows.reserve(rows.size());
  for (const auto &row : rows) {
    tableRows.append(row.cells);
  }
  return tableRows;
}

CommissioningWorkflowStats
commissioningWorkflowStats(const QVector<CommissioningWorkflowUiRow> &rows) {
  CommissioningWorkflowStats stats;
  for (const auto &row : rows) {
    switch (row.status) {
    case CommissioningWorkflowStatus::Ready:
      ++stats.ready;
      break;
    case CommissioningWorkflowStatus::Action:
      ++stats.action;
      break;
    case CommissioningWorkflowStatus::Blocked:
      ++stats.blocked;
      break;
    }
  }
  return stats;
}
