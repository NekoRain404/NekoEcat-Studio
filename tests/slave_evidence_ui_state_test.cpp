#include "ui_state/SlaveEvidenceUiState.h"

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

SlaveEvidenceUiTexts englishTexts() {
  return {
      .p0Fault = "P0 Fault",
      .p1Risk = "P1 Risk",
      .p2Action = "P2 Action",
      .p3Ready = "P3 Ready",
      .reviewOd = "Review OD",
      .loadPdo = "Load PDO",
      .addWatch = "Add Watch",
      .reviewStartup = "Review Startup",
      .validateProcess = "Validate Process",
      .reviewRisk = "Review Risk",
      .ready = "Ready",
      .identityMissing = "identity missing",
      .odMissing = "OD missing",
      .pdoMissing = "PDO missing",
      .watchMissing = "Watch missing",
      .processMissing = "process evidence missing",
      .startupDiffPattern = "Startup diff %1",
      .pdoMapIssuePattern = "PDO map issue %1",
      .topologyBaselineIssue = "topology baseline issue",
      .driveFaultEvidence = "drive fault evidence",
      .unknownEvidenceRisk = "unknown evidence risk",
      .unnamed = "Unnamed",
      .unknown = "Unknown",
      .missing = "Missing",
      .noRows = "No rows",
      .none = "None",
      .watchValuesPattern = "%1/%2 values",
      .startupRowsPattern = "%1 row(s), %2 diff(s)",
      .processRowsPattern = "%1 row(s), %2 issue(s)",
      .modePattern = "mode %1",
      .slavePattern = "Slave #%1 %2",
      .priorityPattern = "Priority: %1",
      .statePattern = "State: %1",
      .identityRowsPattern = "Identity rows: %1",
      .odRowsPattern = "Object Dictionary rows: %1",
      .pdoRowsPattern = "PDO rows: %1",
      .watchValuesDetailPattern = "Watch values: %1/%2",
      .startupRowsDetailPattern = "Startup rows: %1, diffs: %2",
      .processRowsDetailPattern = "Process rows: %1, map issues: %2",
      .drivePattern = "Drive: %1",
      .nextPattern = "Next: %1",
      .riskPattern = "Risk: %1",
      .priorityHeader = "Priority",
      .slaveHeader = "Slave",
      .nameHeader = "Name",
      .stateHeader = "State",
      .readinessHeader = "Readiness",
      .odHeader = "OD",
      .pdoHeader = "PDO",
      .watchHeader = "Watch",
      .startupHeader = "Startup",
      .processHeader = "Process",
      .riskHeader = "Risk",
      .nextHeader = "Next",
  };
}

void testDisplayFallbacks() {
  const SlaveEvidenceUiTexts texts = englishTexts();
  expectEqual(slaveEvidenceDisplayName("  ", texts), "Unnamed",
              "blank slave name fallback");
  expectEqual(slaveEvidenceDisplayState("\t", texts), "Unknown",
              "blank slave state fallback");
  expectEqual(slaveEvidenceDisplayName(" Axis ", texts), "Axis",
              "slave name is trimmed");
}

void testUiRowCellsAndDetails() {
  const SlaveEvidenceUiTexts texts = englishTexts();
  SlaveEvidenceRow row;
  row.position = 2;
  row.name = "Axis";
  row.state = "SAFEOP";
  row.readiness = 4;
  row.maxReadiness = 6;
  row.odRows = 12;
  row.pdoRows = 0;
  row.watchRows = 3;
  row.watchValueRows = 2;
  row.startupRows = 1;
  row.startupDiffs = 1;
  row.processRows = 0;
  row.mapIssues = 0;
  row.driveModeDisplay = "CSP";
  row.priority = SlaveEvidencePriority::Risk;
  row.nextAction = SlaveEvidenceNextAction::LoadPdo;
  row.risks = {{SlaveEvidenceRiskKind::PdoMissing, 0},
               {SlaveEvidenceRiskKind::StartupDiff, 1}};

  const SlaveEvidenceUiRow uiRow = slaveEvidenceUiRow(row, texts);
  expectEqual(uiRow.cells.size(), 12, "matrix cell count");
  expectEqual(uiRow.cells.at(0), "P1 Risk", "priority cell");
  expectEqual(uiRow.cells.at(2), "Axis", "name cell");
  expectEqual(uiRow.cells.at(4), "66% (4/6)", "readiness cell");
  expectEqual(uiRow.cells.at(6), "Missing", "PDO cell");
  expectEqual(uiRow.cells.at(7), "2/3 values", "Watch cell");
  expectEqual(uiRow.cells.at(8), "1 row(s), 1 diff(s)", "Startup cell");
  expectEqual(uiRow.cells.at(10), "PDO missing; Startup diff 1", "risk cell");
  expectEqual(uiRow.cells.at(11), "Load PDO", "next cell");
  expectEqual(uiRow.detailLines.first(), "Slave #2 Axis", "detail heading");
  expectEqual(uiRow.detailLines.last(), "Risk: PDO missing; Startup diff 1",
              "detail risk line");
}

void testMatrixHeaders() {
  const QStringList headers = slaveEvidenceMatrixHeaders(englishTexts());
  expectEqual(headers.size(), 12, "matrix header count");
  expectEqual(headers.first(), "Priority", "first matrix header");
  expectEqual(headers.last(), "Next", "last matrix header");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testDisplayFallbacks();
  testUiRowCellsAndDetails();
  testMatrixHeaders();
  return 0;
}
