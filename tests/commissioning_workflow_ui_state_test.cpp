#include "ui_state/CommissioningWorkflowUiState.h"

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

void expectEqual(int actual, int expected, const QString &message) {
  if (actual != expected) {
    fail(QString("%1: expected %2, got %3")
             .arg(message)
             .arg(expected)
             .arg(actual));
  }
}

void expectStatus(CommissioningWorkflowStatus actual,
                  CommissioningWorkflowStatus expected,
                  const QString &message) {
  if (actual != expected) {
    fail(message);
  }
}

CommissioningWorkflowTexts englishTexts() {
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

void testStatusRulesAndKeys() {
  expectStatus(commissioningWorkflowStatus(true, false),
               CommissioningWorkflowStatus::Ready, "done row is ready");
  expectStatus(commissioningWorkflowStatus(false, true),
               CommissioningWorkflowStatus::Action, "ready row needs action");
  expectStatus(commissioningWorkflowStatus(false, false),
               CommissioningWorkflowStatus::Blocked, "not ready row blocks");
  expectEqual(commissioningWorkflowColorKey(CommissioningWorkflowStatus::Ready),
              "ready", "ready color key");
  expectEqual(
      commissioningWorkflowColorKey(CommissioningWorkflowStatus::Action),
      "action", "action color key");
  expectEqual(
      commissioningWorkflowColorKey(CommissioningWorkflowStatus::Blocked),
      "blocked", "blocked color key");
}

void testHeadersUiRowAndStats() {
  const CommissioningWorkflowTexts texts = englishTexts();
  const QStringList headers = commissioningWorkflowHeaders(texts);
  expectEqual(headers.size(), 6, "workflow header count");
  expectEqual(headers.first(), "Phase", "first workflow header");
  expectEqual(headers.last(), "Next Action", "last workflow header");

  CommissioningWorkflowRow readyRow;
  readyRow.phase = "Runtime";
  readyRow.status = CommissioningWorkflowStatus::Ready;
  readyRow.step = "Connect runtime";
  readyRow.risk = "None";
  readyRow.evidence = "ecatd is connected";
  readyRow.action = "Refresh online data";

  CommissioningWorkflowRow blockedRow;
  blockedRow.phase = "Map";
  blockedRow.status = CommissioningWorkflowStatus::Blocked;
  blockedRow.step = "Review PDO Map";
  blockedRow.risk = "PDO missing";
  blockedRow.evidence = "PDO data not loaded";
  blockedRow.action = "Select a slave first";

  const QVector<CommissioningWorkflowUiRow> rows = {
      commissioningWorkflowUiRow(readyRow, texts),
      commissioningWorkflowUiRow(blockedRow, texts),
  };
  expectEqual(rows.at(0).cells.size(), 6, "workflow cell count");
  expectEqual(rows.at(0).cells.at(1), "Ready", "ready status cell");
  expectEqual(rows.at(1).cells.at(3), "PDO missing", "risk cell");
  expectEqual(rows.at(1).tooltip,
              "Phase: Map\nStatus: Blocked\nRisk: PDO missing\nEvidence: PDO "
              "data not loaded\nNext: Select a slave first",
              "workflow tooltip");
  expectEqual(commissioningWorkflowTableRows(rows).size(), 2,
              "table row count");

  const CommissioningWorkflowStats stats = commissioningWorkflowStats(rows);
  expectEqual(stats.ready, 1, "ready stats");
  expectEqual(stats.action, 0, "action stats");
  expectEqual(stats.blocked, 1, "blocked stats");
}

void testStepBoundaries() {
  const CommissioningWorkflowTexts texts = englishTexts();
  const CommissioningWorkflowStepBoundary odBoundary =
      commissioningWorkflowStepBoundary(
          CommissioningWorkflowStep::InspectObjectDictionary, texts);
  expectEqual(odBoundary.kind, "Online OD read", "OD boundary kind");
  expectEqual(odBoundary.detail, "Inspect OD boundary", "OD boundary detail");

  const CommissioningWorkflowStepBoundary processBoundary =
      commissioningWorkflowStepBoundary(
          CommissioningWorkflowStep::ValidateProcessImage, texts);
  expectEqual(processBoundary.kind, "Process data action",
              "process boundary kind");
  expectEqual(processBoundary.detail, "Validate process image boundary",
              "process boundary detail");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testStatusRulesAndKeys();
  testHeadersUiRowAndStats();
  testStepBoundaries();
  return 0;
}
