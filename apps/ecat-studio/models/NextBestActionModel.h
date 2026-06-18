#pragma once

// Next-best-action recommendation engine for the commissioning overview.


#include "CommissioningWorkflowModel.h"

#include <QString>

// Type of recommended action for the user
enum class NextBestActionKind {
  Connect,
  Diagnostics,
  Rescan,
  SelectSlave,
  LoadOd,
  FailedOdEvidence,
  LoadPdo,
  AddWatch,
  StartupDiffs,
  ConsistencyEvidenceIssue,
  Consistency,
  FreeRun,
  MatrixReview,
  CommandPalette,
};

// Visual severity level for the action card
enum class NextBestActionSeverity {
  Ok,
  Action,
  Warning,
  Error,
  Neutral,
};

// Aggregated state from all workspaces for action selection
struct NextBestActionInput {
  CommissioningWorkflowInput workflow;
  bool hasDiagnosticError = false;
  int consistencyBlockingIssueRow = -1;
  int matrixP0 = 0;
  int matrixP1 = 0;
  int matrixP2 = 0;
};

// Selected action and its severity for display
struct NextBestActionDecision {
  NextBestActionKind kind = NextBestActionKind::CommandPalette;
  NextBestActionSeverity severity = NextBestActionSeverity::Neutral;
};

NextBestActionDecision chooseNextBestAction(const NextBestActionInput &input);
QString nextBestActionKey(NextBestActionKind kind);
QString nextBestActionSeverityKey(NextBestActionSeverity severity);
