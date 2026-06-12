// Unit tests for NextBestActionUiState.
#include "ui_state/NextBestActionUiState.h"

#include <QCoreApplication>

#include <cstdlib>
#include <iostream>

namespace {

void fail(const QString &message) {
  std::cerr << message.toStdString() << '\n';
  std::exit(1);
}

void expectEqual(const QString &actual, const QString &expected,
                 const QString &message) {
  if (actual != expected) {
    fail(QString("%1: expected %2, got %3").arg(message, expected, actual));
  }
}

void expectIcon(NextBestActionIconKey actual, NextBestActionIconKey expected,
                const QString &message) {
  if (actual != expected) {
    fail(message);
  }
}

NextBestActionTexts englishTexts() {
  return {
      .commands = "Commands",
      .commandPaletteTip = "Open commands",
      .nextConnect = "Next: Connect",
      .connectTip = "Connect runtime",
      .reviewDiagnostics = "Review Diagnostics",
      .diagnosticsTip = "Review diagnostics",
      .nextRescan = "Next: Rescan",
      .rescanTip = "Rescan bus",
      .nextSelectSlave = "Next: Select Slave",
      .selectSlaveTip = "Select slave",
      .nextLoadOd = "Next: Load OD",
      .loadOdTip = "Load OD",
      .reviewOdEvidence = "Review OD Evidence",
      .failedOdEvidenceTip = "Review failed OD",
      .nextLoadPdo = "Next: Load PDO",
      .loadPdoTip = "Load PDO",
      .nextAddWatch = "Next: Add Watch",
      .addWatchTip = "Add Watch",
      .reviewStartupDiffs = "Review Startup Diffs",
      .startupDiffsTip = "Review Startup diffs",
      .reviewEvidence = "Review Evidence",
      .consistencyEvidenceTip = "Open consistency evidence",
      .reviewConsistency = "Review Consistency",
      .reviewConsistencyTip = "Review consistency",
      .runConsistency = "Run Consistency",
      .runConsistencyTip = "Run consistency",
      .nextFreeRun = "Next: Free Run",
      .freeRunTip = "Open Free Run",
      .reviewMatrixRisk = "Review Matrix Risk",
      .reviewMatrixAction = "Review Matrix Action",
      .matrixTipPattern = "Matrix P0 %1 P1 %2 P2 %3",
  };
}

void testBasicActionUiState() {
  NextBestActionInput input;
  NextBestActionDecision decision{NextBestActionKind::Connect,
                                  NextBestActionSeverity::Action};
  const NextBestActionUiState state =
      buildNextBestActionUiState(decision, input, englishTexts());
  expectEqual(state.actionKey, "connect", "connect action key");
  expectEqual(state.severityKey, "action", "connect severity key");
  expectEqual(state.text, "Next: Connect", "connect text");
  expectEqual(state.tip, "Connect runtime", "connect tip");
  expectIcon(state.icon, NextBestActionIconKey::DriveNet, "connect icon");
}

void testConsistencyTextVariants() {
  NextBestActionInput input;
  NextBestActionDecision decision{NextBestActionKind::Consistency,
                                  NextBestActionSeverity::Action};
  NextBestActionUiState state =
      buildNextBestActionUiState(decision, input, englishTexts());
  expectEqual(state.text, "Run Consistency", "run consistency text");
  expectEqual(state.tip, "Run consistency", "run consistency tip");
  expectIcon(state.icon, NextBestActionIconKey::DetailedView,
             "run consistency icon");

  input.workflow.hasConsistencyCheck = true;
  input.workflow.hasConsistencyBlockingIssues = true;
  decision.severity = NextBestActionSeverity::Warning;
  state = buildNextBestActionUiState(decision, input, englishTexts());
  expectEqual(state.text, "Review Consistency", "review consistency text");
  expectEqual(state.tip, "Review consistency", "review consistency tip");
  expectIcon(state.icon, NextBestActionIconKey::Warning,
             "review consistency icon");
}

void testMatrixAndCommandPaletteUiState() {
  NextBestActionInput input;
  input.matrixP0 = 1;
  input.matrixP1 = 2;
  input.matrixP2 = 3;
  NextBestActionDecision decision{NextBestActionKind::MatrixReview,
                                  NextBestActionSeverity::Warning};
  NextBestActionUiState state =
      buildNextBestActionUiState(decision, input, englishTexts());
  expectEqual(state.text, "Review Matrix Risk", "matrix risk text");
  expectEqual(state.tip, "Matrix P0 1 P1 2 P2 3", "matrix tip");
  expectIcon(state.icon, NextBestActionIconKey::DetailedView, "matrix icon");

  input.matrixP0 = 0;
  input.matrixP1 = 0;
  state = buildNextBestActionUiState(decision, input, englishTexts());
  expectEqual(state.text, "Review Matrix Action", "matrix action text");

  decision = {NextBestActionKind::CommandPalette,
              NextBestActionSeverity::Neutral};
  state = buildNextBestActionUiState(decision, input, englishTexts());
  expectEqual(state.actionKey, "commandPalette", "commands action key");
  expectEqual(state.severityKey, "neutral", "commands severity key");
  expectEqual(state.text, "Commands", "commands text");
  expectEqual(state.tip, "Open commands", "commands tip");
  expectIcon(state.icon, NextBestActionIconKey::ContentsView, "commands icon");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testBasicActionUiState();
  testConsistencyTextVariants();
  testMatrixAndCommandPaletteUiState();
  return 0;
}
