// Unit tests for IoVariableDetailUiState.
#include "ui_state/IoVariableDetailUiState.h"

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

IoVariableDetailTexts englishTexts() {
  return {
      .unavailableText = "I/O variable evidence is not available.",
      .unavailableTip = "Preview is local.",
      .noSelectionText = "Select a visible I/O variable.",
      .noSelectionTip = "Selection is local.",
      .readyText = "Ready",
      .noValue = "No value",
      .directionFallback = "Direction?",
      .unnamedSignal = "Unnamed signal",
      .noComparison = "No comparison",
      .noMapEvidence = "No map evidence",
      .notReviewed = "Not reviewed",
      .startupMismatch = "Startup mismatch",
      .mapIssue = "Map issue",
      .plcReview = "PLC review",
      .missingValue = "Missing value",
      .changed = "Changed",
      .readyEvidence = "Ready evidence",
      .summaryPattern = "#%1 %2:%3 | %4 | %5 | Value: %6 | Startup: %7 | "
                        "Map: %8 | PLC: %9",
      .selectedTitle = "Selected I/O variable",
      .slaveLabel = "Slave",
      .directionLabel = "Direction",
      .symbolLabel = "Symbol",
      .aliasLabel = "Alias",
      .objectLabel = "Object",
      .bitsLabel = "Bits",
      .pdoLabel = "PDO",
      .sourceLabel = "Source",
      .rawLabel = "Raw",
      .decodedLabel = "Decoded",
      .meaningLabel = "Meaning",
      .watchLabel = "Watch",
      .startupLabel = "Startup",
      .mapLabel = "Map",
      .changedLabel = "Changed",
      .plcLabel = "PLC",
      .tagsLabel = "Tags",
      .noteLabel = "Note",
      .signalStateLabel = "Signal State",
      .localBoundary = "Local boundary.",
      .executionBoundary = "Execution boundary.",
  };
}

IoVariableTableRow readyRow() {
  IoVariableTableRow row;
  row.position = 3;
  row.positionValid = true;
  row.direction = "Rx Output";
  row.symbol = "Drive.Statusword";
  row.index = "0x6041";
  row.subIndex = "0x00";
  row.bits = "16";
  row.pdo = "0x1a00";
  row.source = "Process | uint16";
  row.raw = "0x1234";
  row.decoded = "Operation enabled";
  row.meaning = "Statusword";
  row.plcQuality = "Ready";
  row.alias = "Axis_Statusword";
  row.tags = "axis, input";
  row.note = "commissioning";
  return row;
}

void testEmptyStates() {
  IoVariableDetailUiState state =
      ioVariableDetailUnavailableState(englishTexts());
  expectEqual(state.text, "I/O variable evidence is not available.",
              "unavailable text");
  expectEqual(state.severityKey, "neutral", "unavailable severity");
  expectEqual(state.tooltip, "Preview is local.", "unavailable tooltip");

  state = ioVariableDetailNoSelectionState(englishTexts());
  expectEqual(state.text, "Select a visible I/O variable.",
              "no selection text");
  expectEqual(state.severityKey, "neutral", "no selection severity");
  expectEqual(state.tooltip, "Selection is local.", "no selection tooltip");
}

void testSeverityAndSignalState() {
  const IoVariableDetailTexts texts = englishTexts();
  IoVariableTableRow row = readyRow();
  expectEqual(ioVariableDetailSeverityKey(row, texts.readyText), "ok",
              "ready row severity");
  expectEqual(ioVariableDetailSignalState(row, texts), "Ready evidence",
              "ready signal state");

  row.startup = "Diff";
  expectEqual(ioVariableDetailSeverityKey(row, texts.readyText), "error",
              "startup diff severity");
  expectEqual(ioVariableDetailSignalState(row, texts), "Startup mismatch",
              "startup diff signal state");

  row.startup.clear();
  row.map = "Missing in PDO map";
  expectEqual(ioVariableDetailSeverityKey(row, texts.readyText), "warning",
              "map issue severity");
  expectEqual(ioVariableDetailSignalState(row, texts), "Map issue",
              "map issue signal state");

  row.map.clear();
  row.plcQuality = "Missing Alias";
  expectEqual(ioVariableDetailSignalState(row, texts), "PLC review",
              "PLC issue signal state");
}

void testSelectedRowState() {
  const IoVariableDetailUiState state =
      buildIoVariableDetailUiState(readyRow(), englishTexts());
  expectEqual(state.severityKey, "ok", "selected severity");
  expectEqual(state.signalState, "Ready evidence", "selected signal state");
  expectEqual(state.text,
              "#3 0x6041:0x00 | Rx Output | Axis_Statusword | Value: "
              "Operation enabled | Startup: No comparison | Map: No map "
              "evidence | PLC: Ready",
              "selected summary");
  expectContains(state.tooltipLines, "Selected I/O variable", "tooltip title");
  expectContains(state.tooltipLines, "Slave: #3", "tooltip slave");
  expectContains(state.tooltipLines, "Object: 0x6041:0x00", "tooltip object");
  expectContains(state.tooltipLines, "Signal State: Ready evidence",
                 "tooltip signal state");
  expectContains(state.tooltipLines, "Local boundary.", "tooltip boundary");
  expectEqual(state.tooltip, state.tooltipLines.join('\n'), "tooltip text");
}

void testFallbackText() {
  IoVariableTableRow row;
  row.positionValid = false;
  const IoVariableDetailUiState state =
      buildIoVariableDetailUiState(row, englishTexts());
  expectEqual(state.severityKey, "warning", "empty row severity");
  expectEqual(state.text,
              "#? ----:-- | Direction? | Unnamed signal | Value: No value | "
              "Startup: No comparison | Map: No map evidence | PLC: Not "
              "reviewed",
              "fallback summary");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testEmptyStates();
  testSeverityAndSignalState();
  testSelectedRowState();
  testFallbackText();
  return 0;
}
