#include "ui_state/SdoTargetTrailDetailUiState.h"

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

SdoTargetTrailDetailTexts englishTexts() {
  return {
      .unavailableText = "SDO target trail evidence is not available.",
      .unavailableTip = "Preview is local.",
      .noSelectionText = "Select an SDO target trail row.",
      .noSelectionTip = "Selection is local.",
      .timeFallback = "Time?",
      .typeFallback = "type?",
      .unknownSource = "Unknown",
      .noValue = "No value",
      .noWriteValue = "No write value",
      .restoreTarget = "Restore target locally",
      .readyForReuse = "Ready for Watch, Bookmark, or Startup",
      .watchBookmarkOnly =
          "Can seed Watch or Bookmark; Startup needs writable value",
      .missingAddress = "Missing target address",
      .noSavedValue = "No saved value; fill target only",
      .summaryPattern =
          "%1 | #%2 %3:%4 %5 | Source: %6 | Value: %7 | Write: %8 | %9",
      .selectedTitle = "Selected SDO target trail row",
      .timeLabel = "Time",
      .slaveLabel = "Slave",
      .objectLabel = "Object",
      .typeLabel = "Type",
      .sourceLabel = "Source",
      .valueLabel = "Value",
      .writeValueLabel = "Write Value",
      .startupCandidateLabel = "Startup Candidate",
      .detailLabel = "Detail",
      .reuseLabel = "Reuse",
      .localBoundary = "Local boundary.",
      .executionBoundary = "Execution boundary.",
  };
}

SdoTargetTrailRow readyRow() {
  SdoTargetTrailRow row;
  row.row = 1;
  row.time = "12:00";
  row.positionText = "3";
  row.position = 3;
  row.positionValid = true;
  row.index = "0x6040";
  row.subIndex = "0x00";
  row.type = "uint16";
  row.source = "Object Dictionary";
  row.value = "0x0006";
  row.writeValue = "0x000F";
  row.detail = "writable";
  return row;
}

void testEmptyStates() {
  SdoTargetTrailDetailUiState state =
      sdoTargetTrailDetailUnavailableState(englishTexts());
  expectEqual(state.text, "SDO target trail evidence is not available.",
              "unavailable text");
  expectEqual(state.severityKey, "neutral", "unavailable severity");
  expectEqual(state.tooltip, "Preview is local.", "unavailable tooltip");

  state = sdoTargetTrailDetailNoSelectionState(englishTexts());
  expectEqual(state.text, "Select an SDO target trail row.",
              "no selection text");
  expectEqual(state.severityKey, "neutral", "no selection severity");
  expectEqual(state.tooltip, "Selection is local.", "no selection tooltip");
}

void testSeverityRules() {
  const SdoTargetTrailDetailTexts texts = englishTexts();
  SdoTargetTrailRow row = readyRow();
  expectEqual(sdoTargetTrailDetailSeverityKey(row, true, texts), "ok",
              "startup-capable row is ok");
  expectEqual(sdoTargetTrailDetailSeverityKey(row, false, texts), "action",
              "valued non-startup row is action");

  row.value.clear();
  row.writeValue.clear();
  expectEqual(sdoTargetTrailDetailSeverityKey(row, false, texts), "neutral",
              "target without saved value is neutral");

  row.positionValid = false;
  expectEqual(sdoTargetTrailDetailSeverityKey(row, false, texts), "warning",
              "missing target is warning");
}

void testSelectedRowState() {
  const SdoTargetTrailRow row = readyRow();
  const SdoTargetTrailDetailUiState state =
      buildSdoTargetTrailDetailUiState(row, true, englishTexts());

  expectEqual(state.severityKey, "ok", "selected severity");
  expectEqual(state.reuse, "Ready for Watch, Bookmark, or Startup",
              "reuse text");
  expectEqual(state.startupValue, "0x000F", "startup value prefers write");
  expectTrue(state.hasTarget, "selected trail has target");
  expectTrue(state.canStartup, "selected trail can startup");
  expectTrue(state.hasAnyValue, "selected trail has value");
  expectEqual(state.text,
              "12:00 | #3 0x6040:0x00 uint16 | Source: Object Dictionary | "
              "Value: 0x0006 | Write: 0x000F | Ready for Watch, Bookmark, or "
              "Startup",
              "summary text");
  expectContains(state.tooltipLines, "Selected SDO target trail row",
                 "tooltip title");
  expectContains(state.tooltipLines, "Slave: #3", "tooltip slave");
  expectContains(state.tooltipLines, "Object: 0x6040:0x00", "tooltip object");
  expectContains(state.tooltipLines, "Startup Candidate: 0x000F",
                 "tooltip startup value");
  expectContains(state.tooltipLines,
                 "Reuse: Ready for Watch, Bookmark, or Startup",
                 "tooltip reuse");
  expectEqual(state.tooltip, state.tooltipLines.join('\n'), "tooltip text");
}

void testFallbackState() {
  SdoTargetTrailRow row;
  row.row = 0;
  row.positionText = "4";
  row.position = 4;
  row.positionValid = true;
  row.index = "0x6060";
  row.subIndex = "0x00";
  row.value = "8";
  const SdoTargetTrailDetailUiState state =
      buildSdoTargetTrailDetailUiState(row, false, englishTexts());

  expectEqual(state.severityKey, "action", "value without startup is action");
  expectFalse(state.canStartup, "fallback row cannot startup");
  expectEqual(state.startupValue, "8", "startup fallback uses value");
  expectEqual(state.reuse,
              "Can seed Watch or Bookmark; Startup needs writable value",
              "fallback reuse");
  expectEqual(state.text,
              "Time? | #4 0x6060:0x00 type? | Source: Unknown | Value: 8 | "
              "Write: No write value | Can seed Watch or Bookmark; Startup "
              "needs writable value",
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
