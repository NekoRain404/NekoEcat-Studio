#pragma once

// Workspace tab badge text templates and severity color keys.


#include <QString>

// Localized tab names and tooltip patterns for all 8 workspace tabs.
struct WorkspaceTabBadgeTexts {
  QString overview;
  QString watch;
  QString startupSdo;
  QString freeRun;
  QString ioVariables;
  QString consistency;
  QString stateMachine;
  QString diagnostics;
  QString overviewTipPattern;
  QString watchTipPattern;
  QString startupSdoTipPattern;
  QString freeRunTipPattern;
  QString ioVariablesTipPattern;
  QString consistencyTipPattern;
  QString stateMachineTipPattern;
  QString diagnosticsTipPattern;
};

// Aggregated counts from all workspace tables for badge computation.
struct WorkspaceTabBadgeCounts {
  int matrixP0 = 0;
  int matrixP1 = 0;
  int matrixP2 = 0;
  int matrixP3 = 0;
  int watchRows = 0;
  int watchStartupDiffs = 0;
  int startupRows = 0;
  int startupDiffs = 0;
  int freeRunRows = 0;
  int ioRows = 0;
  int ioIssues = 0;
  int consistencyRows = 0;
  int consistencyErrors = 0;
  int consistencyWarnings = 0;
  int consistencyInfos = 0;
  int consistencyReady = 0;
  int stateRiskRows = 0;
  int diagnosticRows = 0;
  int diagnosticErrors = 0;
  int diagnosticWarnings = 0;
  int diagnosticInfos = 0;
};

// A single tab badge: formatted text and detailed tooltip.
struct WorkspaceTabBadge {
  QString text;
  QString tip;
};

// All 8 workspace tab badges ready for rendering.
struct WorkspaceTabBadgeUiState {
  WorkspaceTabBadge overview;
  WorkspaceTabBadge watch;
  WorkspaceTabBadge startupSdo;
  WorkspaceTabBadge freeRun;
  WorkspaceTabBadge ioVariables;
  WorkspaceTabBadge consistency;
  WorkspaceTabBadge stateMachine;
  WorkspaceTabBadge diagnostics;
};

// Formats badge text with optional issue indicator.
QString workspaceTabBadgeText(const QString &label, int count,
                              bool issue = false);
// Builds all 8 workspace tab badges.
WorkspaceTabBadgeUiState
buildWorkspaceTabBadgeUiState(const WorkspaceTabBadgeCounts &counts,
                              const WorkspaceTabBadgeTexts &texts);
