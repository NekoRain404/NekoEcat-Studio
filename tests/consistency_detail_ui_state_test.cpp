// Unit tests for ConsistencyDetailUiState.
#include "ui_state/ConsistencyDetailUiState.h"

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

void expectContains(const QStringList &actual, const QString &expected,
                    const QString &message) {
  if (!actual.contains(expected)) {
    fail(QString("%1: missing %2").arg(message, expected));
  }
}

ConsistencyDetailTexts englishTexts() {
  return {
      .unavailableText = "Consistency evidence is not available.",
      .unavailableTip = "Preview is local.",
      .selectVisibleRowText = "Select a visible Consistency row.",
      .selectVisibleRowTip = "Selection is local.",
      .summaryPattern = "%1 | %2 | %3 | Expected: %4 | Actual: %5 | Action: %6",
      .selectedRowTitle = "Selected Consistency row",
      .levelLabel = "Level",
      .scopeLabel = "Scope",
      .targetLabel = "Target",
      .evidenceLabel = "Evidence",
      .expectedLabel = "Expected",
      .actualLabel = "Actual",
      .actionLabel = "Action",
      .routeLabel = "Open Evidence Route",
      .levelFallback = "Level?",
      .scopeFallback = "Scope?",
      .targetFallback = "No target",
      .expectedFallback = "No expected evidence",
      .actualFallback = "No actual evidence",
      .routeIo = "Open I/O Variables evidence",
      .routeTopology = "Open State Machine/topology evidence",
      .routeStartup = "Open Startup SDO evidence",
      .routeWatchOrIo = "Open Watch or I/O evidence",
      .routeReady = "Ready row; continue commissioning",
      .localBoundary = "Local gate boundary.",
      .executionBoundary = "Execution boundary.",
  };
}

void testEmptyStates() {
  ConsistencyDetailUiState state =
      consistencyDetailUnavailableState(englishTexts());
  expectEqual(state.text, "Consistency evidence is not available.",
              "unavailable text");
  expectEqual(state.severityKey, "neutral", "unavailable severity");
  expectEqual(state.tooltip, "Preview is local.", "unavailable tip");

  state = consistencyDetailNoSelectionState(englishTexts());
  expectEqual(state.text, "Select a visible Consistency row.",
              "no selection text");
  expectEqual(state.severityKey, "neutral", "no selection severity");
  expectEqual(state.tooltip, "Selection is local.", "no selection tip");
}

void testSeverityAndRoutes() {
  expectEqual(consistencyDetailSeverityKey("Error"), "error", "error severity");
  expectEqual(consistencyDetailSeverityKey("警告"), "warning",
              "warning severity");
  expectEqual(consistencyDetailSeverityKey("Ready"), "ok", "ready severity");
  expectEqual(consistencyDetailSeverityKey("Info"), "action", "info severity");

  const ConsistencyDetailTexts texts = englishTexts();
  expectEqual(consistencyDetailRoute({.scope = "Topology"}, texts),
              "Open State Machine/topology evidence", "topology route");
  expectEqual(consistencyDetailRoute({.scope = "Startup"}, texts),
              "Open Startup SDO evidence", "startup route");
  expectEqual(consistencyDetailRoute({.evidence = "watch missing"}, texts),
              "Open Watch or I/O evidence", "watch route");
  expectEqual(consistencyDetailRoute({.scope = "Project"}, texts),
              "Ready row; continue commissioning", "ready route");
  expectEqual(consistencyDetailRoute({}, texts), "Open I/O Variables evidence",
              "default route");
}

void testSelectedRowState() {
  const ConsistencyDetailUiState state =
      buildConsistencyDetailUiState({.level = "Warning",
                                     .scope = "Startup",
                                     .target = "#1 0x6040:0x00",
                                     .evidence = "diff",
                                     .expected = "0x0006",
                                     .actual = "0x0007",
                                     .action = ""},
                                    englishTexts());

  expectEqual(state.severityKey, "warning", "selected severity");
  expectEqual(state.route, "Open Startup SDO evidence", "selected route");
  expectEqual(state.text,
              "Warning | Startup | #1 0x6040:0x00 | Expected: 0x0006 | "
              "Actual: 0x0007 | Action: Open Startup SDO evidence",
              "selected summary");
  expectContains(state.tooltipLines, "Selected Consistency row",
                 "tooltip title");
  expectContains(state.tooltipLines, "Level: Warning", "tooltip level");
  expectContains(state.tooltipLines,
                 "Open Evidence Route: Open Startup SDO evidence",
                 "tooltip route");
  expectContains(state.tooltipLines, "Local gate boundary.",
                 "tooltip local boundary");
  expectEqual(state.tooltip, state.tooltipLines.join('\n'), "tooltip text");
}

void testFallbackText() {
  const ConsistencyDetailUiState state =
      buildConsistencyDetailUiState({}, englishTexts());
  expectEqual(state.text,
              "Level? | Scope? | No target | Expected: No expected evidence | "
              "Actual: No actual evidence | Action: Open I/O Variables "
              "evidence",
              "fallback summary");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testEmptyStates();
  testSeverityAndRoutes();
  testSelectedRowState();
  testFallbackText();
  return 0;
}
