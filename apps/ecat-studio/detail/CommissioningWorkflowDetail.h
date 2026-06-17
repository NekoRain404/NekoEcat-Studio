#pragma once

// Summary and scope-filter state for the commissioning workflow panel.


#include "models/CommissioningWorkflowModel.h"

#include <QList>
#include <QString>
#include <QStringList>
#include <QVector>

// All localized strings for the commissioning workflow panel.
struct CommissioningWorkflowTexts {
  QString ready;
  QString action;
  QString blocked;
  QString onlineRuntimeAction;
  QString onlineTopologyAction;
  QString localTargetSelection;
  QString onlineOdRead;
  QString localEvidenceReview;
  QString onlinePdoRead;
  QString localWatchEdit;
  QString localStartupReview;
  QString consistencyGate;
  QString processDataAction;
  QString connectRuntimeBoundary;
  QString scanTopologyBoundary;
  QString selectSlaveBoundary;
  QString inspectObjectDictionaryBoundary;
  QString reviewObjectDictionaryEvidenceBoundary;
  QString reviewPdoMapBoundary;
  QString monitorWatchBoundary;
  QString reviewStartupDiffsBoundary;
  QString runConsistencyGateBoundary;
  QString validateProcessImageBoundary;
  QString phaseHeader;
  QString statusHeader;
  QString stepHeader;
  QString riskHeader;
  QString evidenceHeader;
  QString nextActionHeader;
  QString tooltipPattern;
};

// Domain model row with status, risk, evidence, and action.
struct CommissioningWorkflowRow {
  QString phase;
  CommissioningWorkflowStatus status = CommissioningWorkflowStatus::Blocked;
  QString step;
  QString risk;
  QString evidence;
  QString action;
};

// UI-ready row with cell strings, tooltip, and color key.
struct CommissioningWorkflowUiRow {
  QStringList cells;
  QString tooltip;
  QString colorKey;
  CommissioningWorkflowStatus status = CommissioningWorkflowStatus::Blocked;
};

// Tallied counts of ready, action, and blocked rows.
struct CommissioningWorkflowStats {
  int ready = 0;
  int action = 0;
  int blocked = 0;
};

// Boundary classification for a workflow step (online/local + detail).
struct CommissioningWorkflowStepBoundary {
  QString kind;
  QString detail;
};

QString
commissioningWorkflowStatusText(CommissioningWorkflowStatus status,
                                const CommissioningWorkflowTexts &texts);
QString commissioningWorkflowColorKey(CommissioningWorkflowStatus status);
CommissioningWorkflowStepBoundary
commissioningWorkflowStepBoundary(CommissioningWorkflowStep step,
                                  const CommissioningWorkflowTexts &texts);
QStringList
commissioningWorkflowHeaders(const CommissioningWorkflowTexts &texts);
CommissioningWorkflowUiRow
commissioningWorkflowUiRow(const CommissioningWorkflowRow &row,
                           const CommissioningWorkflowTexts &texts);
QList<QStringList>
commissioningWorkflowTableRows(const QVector<CommissioningWorkflowUiRow> &rows);
CommissioningWorkflowStats
commissioningWorkflowStats(const QVector<CommissioningWorkflowUiRow> &rows);
