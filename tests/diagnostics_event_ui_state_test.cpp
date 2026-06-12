#include "ui_state/DiagnosticsEventUiState.h"

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

DiagnosticsEventTexts englishTexts() {
  return {
      .timeHeader = "Time",
      .levelHeader = "Level",
      .sourceHeader = "Source",
      .messageHeader = "Message",
      .noDiagnostics = "No diagnostics",
      .shown = "shown",
      .errorLabel = "Error",
      .warningLabel = "Warning",
      .infoLabel = "Info",
  };
}

void testHeadersAndColorKeys() {
  const QStringList headers = diagnosticsEventHeaders(englishTexts());
  expectEqual(headers.size(), 4, "diagnostics header count");
  expectEqual(headers.at(0), "Time", "time header");
  expectEqual(headers.at(3), "Message", "message header");
  expectEqual(diagnosticsEventColorKey("Error"), "error", "error color key");
  expectEqual(diagnosticsEventColorKey("Warning"), "warning",
              "warning color key");
  expectEqual(diagnosticsEventColorKey("Info"), "info", "info color key");
  expectEqual(diagnosticsEventColorKey("Debug"), "info", "fallback color key");
}

void testEmptySummary() {
  const DiagnosticsEventSummary summary =
      diagnosticsEventSummary({}, englishTexts());
  expectEqual(summary.total, 0, "empty total");
  expectEqual(summary.visible, 0, "empty visible");
  expectEqual(summary.text, "No diagnostics", "empty summary");
}

void testSummaryCountsVisibleAndHiddenRows() {
  const QList<DiagnosticsEventRowState> rows = {
      {.level = "Error", .visible = true},
      {.level = "Warning", .visible = false},
      {.level = "Info", .visible = true},
      {.level = "Debug", .visible = true},
  };
  const DiagnosticsEventSummary summary =
      diagnosticsEventSummary(rows, englishTexts());
  expectEqual(summary.total, 4, "summary total");
  expectEqual(summary.visible, 3, "summary visible");
  expectEqual(summary.errors, 1, "summary errors");
  expectEqual(summary.warnings, 1, "summary warnings");
  expectEqual(summary.infos, 1, "summary infos");
  expectEqual(summary.text, "3/4 shown   Error: 1   Warning: 1   Info: 1",
              "summary text");
}

void testLevelCountsIgnoreUnknownLevels() {
  const DiagnosticsEventSummary counts =
      diagnosticsEventCounts({"Error", "Warning", "Info", "Debug", "Error"});
  expectEqual(counts.total, 5, "counts total");
  expectEqual(counts.visible, 5, "counts visible");
  expectEqual(counts.errors, 2, "counts errors");
  expectEqual(counts.warnings, 1, "counts warnings");
  expectEqual(counts.infos, 1, "counts infos");
  expectEqual(counts.text, QString(), "counts text remains empty");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testHeadersAndColorKeys();
  testEmptySummary();
  testSummaryCountsVisibleAndHiddenRows();
  testLevelCountsIgnoreUnknownLevels();
  return 0;
}
