// SelectedDriveSummaryDetailTest — Tests for SelectedDriveSummaryDetail
//
// Test coverage:
//   - Fallback states for missing Watch and CiA 402 evidence
//   - Operation-enabled summary text and severity
//   - Error code evidence display
//   - Controlword recommendation logic

#include "detail/SelectedDriveSummaryDetail.h"

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
    fail(QString("%1: expected '%2', got '%3'").arg(message, expected, actual));
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

SelectedDriveSummaryTexts englishTexts() {
  return {
      .noWatchEvidence = "Drive: no Watch evidence",
      .noCia402Evidence = "Drive: no CiA 402 Watch evidence",
      .summaryPattern = "Drive: %1",
      .modePattern = "mode %1",
      .controlwordPattern = "cw %1",
  };
}

WatchStartupWatchRow watchRow(int position, const QString &index,
                              const QString &value, const QString &decoded) {
  WatchStartupWatchRow row;
  row.position = position;
  row.index = index;
  row.value = value;
  row.decoded = decoded;
  return row;
}

// Test fallback text and severity when Watch or CiA 402 evidence is missing
void testNoWatchAndNoCia402Fallbacks() {
  const SelectedDriveSummaryTexts texts = englishTexts();
  SelectedDriveSummaryDetail state = selectedDriveNoWatchEvidenceState(texts);
  expectEqual(state.text, "Drive: no Watch evidence", "no Watch text");
  expectEqual(state.severityKey, "neutral", "no Watch severity");

  state = buildSelectedDriveSummaryDetail(
      {watchRow(2, "0x2000", "1", "Vendor object")}, 2, texts);
  expectEqual(state.text, "Drive: no CiA 402 Watch evidence",
              "no CiA 402 text");
  expectEqual(state.severityKey, "neutral", "no CiA 402 severity");
  expectEqual(state.parts.size(), 0, "no CiA 402 parts");
}

// Test summary text when drive is in operation-enabled state
void testOperationEnabledSummary() {
  const QVector<WatchStartupWatchRow> rows = {
      watchRow(2, "0x6041", "0x0037", "Operation enabled (0x0037, voltage)"),
      watchRow(2, "0x6061", "8", "Cyclic sync position"),
      watchRow(2, "0x6040", "0x000f", "enable operation (0x000f)"),
      watchRow(3, "0x6041", "0x0008", "Fault (0x0008)"),
  };

  const SelectedDriveSummaryDetail state =
      buildSelectedDriveSummaryDetail(rows, 2, englishTexts());
  expectEqual(
      state.text,
      "Drive: Operation enabled (0x0037, voltage) | mode Cyclic sync position "
      "| cw enable operation (0x000f)",
      "operation enabled summary text");
  expectEqual(state.severityKey, "ok", "operation enabled severity");
  expectEqual(state.parts.size(), 3, "operation enabled part count");
}

// Test error code display and severity in summary
void testErrorCodeEvidence() {
  const SelectedDriveSummaryTexts texts = englishTexts();
  SelectedDriveSummaryDetail state = buildSelectedDriveSummaryDetail(
      {watchRow(2, "0x603f", "0", "No error")}, 2, texts);
  expectEqual(state.text, "Drive: no CiA 402 Watch evidence",
              "zero error code is not surfaced");

  state = buildSelectedDriveSummaryDetail(
      {watchRow(2, "0x603f", "0x2310", "Error code 0x2310")}, 2, texts);
  expectEqual(state.text, "Drive: Error code 0x2310",
              "non-zero error code summary text");
  expectEqual(state.severityKey, "error", "non-zero error severity");
}

// Test controlword recommendation based on statusword
void testRecommendedControlword() {
  Cia402ControlwordRecommendation recommendation =
      selectedDriveControlwordRecommendation(
          {watchRow(2, "0x6041", "0x0040", "Switch on disabled (0x0040)")}, 2);
  expectEqual(recommendation.label, "Shutdown", "recommended label");
  expectEqual(recommendation.value, "0x0006", "recommended value");
  expectEqual(recommendation.reason, "Switch on disabled (0x0040)",
              "recommended reason");

  recommendation = selectedDriveControlwordRecommendation(
      {watchRow(2, "0x6041", "0x0027", "Operation enabled (0x0027)")}, 2);
  expectEqual(recommendation.label, QString(), "no recommendation label");
  expectEqual(recommendation.value, QString(), "no recommendation value");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testNoWatchAndNoCia402Fallbacks();
  testOperationEnabledSummary();
  testErrorCodeEvidence();
  testRecommendedControlword();
  return 0;
}
