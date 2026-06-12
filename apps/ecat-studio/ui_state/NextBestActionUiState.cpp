// Rendered next-best-action card: title, body, icon, and safety level.
#include "NextBestActionUiState.h"

// Maps a next-best-action decision to the rendered card state with text, tip, icon, and severity.
NextBestActionUiState
buildNextBestActionUiState(const NextBestActionDecision &decision,
                           const NextBestActionInput &input,
                           const NextBestActionTexts &texts) {
  NextBestActionUiState state;
  state.actionKey = nextBestActionKey(decision.kind);
  state.severityKey = nextBestActionSeverityKey(decision.severity);
  state.text = texts.commands;
  state.tip = texts.commandPaletteTip;
  state.icon = NextBestActionIconKey::ContentsView;

  switch (decision.kind) {
  case NextBestActionKind::Connect:
    state.text = texts.nextConnect;
    state.tip = texts.connectTip;
    state.icon = NextBestActionIconKey::DriveNet;
    break;
  case NextBestActionKind::Diagnostics:
    state.text = texts.reviewDiagnostics;
    state.tip = texts.diagnosticsTip;
    state.icon = NextBestActionIconKey::Warning;
    break;
  case NextBestActionKind::Rescan:
    state.text = texts.nextRescan;
    state.tip = texts.rescanTip;
    state.icon = NextBestActionIconKey::DetailedView;
    break;
  case NextBestActionKind::SelectSlave:
    state.text = texts.nextSelectSlave;
    state.tip = texts.selectSlaveTip;
    state.icon = NextBestActionIconKey::ListView;
    break;
  case NextBestActionKind::LoadOd:
    state.text = texts.nextLoadOd;
    state.tip = texts.loadOdTip;
    state.icon = NextBestActionIconKey::DetailedView;
    break;
  case NextBestActionKind::FailedOdEvidence:
    state.text = texts.reviewOdEvidence;
    state.tip = texts.failedOdEvidenceTip;
    state.icon = NextBestActionIconKey::Warning;
    break;
  case NextBestActionKind::LoadPdo:
    state.text = texts.nextLoadPdo;
    state.tip = texts.loadPdoTip;
    state.icon = NextBestActionIconKey::ListView;
    break;
  case NextBestActionKind::AddWatch:
    state.text = texts.nextAddWatch;
    state.tip = texts.addWatchTip;
    state.icon = NextBestActionIconKey::NewFolder;
    break;
  case NextBestActionKind::StartupDiffs:
    state.text = texts.reviewStartupDiffs;
    state.tip = texts.startupDiffsTip;
    state.icon = NextBestActionIconKey::DetailedView;
    break;
  case NextBestActionKind::ConsistencyEvidenceIssue:
    state.text = texts.reviewEvidence;
    state.tip = texts.consistencyEvidenceTip;
    state.icon = NextBestActionIconKey::Warning;
    break;
  case NextBestActionKind::Consistency:
    state.text = input.workflow.hasConsistencyCheck ? texts.reviewConsistency
                                                    : texts.runConsistency;
    state.tip = input.workflow.hasConsistencyCheck ? texts.reviewConsistencyTip
                                                   : texts.runConsistencyTip;
    state.icon = input.workflow.hasConsistencyBlockingIssues
                     ? NextBestActionIconKey::Warning
                     : NextBestActionIconKey::DetailedView;
    break;
  case NextBestActionKind::FreeRun:
    state.text = texts.nextFreeRun;
    state.tip = texts.freeRunTip;
    state.icon = NextBestActionIconKey::MediaPlay;
    break;
  case NextBestActionKind::MatrixReview:
    state.text = input.matrixP0 + input.matrixP1 > 0 ? texts.reviewMatrixRisk
                                                     : texts.reviewMatrixAction;
    state.tip = texts.matrixTipPattern.arg(input.matrixP0)
                    .arg(input.matrixP1)
                    .arg(input.matrixP2);
    state.icon = NextBestActionIconKey::DetailedView;
    break;
  case NextBestActionKind::CommandPalette:
    break;
  }

  return state;
}
