// Unit tests for StateMachineRowDetail.
#include "detail/StateMachineRowDetail.h"

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

void expectTrue(bool condition, const QString &message) {
  if (!condition) {
    fail(message);
  }
}

void expectFalse(bool condition, const QString &message) {
  if (condition) {
    fail(message);
  }
}

void expectContains(const QStringList &actual, const QString &expected,
                    const QString &message) {
  if (!actual.contains(expected)) {
    fail(QString("%1: missing %2").arg(message, expected));
  }
}

StateMachineRowDetailTexts englishTexts() {
  return {
      .unavailableText = "State evidence is not available.",
      .unavailableTip = "Preview is local.",
      .noSelectionText = "Select a visible slave row.",
      .noSelectionTip = "Selection is local.",
      .unnamed = "Unnamed",
      .unknown = "Unknown",
      .none = "None",
      .noRisk = "No risk",
      .reviewEvidence = "Review evidence",
      .confirmedRequestBoundary = "Explicit confirmed state request",
      .localReviewBoundaryLabel = "Local evidence review",
      .opBoundary = "OP transition requires confirmation",
      .safeopBoundary = "SAFEOP transition requires confirmation",
      .preopBoundary = "PREOP transition requires confirmation",
      .summaryPattern =
          "#%1 %2 | Current: %3 | Recommended: %4 | %5 | Risk: %6 | Action: "
          "%7",
      .selectedTitle = "Selected state-machine row",
      .slaveLabel = "Slave",
      .currentStateLabel = "Current state",
      .recommendedStateLabel = "Recommended state",
      .evidenceLabel = "Evidence",
      .driveLabel = "Drive",
      .startupLabel = "Startup",
      .processLabel = "PDO/Process",
      .riskLabel = "Risk",
      .boundaryLabel = "Boundary",
      .localBoundary = "Local boundary.",
      .executionBoundary = "Execution boundary.",
  };
}

StateMachineTableRow readyRow() {
  StateMachineTableRow row;
  row.row = 2;
  row.position = "3";
  row.name = "Drive A";
  row.current = "PREOP";
  row.recommended = "SAFEOP";
  row.evidence = "PDO + Watch";
  row.drive = "Statusword ok";
  row.startup = "No diff";
  row.process = "PDO loaded";
  row.action = "Send SAFEOP";
  return row;
}

void testEmptyStates() {
  StateMachineRowDetailState state =
      stateMachineRowDetailUnavailableState(englishTexts());
  expectEqual(state.text, "State evidence is not available.",
              "unavailable text");
  expectEqual(state.severityKey, "neutral", "unavailable severity");
  expectEqual(state.tooltip, "Preview is local.", "unavailable tooltip");

  state = stateMachineRowDetailNoSelectionState(englishTexts());
  expectEqual(state.text, "Select a visible slave row.", "no selection text");
  expectEqual(state.severityKey, "neutral", "no selection severity");
  expectEqual(state.tooltip, "Selection is local.", "no selection tooltip");
}

void testSeverityRules() {
  const StateMachineRowDetailTexts texts = englishTexts();
  StateMachineTableRow row = readyRow();
  expectEqual(stateMachineRowDetailSeverityKey(row, texts), "action",
              "non-OP recommendation is action");

  row.recommended = "OP";
  expectEqual(stateMachineRowDetailSeverityKey(row, texts), "warning",
              "OP recommendation is warning");

  row.risk = "offline";
  expectEqual(stateMachineRowDetailSeverityKey(row, texts), "error",
              "severe risk is error");

  row.risk.clear();
  row.recommended.clear();
  row.current = "OP";
  expectEqual(stateMachineRowDetailSeverityKey(row, texts), "ok",
              "OP current row is ok");

  row.current = "SAFEOP";
  expectEqual(stateMachineRowDetailSeverityKey(row, texts), "neutral",
              "review-only row is neutral");
}

void testSelectedRowState() {
  const StateMachineTableRow row = readyRow();
  const StateMachineRowDetailState state =
      buildStateMachineRowDetailState(row, englishTexts());

  expectEqual(state.severityKey, "action", "selected severity");
  expectEqual(state.boundary, "SAFEOP transition requires confirmation",
              "safeop boundary");
  expectTrue(state.hasRecommendation, "selected row has recommendation");
  expectFalse(state.hasRisk, "selected row has no risk");
  expectEqual(state.text,
              "#3 Drive A | Current: PREOP | Recommended: SAFEOP | SAFEOP "
              "transition requires confirmation | Risk: No risk | Action: "
              "Send SAFEOP",
              "summary text");
  expectContains(state.tooltipLines, "Selected state-machine row",
                 "tooltip title");
  expectContains(state.tooltipLines, "Slave: #3 Drive A", "tooltip slave");
  expectContains(state.tooltipLines, "Current state: PREOP",
                 "tooltip current state");
  expectContains(state.tooltipLines, "Recommended state: SAFEOP",
                 "tooltip recommended state");
  expectContains(state.tooltipLines,
                 "Boundary: SAFEOP transition requires confirmation",
                 "tooltip boundary");
  expectEqual(state.tooltip, state.tooltipLines.join('\n'), "tooltip text");
}

void testLocalizedRiskAndFallbackState() {
  StateMachineTableRow row;
  row.row = 4;
  row.current = "SAFEOP";
  row.risk = "驱动错误";
  const StateMachineRowDetailState state =
      buildStateMachineRowDetailState(row, englishTexts());

  expectEqual(state.severityKey, "error", "localized risk is error");
  expectTrue(state.severeRisk, "localized severe risk detected");
  expectEqual(state.displayPosition, "4", "fallback position");
  expectEqual(state.displayName, "Unnamed", "fallback name");
  expectEqual(state.displayRecommended, "None", "fallback recommendation");
  expectEqual(state.displayAction, "Review evidence", "fallback action");
  expectEqual(state.boundary, "Local evidence review", "fallback boundary");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testEmptyStates();
  testSeverityRules();
  testSelectedRowState();
  testLocalizedRiskAndFallbackState();
  return 0;
}
