#include "SdoHistoryRowDetailUiState.h"

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

SdoHistoryRowDetailTexts englishTexts() {
  return {
      .unavailableText = "SDO history evidence is not available.",
      .unavailableTip = "Preview is local.",
      .noSelectionText = "Select an SDO history row.",
      .noSelectionTip = "Selection is local.",
      .timeFallback = "Time?",
      .actionFallback = "Action?",
      .typeFallback = "type?",
      .noValue = "No value",
      .noStatus = "No status",
      .fillTargetOnly = "Fill target only",
      .reusableValue = "Can seed Watch or Startup",
      .reviewFailure = "Review failure before reuse",
      .waitingRuntime = "Waiting for runtime result",
      .summaryPattern = "%1 %2 | #%3 %4:%5 %6 | Value: %7 | Status: %8 | %9",
      .selectedTitle = "Selected SDO history row",
      .timeLabel = "Time",
      .actionLabel = "Action",
      .slaveLabel = "Slave",
      .objectLabel = "Object",
      .typeLabel = "Type",
      .valueLabel = "Value",
      .statusLabel = "Status",
      .detailLabel = "Detail",
      .reuseLabel = "Reuse",
      .localBoundary = "Local boundary.",
      .executionBoundary = "Execution boundary.",
  };
}

SdoHistoryRow readyRow() {
  SdoHistoryRow row;
  row.row = 2;
  row.time = "12:00";
  row.action = "Read";
  row.positionText = "3";
  row.position = 3;
  row.positionValid = true;
  row.index = "0x6041";
  row.subIndex = "0x00";
  row.type = "uint16";
  row.value = "0x0027";
  row.status = "Complete";
  row.detail = "Object Dictionary";
  return row;
}

void testEmptyStates() {
  SdoHistoryRowDetailUiState state =
      sdoHistoryRowDetailUnavailableState(englishTexts());
  expectEqual(state.text, "SDO history evidence is not available.",
              "unavailable text");
  expectEqual(state.severityKey, "neutral", "unavailable severity");
  expectEqual(state.tooltip, "Preview is local.", "unavailable tooltip");

  state = sdoHistoryRowDetailNoSelectionState(englishTexts());
  expectEqual(state.text, "Select an SDO history row.", "no selection text");
  expectEqual(state.severityKey, "neutral", "no selection severity");
  expectEqual(state.tooltip, "Selection is local.", "no selection tooltip");
}

void testSeverityRules() {
  const SdoHistoryRowDetailTexts texts = englishTexts();
  SdoHistoryRow row = readyRow();
  expectEqual(sdoHistoryRowDetailSeverityKey(row, texts), "ok",
              "completed read is ok");

  row.status = "Failed";
  expectEqual(sdoHistoryRowDetailSeverityKey(row, texts), "error",
              "failed history is error");

  row.status = "Requested";
  expectEqual(sdoHistoryRowDetailSeverityKey(row, texts), "warning",
              "requested history is warning");

  row.status = "Complete";
  row.action = "Write";
  expectEqual(sdoHistoryRowDetailSeverityKey(row, texts), "action",
              "write history is action");

  row.action = "Read";
  row.positionValid = false;
  expectEqual(sdoHistoryRowDetailSeverityKey(row, texts), "warning",
              "missing target is warning");
}

void testSelectedRowState() {
  const SdoHistoryRow row = readyRow();
  const SdoHistoryRowDetailUiState state =
      buildSdoHistoryRowDetailUiState(row, englishTexts());

  expectEqual(state.severityKey, "ok", "selected severity");
  expectEqual(state.reuse, "Can seed Watch or Startup", "reuse text");
  expectTrue(state.hasReusableValue, "complete value is reusable");
  expectTrue(state.readAction, "read action detected");
  expectFalse(state.failed, "selected row is not failed");
  expectEqual(state.text,
              "12:00 Read | #3 0x6041:0x00 uint16 | Value: 0x0027 | Status: "
              "Complete | Can seed Watch or Startup",
              "summary text");
  expectContains(state.tooltipLines, "Selected SDO history row",
                 "tooltip title");
  expectContains(state.tooltipLines, "Slave: #3", "tooltip slave");
  expectContains(state.tooltipLines, "Object: 0x6041:0x00", "tooltip object");
  expectContains(state.tooltipLines, "Reuse: Can seed Watch or Startup",
                 "tooltip reuse");
  expectContains(state.tooltipLines, "Local boundary.", "tooltip boundary");
  expectEqual(state.tooltip, state.tooltipLines.join('\n'), "tooltip text");
}

void testLocalizedAndFallbackState() {
  SdoHistoryRow row;
  row.row = 0;
  row.positionText = "4";
  row.position = 4;
  row.positionValid = true;
  row.index = "0x6060";
  row.subIndex = "0x00";
  row.action = "校验";
  row.status = "已请求";
  const SdoHistoryRowDetailUiState state =
      buildSdoHistoryRowDetailUiState(row, englishTexts());

  expectEqual(state.severityKey, "warning", "localized requested warning");
  expectTrue(state.requested, "localized requested is detected");
  expectTrue(state.verifyAction, "localized verify is detected");
  expectEqual(state.reuse, "Waiting for runtime result", "waiting reuse");
  expectEqual(state.text,
              "Time? 校验 | #4 0x6060:0x00 type? | Value: No value | Status: "
              "已请求 | Waiting for runtime result",
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
