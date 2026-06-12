// Unit tests for StartupSdoRowDetailUiState.
#include "ui_state/StartupSdoRowDetailUiState.h"

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

StartupSdoRowDetailTexts englishTexts() {
  return {
      .unavailableText = "Startup SDO evidence is not available.",
      .unavailableTip = "Preview is local.",
      .noSelectionText = "Select a visible Startup SDO row.",
      .noSelectionTip = "Selection is local.",
      .defaultType = "default type",
      .emptyValue = "Empty",
      .pendingStatus = "Pending",
      .noWatchValue = "No value",
      .watchMismatch = "Watch mismatch",
      .noWatchEvidence = "No Watch evidence",
      .pendingComparison = "Pending comparison",
      .watchMatches = "Watch matches",
      .reviewRow = "Review row",
      .summaryPattern =
          "Row %1 | #%2 %3:%4 | %5 = %6 | Status: %7 | Watch: %8 | %9",
      .selectedTitle = "Selected Startup SDO row",
      .rowLabel = "Row",
      .slaveLabel = "Slave",
      .objectLabel = "Object",
      .valueLabel = "Value",
      .typeLabel = "Type",
      .statusLabel = "Status",
      .detailLabel = "Detail",
      .watchValueLabel = "Watch Value",
      .watchDeltaLabel = "Watch Delta",
      .localBoundary = "Local boundary.",
      .executionBoundary = "Execution boundary.",
  };
}

WatchStartupStartupRow readyRow() {
  WatchStartupStartupRow row;
  row.row = 1;
  row.position = 3;
  row.positionText = "3";
  row.index = "0x6040";
  row.subIndex = "0x00";
  row.value = "0x0006";
  row.type = "uint16";
  row.status = "Synced";
  row.detail = "Created from Watch";
  row.watchValue = "0x0006";
  row.watchDelta = "match";
  return row;
}

void testEmptyStates() {
  StartupSdoRowDetailUiState state =
      startupSdoRowDetailUnavailableState(englishTexts());
  expectEqual(state.text, "Startup SDO evidence is not available.",
              "unavailable text");
  expectEqual(state.severityKey, "neutral", "unavailable severity");
  expectEqual(state.tooltip, "Preview is local.", "unavailable tooltip");

  state = startupSdoRowDetailNoSelectionState(englishTexts());
  expectEqual(state.text, "Select a visible Startup SDO row.",
              "no selection text");
  expectEqual(state.severityKey, "neutral", "no selection severity");
  expectEqual(state.tooltip, "Selection is local.", "no selection tooltip");
}

void testSeverityRules() {
  const StartupSdoRowDetailTexts texts = englishTexts();
  WatchStartupStartupRow row = readyRow();
  expectEqual(startupSdoRowDetailSeverityKey(row, texts), "ok",
              "matching row is ok");

  row.watchDelta = "diff";
  expectEqual(startupSdoRowDetailSeverityKey(row, texts), "error",
              "watch diff is error");

  row.watchDelta = "match";
  row.status = "Preflight Error";
  expectEqual(startupSdoRowDetailSeverityKey(row, texts), "error",
              "validation issue is error");

  row.status = "Applying";
  expectEqual(startupSdoRowDetailSeverityKey(row, texts), "action",
              "applying row is action");

  row.status = "Synced";
  row.watchDelta = "pending";
  expectEqual(startupSdoRowDetailSeverityKey(row, texts), "warning",
              "pending comparison is warning");

  row.watchDelta = "match";
  row.value.clear();
  expectEqual(startupSdoRowDetailSeverityKey(row, texts), "warning",
              "missing target value is warning");
}

void testSelectedRowState() {
  const WatchStartupStartupRow row = readyRow();
  const StartupSdoRowDetailUiState state =
      buildStartupSdoRowDetailUiState(row, englishTexts());

  expectEqual(state.severityKey, "ok", "selected severity");
  expectEqual(state.evidence, "Watch matches", "selected evidence");
  expectTrue(state.match, "selected row matches watch");
  expectFalse(state.watchDiff, "selected row has no watch diff");
  expectFalse(state.missingTarget, "selected row has target");
  expectEqual(state.text,
              "Row 2 | #3 0x6040:0x00 | uint16 = 0x0006 | Status: Synced | "
              "Watch: 0x0006 | Watch matches",
              "summary text");
  expectContains(state.tooltipLines, "Selected Startup SDO row",
                 "tooltip title");
  expectContains(state.tooltipLines, "Slave: #3", "tooltip slave");
  expectContains(state.tooltipLines, "Object: 0x6040:0x00", "tooltip object");
  expectContains(state.tooltipLines, "Watch Delta: match",
                 "tooltip watch delta");
  expectContains(state.tooltipLines, "Local boundary.", "tooltip boundary");
  expectEqual(state.tooltip, state.tooltipLines.join('\n'), "tooltip text");
}

void testLocalizedAndFallbackState() {
  WatchStartupStartupRow row;
  row.row = 0;
  row.position = 4;
  row.index = "0x6060";
  row.subIndex = "0x00";
  row.status = "校验中";
  row.watchDelta = "无监视";
  StartupSdoRowDetailUiState state =
      buildStartupSdoRowDetailUiState(row, englishTexts());

  expectEqual(state.severityKey, "warning", "localized no watch warning");
  expectTrue(state.applying, "localized applying is detected");
  expectTrue(state.noWatch, "localized no-watch is detected");
  expectTrue(state.missingTarget, "missing value is detected");
  expectEqual(state.evidence, "No Watch evidence", "localized evidence");
  expectEqual(state.text,
              "Row 1 | #4 0x6060:0x00 | default type = Empty | Status: 校验中 "
              "| Watch: No value | No Watch evidence",
              "fallback summary");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testEmptyStates();
  testSeverityRules();
  testSelectedRowState();
  testLocalizedAndFallbackState();
  return 0;
}
