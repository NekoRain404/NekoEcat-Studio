// WorkspaceTabBadgeDetailTest — Tests for Workspace Tab Badge Detail
//
// Test coverage:
//   - Badge text formatting (zero count, issue count, normal count)
//   - Overview priority counts (P0-P3 matrix)
//   - Startup diff issues precedence over row count
//   - Watch delta badge text
//   - Free Run row count badge
//   - I/O Variables badge with review issues
//   - Consistency badge with error/warning/info counts
//   - State machine risk badge
//   - Diagnostics badge with severity counts
#include "detail/WorkspaceTabBadgeDetail.h"

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

WorkspaceTabBadgeTexts englishTexts() {
  return {
      .overview = "Overview",
      .watch = "Watch",
      .startupSdo = "Startup SDO",
      .freeRun = "Free Run",
      .ioVariables = "I/O Variables",
      .consistency = "Consistency",
      .stateMachine = "State Machine",
      .diagnostics = "Diagnostics",
      .overviewTipPattern =
          "Slave matrix priority queue: P0 %1; P1 %2; P2 %3; P3 %4",
      .watchTipPattern = "Watch rows: %1; Startup mismatches: %2",
      .startupSdoTipPattern = "Startup SDO rows: %1; Watch mismatches: %2",
      .freeRunTipPattern = "Process-image rows: %1",
      .ioVariablesTipPattern = "I/O variable rows: %1; review issues: %2",
      .consistencyTipPattern =
          "Consistency errors: %1; warnings: %2; info: %3; ready: %4",
      .stateMachineTipPattern = "State-machine risk rows: %1",
      .diagnosticsTipPattern = "Diagnostics errors: %1; warnings: %2; info: %3",
  };
}

// Badge text with zero count shows label only, issues use ! prefix, normal uses count
void testBadgeText() {
  expectEqual(workspaceTabBadgeText("Watch", 0), "Watch", "zero count text");
  expectEqual(workspaceTabBadgeText("Overview", 3, true), "Overview !3",
              "issue count text");
  expectEqual(workspaceTabBadgeText("Free Run", 5), "Free Run 5",
              "normal count text");
}

// Overview badge shows sum of P0+P1 priority counts with matrix tip
void testOverviewPriorityCounts() {
  WorkspaceTabBadgeCounts counts;
  counts.matrixP0 = 1;
  counts.matrixP1 = 2;
  counts.matrixP2 = 3;
  counts.matrixP3 = 4;

  const WorkspaceTabBadgeDetail state =
      buildWorkspaceTabBadgeDetail(counts, englishTexts());

  expectEqual(state.overview.text, "Overview !6", "overview badge");
  expectEqual(state.overview.tip,
              "Slave matrix priority queue: P0 1; P1 2; P2 3; P3 4",
              "overview tip");
}

// Startup badge prefers diff issues over row count
void testStartupUsesDiffIssuesBeforeRows() {
  WorkspaceTabBadgeCounts counts;
  counts.startupRows = 12;
  counts.startupDiffs = 2;

  WorkspaceTabBadgeDetail state =
      buildWorkspaceTabBadgeDetail(counts, englishTexts());
  expectEqual(state.startupSdo.text, "Startup SDO !2", "startup issue badge");
  expectEqual(state.startupSdo.tip, "Startup SDO rows: 12; Watch mismatches: 2",
              "startup tip");

  counts.startupDiffs = 0;
  state = buildWorkspaceTabBadgeDetail(counts, englishTexts());
  expectEqual(state.startupSdo.text, "Startup SDO 12", "startup row badge");
}

// Consistency badge shows blocking issues (errors+warnings) before row count
void testConsistencyUsesBlockingIssuesBeforeRows() {
  WorkspaceTabBadgeCounts counts;
  counts.consistencyRows = 9;
  counts.consistencyErrors = 1;
  counts.consistencyWarnings = 3;
  counts.consistencyInfos = 4;
  counts.consistencyReady = 5;

  WorkspaceTabBadgeDetail state =
      buildWorkspaceTabBadgeDetail(counts, englishTexts());
  expectEqual(state.consistency.text, "Consistency !4",
              "consistency issue badge");
  expectEqual(state.consistency.tip,
              "Consistency errors: 1; warnings: 3; info: 4; ready: 5",
              "consistency tip");

  counts.consistencyErrors = 0;
  counts.consistencyWarnings = 0;
  state = buildWorkspaceTabBadgeDetail(counts, englishTexts());
  expectEqual(state.consistency.text, "Consistency 9", "consistency row badge");
}

// Diagnostics badge shows issues (errors+warnings) before row count
void testDiagnosticsUsesIssuesBeforeRows() {
  WorkspaceTabBadgeCounts counts;
  counts.diagnosticRows = 8;
  counts.diagnosticErrors = 2;
  counts.diagnosticWarnings = 1;
  counts.diagnosticInfos = 5;

  WorkspaceTabBadgeDetail state =
      buildWorkspaceTabBadgeDetail(counts, englishTexts());
  expectEqual(state.diagnostics.text, "Diagnostics !3",
              "diagnostics issue badge");
  expectEqual(state.diagnostics.tip,
              "Diagnostics errors: 2; warnings: 1; info: 5", "diagnostics tip");

  counts.diagnosticErrors = 0;
  counts.diagnosticWarnings = 0;
  state = buildWorkspaceTabBadgeDetail(counts, englishTexts());
  expectEqual(state.diagnostics.text, "Diagnostics 8", "diagnostics row badge");
}

// Verify watch, free run, I/O variables, and state machine badges
void testOtherWorkspaceBadges() {
  WorkspaceTabBadgeCounts counts;
  counts.watchRows = 7;
  counts.watchStartupDiffs = 2;
  counts.freeRunRows = 11;
  counts.ioRows = 6;
  counts.ioIssues = 2;
  counts.stateRiskRows = 1;

  const WorkspaceTabBadgeDetail state =
      buildWorkspaceTabBadgeDetail(counts, englishTexts());

  expectEqual(state.watch.text, "Watch 7", "watch badge");
  expectEqual(state.watch.tip, "Watch rows: 7; Startup mismatches: 2",
              "watch tip");
  expectEqual(state.freeRun.text, "Free Run 11", "free run badge");
  expectEqual(state.ioVariables.text, "I/O Variables !2", "io variables badge");
  expectEqual(state.stateMachine.text, "State Machine !1",
              "state machine badge");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testBadgeText();
  testOverviewPriorityCounts();
  testStartupUsesDiffIssuesBeforeRows();
  testConsistencyUsesBlockingIssuesBeforeRows();
  testDiagnosticsUsesIssuesBeforeRows();
  testOtherWorkspaceBadges();
  return 0;
}
