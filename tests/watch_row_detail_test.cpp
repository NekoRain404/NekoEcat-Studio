// Unit tests for WatchRowDetail.
#include "detail/WatchRowDetail.h"

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

WatchRowDetailTexts englishTexts() {
  return {
      .unavailableText = "Watch evidence is not available.",
      .unavailableTip = "Preview is local.",
      .noSelectionText = "Select a visible Watch row.",
      .noSelectionTip = "Selection is local.",
      .emptyValue = "Empty",
      .typeFallback = "Type?",
      .noBaseline = "No baseline",
      .noComparison = "No comparison",
      .startupMismatch = "Startup mismatch",
      .baselineDrift = "Baseline drift",
      .changed = "Changed",
      .stableEvidence = "Stable evidence",
      .cia402Candidate = "CiA 402 candidate",
      .genericSdo = "Generic SDO",
      .matchText = "Match",
      .pendingText = "Pending",
      .summaryPattern =
          "#%1 %2:%3 | %4 | Value: %5 | Baseline: %6 | Startup: %7 | %8",
      .selectedTitle = "Selected Watch row",
      .timeLabel = "Time",
      .slaveLabel = "Slave",
      .objectLabel = "Object",
      .typeLabel = "Type",
      .modeLabel = "Mode",
      .valueLabel = "Value",
      .decodedLabel = "Decoded",
      .baselineLabel = "Baseline",
      .baselineDeltaLabel = "Baseline Delta",
      .startupLabel = "Startup",
      .startupDeltaLabel = "Startup Delta",
      .changedLabel = "Changed",
      .yesText = "Yes",
      .noText = "No",
      .driveEvidenceLabel = "Drive Evidence",
      .localBoundary = "Local boundary.",
      .executionBoundary = "Execution boundary.",
  };
}

WatchStartupWatchRow readyRow() {
  WatchStartupWatchRow row;
  row.row = 4;
  row.position = 3;
  row.time = "12:00";
  row.index = "0x6041";
  row.subIndex = "0x00";
  row.value = "0x0027";
  row.decoded = "Operation enabled";
  row.type = "uint16";
  row.mode = "CiA 402";
  row.baseline = "0x0027";
  row.baselineDelta = "Match";
  row.startup = "0x0027";
  row.startupDelta = "Match";
  return row;
}

void testEmptyStates() {
  WatchRowDetailState state = watchRowDetailUnavailableState(englishTexts());
  expectEqual(state.text, "Watch evidence is not available.",
              "unavailable text");
  expectEqual(state.severityKey, "neutral", "unavailable severity");
  expectEqual(state.tooltip, "Preview is local.", "unavailable tooltip");

  state = watchRowDetailNoSelectionState(englishTexts());
  expectEqual(state.text, "Select a visible Watch row.", "no selection text");
  expectEqual(state.severityKey, "neutral", "no selection severity");
  expectEqual(state.tooltip, "Selection is local.", "no selection tooltip");
}

void testSeverityRules() {
  const WatchRowDetailTexts texts = englishTexts();
  WatchStartupWatchRow row = readyRow();
  expectEqual(watchRowDetailSeverityKey(row, texts), "ok",
              "stable valued row is ok");
  expectTrue(watchRowDetailIsMatchText("0", texts), "zero is match text");
  expectTrue(watchRowDetailIsMatchText("Pending", texts),
             "pending is match text");
  expectTrue(watchRowDetailIsCia402(row), "CiA mode is drive evidence");

  row.changed = true;
  expectEqual(watchRowDetailSeverityKey(row, texts), "action",
              "changed row severity");

  row.changed = false;
  row.baselineDelta = "0x0001";
  expectEqual(watchRowDetailSeverityKey(row, texts), "warning",
              "baseline drift severity");

  row.baselineDelta = "Match";
  row.startupDelta = "Diff";
  expectEqual(watchRowDetailSeverityKey(row, texts), "error",
              "startup drift severity");

  row.startupDelta = "Match";
  row.value.clear();
  expectEqual(watchRowDetailSeverityKey(row, texts), "warning",
              "missing value severity");
}

void testSelectedRowState() {
  const WatchStartupWatchRow row = readyRow();
  const WatchRowDetailState state =
      buildWatchRowDetailState(row, englishTexts());

  expectEqual(state.severityKey, "ok", "selected severity");
  expectEqual(state.displayValue, "Operation enabled", "display value");
  expectEqual(state.evidence, "Stable evidence", "evidence label");
  expectTrue(state.cia402, "selected row is CiA 402");
  expectFalse(state.baselineDrift, "selected row has no baseline drift");
  expectFalse(state.startupDrift, "selected row has no startup drift");
  expectEqual(state.text,
              "#3 0x6041:0x00 | uint16 | Value: Operation enabled | Baseline: "
              "Match | Startup: Match | Stable evidence",
              "summary text");
  expectContains(state.tooltipLines, "Selected Watch row", "tooltip title");
  expectContains(state.tooltipLines, "Slave: #3", "tooltip slave");
  expectContains(state.tooltipLines, "Object: 0x6041:0x00", "tooltip object");
  expectContains(state.tooltipLines, "Changed: No", "tooltip changed");
  expectContains(state.tooltipLines, "Drive Evidence: CiA 402 candidate",
                 "tooltip drive evidence");
  expectContains(state.tooltipLines, "Local boundary.", "tooltip boundary");
  expectEqual(state.tooltip, state.tooltipLines.join('\n'), "tooltip text");
}

void testFallbackState() {
  WatchStartupWatchRow row;
  row.row = 8;
  const WatchRowDetailState state =
      buildWatchRowDetailState(row, englishTexts());

  expectEqual(state.severityKey, "warning", "empty row warns");
  expectTrue(state.missingValue, "empty row is missing value");
  expectEqual(state.displayValue, "Empty", "fallback value");
  expectEqual(state.text,
              "#8 ----:-- | Type? | Value: Empty | Baseline: No baseline | "
              "Startup: No comparison | Stable evidence",
              "fallback summary");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testEmptyStates();
  testSeverityRules();
  testSelectedRowState();
  testFallbackState();
  return 0;
}
