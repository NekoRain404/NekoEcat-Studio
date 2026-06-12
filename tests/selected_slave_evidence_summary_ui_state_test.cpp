// Unit tests for SelectedSlaveEvidenceSummaryUiState.
#include "ui_state/SelectedSlaveEvidenceSummaryUiState.h"

#include <QCoreApplication>

#include <cstdlib>
#include <iostream>

namespace {

void fail(const QString &message) {
  std::cerr << message.toStdString() << '\n';
  std::exit(1);
}

void expectEqual(int actual, int expected, const QString &message) {
  if (actual != expected) {
    fail(QString("%1: expected %2, got %3")
             .arg(message)
             .arg(expected)
             .arg(actual));
  }
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

SelectedSlaveEvidenceSummaryTexts englishTexts() {
  return {
      .selectSlaveText = "Evidence: select a slave",
      .ready = "ready",
      .missing = "missing",
      .summaryPattern =
          "Evidence %1/5 | ID %2 | OD %3 | PDO %4 | Watch %5/%6 values | "
          "Startup %7 diff %8 | Free %9 map issue %10",
      .scorePattern = "Selected slave evidence score: %1/5",
      .missingIdentity = "Missing identity evidence",
      .missingOd = "Missing OD evidence",
      .missingPdo = "Missing PDO evidence",
      .missingWatch = "Missing Watch values",
      .missingProcess = "Missing process evidence",
      .startupDiffPattern = "Startup SDO has %1 mismatch row(s)",
      .mapIssuePattern = "Free Run has %1 map issue(s)",
      .topologyIssuePattern = "Topology baseline has %1 issue(s)",
  };
}

SlaveEvidenceInput readyInput() {
  SlaveEvidenceInput input;
  input.position = 2;
  input.identityRows = 1;
  input.odRows = 12;
  input.pdoRows = 8;
  input.watchRows = 3;
  input.watchValueRows = 3;
  input.startupRows = 2;
  input.processRows = 4;
  return input;
}

void testNoSelectionState() {
  const SelectedSlaveEvidenceSummaryUiState state =
      selectedSlaveEvidenceNoSelectionState(englishTexts());
  expectEqual(state.text, "Evidence: select a slave", "no selection text");
  expectEqual(state.severityKey, "neutral", "no selection severity");
  expectEqual(state.tooltip, QString(), "no selection tooltip");
}

void testSummaryAndSeverityRules() {
  const SelectedSlaveEvidenceSummaryTexts texts = englishTexts();
  SlaveEvidenceInput input = readyInput();
  expectEqual(selectedSlaveEvidenceSummaryGroupCount(input), 5,
              "ready evidence groups");
  expectEqual(selectedSlaveEvidenceSummarySeverityKey(input, 0), "ok",
              "ready severity");

  SelectedSlaveEvidenceSummaryUiState state =
      buildSelectedSlaveEvidenceSummaryUiState(input, 0, texts);
  expectEqual(state.evidenceGroups, 5, "state evidence groups");
  expectEqual(
      state.text,
      "Evidence 5/5 | ID ready | OD 12 | PDO 8 | Watch 3/3 values | Startup 2 "
      "diff 0 | Free 4 map issue 0",
      "ready summary text");
  expectEqual(state.tooltipLines.first(), "Selected slave evidence score: 5/5",
              "score tooltip");
  expectEqual(state.tooltip, state.tooltipLines.join('\n'), "tooltip text");

  input.startupDiffs = 1;
  expectEqual(selectedSlaveEvidenceSummarySeverityKey(input, 0), "warning",
              "startup diff severity");
  state = buildSelectedSlaveEvidenceSummaryUiState(input, 2, texts);
  expectEqual(state.severityKey, "warning", "warning state severity");
  expectContains(state.tooltipLines, "Startup SDO has 1 mismatch row(s)",
                 "startup diff tooltip");
  expectContains(state.tooltipLines, "Topology baseline has 2 issue(s)",
                 "topology tooltip");
}

void testMissingEvidenceFallbacks() {
  const SelectedSlaveEvidenceSummaryTexts texts = englishTexts();
  SlaveEvidenceInput input;
  input.position = 3;
  input.odRows = 4;
  input.watchRows = 2;

  const SelectedSlaveEvidenceSummaryUiState state =
      buildSelectedSlaveEvidenceSummaryUiState(input, 0, texts);
  expectEqual(state.evidenceGroups, 1, "partial evidence groups");
  expectEqual(state.severityKey, "neutral", "partial severity");
  expectEqual(
      state.text,
      "Evidence 1/5 | ID missing | OD 4 | PDO 0 | Watch 0/2 values | Startup "
      "0 diff 0 | Free 0 map issue 0",
      "partial summary text");
  expectContains(state.tooltipLines, "Missing identity evidence",
                 "missing identity tooltip");
  expectContains(state.tooltipLines, "Missing PDO evidence",
                 "missing PDO tooltip");
  expectContains(state.tooltipLines, "Missing Watch values",
                 "missing Watch tooltip");
  expectContains(state.tooltipLines, "Missing process evidence",
                 "missing process tooltip");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testNoSelectionState();
  testSummaryAndSeverityRules();
  testMissingEvidenceFallbacks();
  return 0;
}
