// Unit tests for ObjectBookmarkDetailUiState.
#include "ui_state/ObjectBookmarkDetailUiState.h"

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

ObjectBookmarkDetailTexts englishTexts() {
  return {
      .unavailableText = "Object bookmark evidence is not available.",
      .unavailableTip = "Preview is local.",
      .noSelectionText = "Select an object bookmark.",
      .noSelectionTip = "Selection is local.",
      .readOnlyText = "只读",
      .typeFallback = "type?",
      .accessFallback = "Access?",
      .unnamed = "Unnamed",
      .noValue = "No value",
      .projectSource = "Project",
      .fillTarget = "Fill target locally",
      .readyForWatchStartup = "Ready for Watch or Startup",
      .readOnlyWatchOnly = "Read-only evidence; Watch only",
      .missingAddress = "Missing bookmark address",
      .noSavedValue = "No saved value; fill target only",
      .summaryPattern = "#%1 %2:%3 %4 | %5 | %6 | Last: %7 | Source: %8 | %9",
      .selectedTitle = "Selected object bookmark",
      .slaveLabel = "Slave",
      .objectLabel = "Object",
      .accessLabel = "Access",
      .typeLabel = "Type",
      .bitsLabel = "Bits",
      .nameLabel = "Name",
      .lastValueLabel = "Last Value",
      .sourceLabel = "Source",
      .reuseLabel = "Reuse",
      .localBoundary = "Local boundary.",
      .executionBoundary = "Execution boundary.",
  };
}

SdoObjectBookmarkRow readyRow() {
  SdoObjectBookmarkRow row;
  row.row = 0;
  row.positionText = "3";
  row.position = 3;
  row.positionValid = true;
  row.slaveName = "Drive A";
  row.index = "0x6040";
  row.subIndex = "0x00";
  row.access = "rw";
  row.type = "uint16";
  row.bits = "16";
  row.name = "Controlword";
  row.lastValue = "0x0006";
  row.source = "Object Dictionary";
  return row;
}

void testEmptyStates() {
  ObjectBookmarkDetailUiState state =
      objectBookmarkDetailUnavailableState(englishTexts());
  expectEqual(state.text, "Object bookmark evidence is not available.",
              "unavailable text");
  expectEqual(state.severityKey, "neutral", "unavailable severity");
  expectEqual(state.tooltip, "Preview is local.", "unavailable tooltip");

  state = objectBookmarkDetailNoSelectionState(englishTexts());
  expectEqual(state.text, "Select an object bookmark.", "no selection text");
  expectEqual(state.severityKey, "neutral", "no selection severity");
  expectEqual(state.tooltip, "Selection is local.", "no selection tooltip");
}

void testSeverityRules() {
  const ObjectBookmarkDetailTexts texts = englishTexts();
  SdoObjectBookmarkRow row = readyRow();
  expectEqual(objectBookmarkDetailSeverityKey(row, texts), "ok",
              "writable bookmark with value is ok");

  row.access = "ro";
  expectEqual(objectBookmarkDetailSeverityKey(row, texts), "action",
              "read-only value is action");

  row.access = "rw";
  row.lastValue.clear();
  expectEqual(objectBookmarkDetailSeverityKey(row, texts), "neutral",
              "target without value is neutral");

  row.positionValid = false;
  expectEqual(objectBookmarkDetailSeverityKey(row, texts), "warning",
              "missing target is warning");
}

void testSelectedRowState() {
  const SdoObjectBookmarkRow row = readyRow();
  const ObjectBookmarkDetailUiState state =
      buildObjectBookmarkDetailUiState(row, englishTexts());

  expectEqual(state.severityKey, "ok", "selected severity");
  expectEqual(state.reuse, "Ready for Watch or Startup", "reuse text");
  expectTrue(state.hasTarget, "selected bookmark has target");
  expectTrue(state.hasValue, "selected bookmark has value");
  expectFalse(state.readOnly, "selected bookmark is writable");
  expectEqual(state.text,
              "#3 0x6040:0x00 uint16 | rw | Controlword | Last: 0x0006 | "
              "Source: Object Dictionary | Ready for Watch or Startup",
              "summary text");
  expectContains(state.tooltipLines, "Selected object bookmark",
                 "tooltip title");
  expectContains(state.tooltipLines, "Slave: #3 Drive A", "tooltip slave");
  expectContains(state.tooltipLines, "Object: 0x6040:0x00", "tooltip object");
  expectContains(state.tooltipLines, "Reuse: Ready for Watch or Startup",
                 "tooltip reuse");
  expectContains(state.tooltipLines, "Local boundary.", "tooltip boundary");
  expectEqual(state.tooltip, state.tooltipLines.join('\n'), "tooltip text");
}

void testReadOnlyAndFallbackState() {
  SdoObjectBookmarkRow row;
  row.row = 1;
  row.positionText = "4";
  row.position = 4;
  row.positionValid = true;
  row.slaveName = "Drive B";
  row.index = "0x6041";
  row.subIndex = "0x00";
  row.access = "只读";
  row.lastValue = "0x0027";
  ObjectBookmarkDetailUiState state =
      buildObjectBookmarkDetailUiState(row, englishTexts());

  expectEqual(state.severityKey, "action", "localized read-only action");
  expectTrue(state.readOnly, "localized read-only detected");
  expectEqual(state.reuse, "Read-only evidence; Watch only", "read-only reuse");
  expectEqual(state.text,
              "#4 0x6041:0x00 type? | 只读 | Drive B | Last: 0x0027 | "
              "Source: Project | Read-only evidence; Watch only",
              "read-only summary");

  row.positionValid = false;
  row.lastValue.clear();
  state = buildObjectBookmarkDetailUiState(row, englishTexts());
  expectEqual(state.severityKey, "warning", "missing target warning");
  expectFalse(state.hasTarget, "missing target flag");
  expectEqual(state.reuse, "Missing bookmark address", "missing target reuse");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testEmptyStates();
  testSeverityRules();
  testSelectedRowState();
  testReadOnlyAndFallbackState();
  return 0;
}
