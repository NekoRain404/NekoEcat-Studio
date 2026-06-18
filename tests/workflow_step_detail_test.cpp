// Unit tests for WorkflowStepDetail.
#include "detail/WorkflowStepDetail.h"

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

CommissioningWorkflowTexts workflowTexts() {
  return {
      .ready = "Ready",
      .action = "Action",
      .blocked = "Blocked",
      .onlineRuntimeAction = "Online runtime action",
      .onlineTopologyAction = "Online topology action",
      .localTargetSelection = "Local target selection",
      .onlineOdRead = "Online OD read",
      .localEvidenceReview = "Local evidence review",
      .onlinePdoRead = "Online PDO read",
      .localWatchEdit = "Local Watch edit",
      .localStartupReview = "Local Startup review",
      .consistencyGate = "Consistency gate",
      .processDataAction = "Process data action",
      .connectRuntimeBoundary = "Connect runtime boundary",
      .scanTopologyBoundary = "Scan topology boundary",
      .selectSlaveBoundary = "Select slave boundary",
      .inspectObjectDictionaryBoundary = "Inspect OD boundary",
      .reviewObjectDictionaryEvidenceBoundary = "Review OD evidence boundary",
      .reviewPdoMapBoundary = "Review PDO map boundary",
      .monitorWatchBoundary = "Monitor Watch boundary",
      .reviewStartupDiffsBoundary = "Review Startup diffs boundary",
      .runConsistencyGateBoundary = "Run Consistency gate boundary",
      .validateProcessImageBoundary = "Validate process image boundary",
      .phaseHeader = "Phase",
      .statusHeader = "Status",
      .stepHeader = "Step",
      .riskHeader = "Risk",
      .evidenceHeader = "Evidence",
      .nextActionHeader = "Next Action",
      .tooltipPattern =
          "Phase: %1\nStatus: %2\nRisk: %3\nEvidence: %4\nNext: %5",
  };
}

WorkflowStepDetailTexts detailTexts() {
  return {
      .unavailableText = "Workflow evidence is not available.",
      .unavailableTip = "Detail strip is local.",
      .noSelectionText = "Select a visible workflow row.",
      .noSelectionTip = "Selection is local.",
      .none = "None",
      .noRisk = "No risk",
      .summaryPattern = "#%1 %2 / %3 | %4 | Risk: %5 | Evidence: %6 | Next: %7",
      .selectedTitle = "Selected workflow step",
      .phaseLabel = "Phase",
      .statusLabel = "Status",
      .stepLabel = "Step",
      .riskLabel = "Risk",
      .evidenceLabel = "Evidence",
      .nextActionLabel = "Next Action",
      .boundaryLabel = "Boundary",
      .localReviewBoundary = "Local review boundary.",
  };
}

CommissioningWorkflowTableRow actionRow() {
  CommissioningWorkflowTableRow row;
  row.row = 3;
  row.phase = "Evidence";
  row.status = "Action";
  row.statusKey = "action";
  row.step = "Inspect Object Dictionary";
  row.risk = "OD missing";
  row.evidence = "Object data not loaded";
  row.nextAction = "Open Object Dictionary";
  return row;
}

void testEmptyStates() {
  WorkflowStepDetailState state =
      workflowStepDetailUnavailableState(detailTexts());
  expectEqual(state.text, "Workflow evidence is not available.",
              "unavailable text");
  expectEqual(state.severityKey, "neutral", "unavailable severity");
  expectEqual(state.tooltip, "Detail strip is local.", "unavailable tooltip");

  state = workflowStepDetailNoSelectionState(detailTexts());
  expectEqual(state.text, "Select a visible workflow row.",
              "no selection text");
  expectEqual(state.severityKey, "neutral", "no selection severity");
  expectEqual(state.tooltip, "Selection is local.", "no selection tooltip");
}

void testSeverityRules() {
  const CommissioningWorkflowTexts workflow = workflowTexts();
  const WorkflowStepDetailTexts detail = detailTexts();
  CommissioningWorkflowTableRow row = actionRow();
  expectEqual(workflowStepDetailSeverityKey(row, workflow, detail),
              "action", "action status severity");

  row.status = "Translated Ready";
  row.statusKey = "ready";
  row.risk = "None";
  expectEqual(workflowStepDetailSeverityKey(row, workflow, detail),
              "ok", "status key drives ready severity");

  row.statusKey = "blocked";
  expectEqual(workflowStepDetailSeverityKey(row, workflow, detail),
              "warning", "blocked status severity");

  row.risk = "Drive failed";
  expectEqual(workflowStepDetailSeverityKey(row, workflow, detail),
              "error", "severe risk overrides status");
}

void testSelectedRowState() {
  const WorkflowStepDetailState state =
      buildWorkflowStepDetailState(actionRow(), workflowTexts(),
                                                  detailTexts());

  expectEqual(state.severityKey, "action", "selected severity");
  expectTrue(state.action, "action flag");
  expectFalse(state.ready, "ready flag");
  expectTrue(state.hasRisk, "risk flag");
  expectFalse(state.severeRisk, "severe risk flag");
  expectEqual(state.boundaryKind, "Online OD read", "boundary kind");
  expectEqual(state.boundaryDetail, "Inspect OD boundary", "boundary detail");
  expectEqual(state.displayRisk, "OD missing", "display risk");
  expectEqual(
      state.text,
      "#4 Evidence / Action | Online OD read | Risk: OD missing | Evidence: "
      "Object data not loaded | Next: Open Object Dictionary",
      "summary text");
  expectContains(state.tooltipLines, "Selected workflow step", "tooltip title");
  expectContains(state.tooltipLines, "Phase: Evidence", "tooltip phase");
  expectContains(state.tooltipLines, "Step: Inspect Object Dictionary",
                 "tooltip step");
  expectContains(state.tooltipLines, "Boundary: Online OD read",
                 "tooltip boundary");
  expectContains(state.tooltipLines, "Local review boundary.",
                 "tooltip local boundary");
  expectEqual(state.tooltip, state.tooltipLines.join('\n'), "tooltip text");
}

void testLocalizedRiskAndNoRiskFallback() {
  CommissioningWorkflowTableRow row = actionRow();
  row.status = "受阻";
  row.statusKey.clear();
  row.risk = "设备错误";
  CommissioningWorkflowTexts workflow = workflowTexts();
  workflow.blocked = "受阻";
  WorkflowStepDetailState state =
      buildWorkflowStepDetailState(row, workflow, detailTexts());
  expectEqual(state.severityKey, "error", "localized severe risk");
  expectTrue(state.severeRisk, "localized severe risk flag");

  row.risk.clear();
  state =
      buildWorkflowStepDetailState(row, workflow, detailTexts());
  expectEqual(state.severityKey, "warning", "localized blocked fallback");
  expectEqual(state.displayRisk, "No risk", "empty risk fallback");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testEmptyStates();
  testSeverityRules();
  testSelectedRowState();
  testLocalizedRiskAndNoRiskFallback();
  return 0;
}
