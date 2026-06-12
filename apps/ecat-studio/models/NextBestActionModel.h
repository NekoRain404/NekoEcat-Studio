#pragma once

// Next-best-action recommendation engine for the commissioning overview.


#include "CommissioningWorkflowModel.h"

#include <QString>

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

enum class NextBestActionSeverity {
  Ok,
  Action,
  Warning,
  Error,
  Neutral,
};

struct NextBestActionInput {
  CommissioningWorkflowInput workflow;
  bool hasDiagnosticError = false;
  int consistencyBlockingIssueRow = -1;
  int matrixP0 = 0;
  int matrixP1 = 0;
  int matrixP2 = 0;
};

struct NextBestActionDecision {
  NextBestActionKind kind = NextBestActionKind::CommandPalette;
  NextBestActionSeverity severity = NextBestActionSeverity::Neutral;
};

NextBestActionDecision chooseNextBestAction(const NextBestActionInput &input);
QString nextBestActionKey(NextBestActionKind kind);
QString nextBestActionSeverityKey(NextBestActionSeverity severity);
