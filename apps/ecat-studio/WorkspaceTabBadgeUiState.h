#pragma once

#include <QString>

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

struct WorkspaceTabBadge {
  QString text;
  QString tip;
};

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

QString workspaceTabBadgeText(const QString &label, int count,
                              bool issue = false);
WorkspaceTabBadgeUiState
buildWorkspaceTabBadgeUiState(const WorkspaceTabBadgeCounts &counts,
                              const WorkspaceTabBadgeTexts &texts);
