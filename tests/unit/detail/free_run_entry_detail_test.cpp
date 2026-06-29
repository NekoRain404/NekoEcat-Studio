// FreeRunEntryDetailTest — Tests for FreeRunEntryDetail
//
// Test coverage:
//   - Empty states (unavailable, no selection)
//   - Severity rules (ok, action, warning, error)
//   - Selected row state and tooltip generation
//   - Fallback text for incomplete rows

// Unit tests for FreeRunEntryDetail.
#include "detail/FreeRunEntryDetail.h"

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

void expectContains(const QStringList &actual, const QString &expected,
                    const QString &message) {
  if (!actual.contains(expected)) {
    fail(QString("%1: missing %2").arg(message, expected));
  }
}

FreeRunEntryDetailTexts englishTexts() {
  return {
      .unavailableText = "Process-image evidence is not available.",
      .unavailableTip = "Preview is local.",
      .noSelectionText = "Select a visible process-image row.",
      .noSelectionTip = "Selection is local.",
      .unknown = "Unknown",
      .directionFallback = "Dir?",
      .unnamed = "Unnamed",
      .emptyValue = "Empty",
      .noMapEvidence = "No map evidence",
      .outputBoundary = "Output-like process data",
      .inputBoundary = "Input/telemetry process data",
      .mappedText = "Mapped",
      .summaryPattern =
          "#%1 %2 %3:%4 | %5 | %6 bit @ %7.%8 | %9 | Value: %10 | Map: %11",
      .nameSourceMarkers = {"Name source:", "name source:"},
      .selectedTitle = "Selected Free Run process-image entry",
      .slaveLabel = "Slave",
      .syncManagerLabel = "Sync Manager",
      .directionLabel = "Direction",
      .pdoLabel = "PDO",
      .objectLabel = "Object",
      .nameLabel = "Name",
      .nameSourceLabel = "Name Source",
      .locationLabel = "Location",
      .rawLabel = "Raw",
      .decodedLabel = "Decoded",
      .meaningLabel = "Meaning",
      .mapStatusLabel = "Map Status",
      .mapDetailLabel = "Map Detail",
      .changedLabel = "Changed",
      .yesText = "Yes",
      .noText = "No",
      .boundaryLabel = "Boundary",
      .localBoundary = "Local boundary.",
      .executionBoundary = "Execution boundary.",
  };
}

FreeRunEntryTableRow readyInputRow() {
  FreeRunEntryTableRow row;
  row.row = 4;
  row.position = 3;
  row.positionValid = true;
  row.syncManager = "2";
  row.direction = "Tx Input";
  row.pdo = "0x1a00";
  row.index = "0x6041";
  row.subIndex = "0x00";
  row.bits = "16";
  row.offset = "12";
  row.bit = "0";
  row.name = "Statusword";
  row.raw = "0x0027";
  row.decoded = "Operation enabled";
  row.meaning = "Statusword";
  row.mapStatus = "Mapped";
  row.mapDetail = "0x1a00 16 bit | Name source: PDO map";
  return row;
}

// Verify unavailable and no-selection empty states
void testEmptyStates() {
  FreeRunEntryDetailState state =
      freeRunEntryDetailUnavailableState(englishTexts());
  expectEqual(state.text, "Process-image evidence is not available.",
              "unavailable text");
  expectEqual(state.severityKey, "neutral", "unavailable severity");
  expectEqual(state.tooltip, "Preview is local.", "unavailable tooltip");

  state = freeRunEntryDetailNoSelectionState(englishTexts());
  expectEqual(state.text, "Select a visible process-image row.",
              "no selection text");
  expectEqual(state.severityKey, "neutral", "no selection severity");
  expectEqual(state.tooltip, "Selection is local.", "no selection tooltip");
}

// Verify severity rules for ok, action, warning, and error
void testSeverityRules() {
  const FreeRunEntryDetailTexts texts = englishTexts();
  FreeRunEntryTableRow row = readyInputRow();
  expectEqual(freeRunEntryDetailSeverityKey(row, texts), "ok",
              "mapped input severity");

  row.changed = true;
  expectEqual(freeRunEntryDetailSeverityKey(row, texts), "action",
              "changed input severity");

  row.changed = false;
  row.direction = "Rx Output";
  expectTrue(freeRunEntryDetailIsOutputLike(row), "rx row is output-like");
  expectEqual(freeRunEntryDetailSeverityKey(row, texts), "warning",
              "output row severity");

  row.mapStatus = "Mapped warning: bit mismatch 16/32";
  expectEqual(freeRunEntryDetailSeverityKey(row, texts), "error",
              "bit mismatch severity");

  row.mapStatus = "Missing in PDO map";
  row.direction = "Tx Input";
  expectEqual(freeRunEntryDetailSeverityKey(row, texts), "warning",
              "missing map severity");
}

// Verify selected row state, summary, and tooltip
void testSelectedRowState() {
  const FreeRunEntryTableRow row = readyInputRow();
  const FreeRunEntryDetailState state =
      buildFreeRunEntryDetailState(row, englishTexts());

  expectEqual(state.severityKey, "ok", "selected severity");
  expectEqual(state.nameSource, "PDO map", "name source parsed");
  expectEqual(state.boundary, "Input/telemetry process data", "boundary");
  expectEqual(state.text,
              "#3 Tx Input 0x6041:0x00 | Statusword | 16 bit @ 12.0 | "
              "Input/telemetry process data | Value: Operation enabled | Map: "
              "Mapped",
              "summary text");
  expectContains(state.tooltipLines, "Selected Free Run process-image entry",
                 "tooltip title");
  expectContains(state.tooltipLines, "Slave: #3", "tooltip slave");
  expectContains(state.tooltipLines, "Object: 0x6041:0x00", "tooltip object");
  expectContains(state.tooltipLines, "Name Source: PDO map",
                 "tooltip name source");
  expectContains(state.tooltipLines, "Changed: No", "tooltip changed");
  expectContains(state.tooltipLines, "Local boundary.", "tooltip boundary");
  expectEqual(state.tooltip, state.tooltipLines.join('\n'), "tooltip text");
}

// Verify fallback text for incomplete rows
void testFallbacks() {
  FreeRunEntryTableRow row;
  row.row = 7;
  const FreeRunEntryDetailState state =
      buildFreeRunEntryDetailState(row, englishTexts());
  expectEqual(state.severityKey, "neutral", "fallback severity");
  expectEqual(state.nameSource, "Unknown", "fallback name source");
  expectEqual(state.text,
              "#7 Dir? ----:-- | Unnamed | ? bit @ ?.? | "
              "Input/telemetry process data | Value: Empty | Map: No map "
              "evidence",
              "fallback summary");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testEmptyStates();
  testSeverityRules();
  testSelectedRowState();
  testFallbacks();
  return 0;
}
