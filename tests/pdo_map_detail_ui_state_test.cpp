#include "PdoMapDetailUiState.h"

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

PdoMapDetailTexts englishTexts() {
  return {
      .unavailableText = "PDO Map evidence is not available.",
      .unavailableTip = "Preview is local.",
      .noSelectionText = "Select a visible PDO entry.",
      .noSelectionTip = "Selection is local.",
      .directionRxOutput = "Rx output",
      .directionTxInput = "Tx input",
      .directionUnknown = "PDO direction unknown",
      .roleRxOutput = "Output/process command candidate",
      .roleTxInput = "Input/process feedback candidate",
      .roleGeneric = "Generic process-data entry",
      .typeFallback = "type?",
      .unnamed = "Unnamed PDO entry",
      .cia402Candidate = "CiA 402 candidate",
      .genericEntry = "Generic PDO entry",
      .summaryPattern = "%1 | %2:%3 | %4 bit %5 | %6 | %7",
      .selectedTitle = "Selected PDO Map row",
      .slaveLabel = "Slave",
      .syncManagerLabel = "Sync Manager",
      .pdoLabel = "PDO",
      .objectLabel = "Object",
      .bitsLabel = "Bits",
      .inferredTypeLabel = "Inferred SDO Type",
      .nameLabel = "Name",
      .directionLabel = "Direction",
      .roleLabel = "Process Role",
      .driveEvidenceLabel = "Drive Evidence",
      .localBoundary = "Local boundary.",
      .executionBoundary = "Execution boundary.",
  };
}

PdoMapTableRow rxControlwordRow() {
  PdoMapTableRow row;
  row.row = 2;
  row.syncManager = "SM2";
  row.pdo = "RxPDO 0x1600";
  row.index = "0x6040";
  row.subIndex = "0x00";
  row.bits = "16";
  row.name = "Controlword";
  return row;
}

void testEmptyStates() {
  PdoMapDetailUiState state = pdoMapDetailUnavailableState(englishTexts());
  expectEqual(state.text, "PDO Map evidence is not available.",
              "unavailable text");
  expectEqual(state.severityKey, "neutral", "unavailable severity");
  expectEqual(state.tooltip, "Preview is local.", "unavailable tooltip");

  state = pdoMapDetailNoSelectionState(englishTexts());
  expectEqual(state.text, "Select a visible PDO entry.", "no selection text");
  expectEqual(state.severityKey, "neutral", "no selection severity");
  expectEqual(state.tooltip, "Selection is local.", "no selection tooltip");
}

void testDirectionSeverityAndDriveEvidence() {
  PdoMapTableRow row = rxControlwordRow();
  expectTrue(pdoMapDetailIsRxOutput(row), "RxPDO is output-like");
  expectFalse(pdoMapDetailIsTxInput(row), "RxPDO is not Tx input");
  expectTrue(pdoMapDetailIsCia402(row), "controlword is CiA 402");
  expectEqual(pdoMapDetailSeverityKey(row), "action",
              "Rx output gets action severity");

  row.pdo = "TxPDO 0x1a00";
  row.index = "0x6041";
  row.name = "Statusword";
  expectTrue(pdoMapDetailIsTxInput(row), "TxPDO is input-like");
  expectEqual(pdoMapDetailSeverityKey(row), "ok", "valid Tx input is ok");

  row.bits = "0";
  expectEqual(pdoMapDetailSeverityKey(row), "warning",
              "invalid bits warn before direction role");

  row.bits = "16";
  row.index.clear();
  expectEqual(pdoMapDetailSeverityKey(row), "warning", "missing address warns");
}

void testSelectedRowState() {
  const PdoMapTableRow row = rxControlwordRow();
  const PdoMapDetailUiState state =
      buildPdoMapDetailUiState(row, 3, englishTexts());

  expectEqual(state.severityKey, "action", "selected severity");
  expectEqual(state.direction, "Rx output", "selected direction");
  expectEqual(state.role, "Output/process command candidate", "selected role");
  expectEqual(state.inferredType, "uint16", "inferred type");
  expectTrue(state.cia402, "selected row is drive evidence");
  expectEqual(state.text,
              "Rx output | 0x6040:0x00 | 16 bit uint16 | Controlword | "
              "Output/process command candidate",
              "summary text");
  expectContains(state.tooltipLines, "Selected PDO Map row", "tooltip title");
  expectContains(state.tooltipLines, "Slave: #3", "tooltip slave");
  expectContains(state.tooltipLines, "Object: 0x6040:0x00", "tooltip object");
  expectContains(state.tooltipLines, "Drive Evidence: CiA 402 candidate",
                 "tooltip drive evidence");
  expectContains(state.tooltipLines, "Local boundary.", "tooltip boundary");
  expectEqual(state.tooltip, state.tooltipLines.join('\n'), "tooltip text");
}

void testFallbackState() {
  PdoMapTableRow row;
  const PdoMapDetailUiState state =
      buildPdoMapDetailUiState(row, 9, englishTexts());

  expectEqual(state.severityKey, "warning", "empty row warns");
  expectEqual(state.direction, "PDO direction unknown", "fallback direction");
  expectEqual(state.role, "Generic process-data entry", "fallback role");
  expectEqual(state.text,
              "PDO direction unknown | ----:-- | ? bit type? | Unnamed PDO "
              "entry | Generic process-data entry",
              "fallback summary");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testEmptyStates();
  testDirectionSeverityAndDriveEvidence();
  testSelectedRowState();
  testFallbackState();
  return 0;
}
