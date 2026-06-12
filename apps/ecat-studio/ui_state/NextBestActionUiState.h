#pragma once

// Rendered next-best-action card: title, body, icon, and safety level.


#include "models/NextBestActionModel.h"

#include <QString>

enum class NextBestActionIconKey {
  DriveNet,
  Warning,
  DetailedView,
  ListView,
  NewFolder,
  MediaPlay,
  ContentsView,
};

struct NextBestActionTexts {
  QString commands;
  QString commandPaletteTip;
  QString nextConnect;
  QString connectTip;
  QString reviewDiagnostics;
  QString diagnosticsTip;
  QString nextRescan;
  QString rescanTip;
  QString nextSelectSlave;
  QString selectSlaveTip;
  QString nextLoadOd;
  QString loadOdTip;
  QString reviewOdEvidence;
  QString failedOdEvidenceTip;
  QString nextLoadPdo;
  QString loadPdoTip;
  QString nextAddWatch;
  QString addWatchTip;
  QString reviewStartupDiffs;
  QString startupDiffsTip;
  QString reviewEvidence;
  QString consistencyEvidenceTip;
  QString reviewConsistency;
  QString reviewConsistencyTip;
  QString runConsistency;
  QString runConsistencyTip;
  QString nextFreeRun;
  QString freeRunTip;
  QString reviewMatrixRisk;
  QString reviewMatrixAction;
  QString matrixTipPattern;
};

struct NextBestActionUiState {
  QString actionKey;
  QString severityKey;
  QString text;
  QString tip;
  NextBestActionIconKey icon = NextBestActionIconKey::ContentsView;
  bool enabled = true;
};

NextBestActionUiState
buildNextBestActionUiState(const NextBestActionDecision &decision,
                           const NextBestActionInput &input,
                           const NextBestActionTexts &texts);
