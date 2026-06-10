#include "IoVariableFilterModel.h"

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

IoVariableTableRow baseRow() {
  IoVariableTableRow row;
  row.position = 3;
  row.positionValid = true;
  row.direction = "Rx Output";
  row.symbol = "Drive.Controlword";
  row.index = "0x6040";
  row.subIndex = "0x00";
  row.bits = "16";
  row.source = "Process | uint16";
  row.raw = "0x0006";
  row.plcQuality = "Ready";
  return row;
}

void testRowStateAndScope() {
  IoVariableTableRow row = baseRow();
  const IoVariableFilterRowState state =
      ioVariableFilterRowState(row, 3, "Ready");
  expectTrue(state.selected, "selected position detected");
  expectTrue(state.process, "process source detected");
  expectTrue(state.rx, "Rx scope detected");
  expectTrue(state.cia402, "CiA 402 scope detected");
  expectFalse(state.plcIssue, "ready PLC row is not an issue");

  expectTrue(ioVariableFilterScopeMatches(state, kIoVariableScopeSelected),
             "selected scope matches");
  expectTrue(ioVariableFilterScopeMatches(state, kIoVariableScopeProcess),
             "process scope matches");
  expectTrue(ioVariableFilterScopeMatches(state, kIoVariableScopeRx),
             "Rx scope matches");
  expectFalse(ioVariableFilterScopeMatches(state, kIoVariableScopePdo),
              "PDO scope excludes process-backed rows");

  row.source = "PDO | uint16";
  const IoVariableFilterRowState pdoState =
      ioVariableFilterRowState(row, 3, "Ready");
  expectTrue(ioVariableFilterScopeMatches(pdoState, kIoVariableScopePdo),
             "PDO scope matches non-process PDO rows");
}

void testSearchAndDecision() {
  const IoVariableTableRow row = baseRow();
  expectTrue(
      ioVariableFilterTextMatches({"Drive.Controlword", "0x6040"}, "control"),
      "search matches any cell case-insensitively");
  expectFalse(
      ioVariableFilterTextMatches({"Drive.Controlword", "0x6040"}, "status"),
      "search rejects missing text");

  IoVariableFilterDecision decision =
      evaluateIoVariableFilterRow(row, {"Drive.Controlword", "0x6040"},
                                  kIoVariableScopeProcess, "drive", 3, "Ready");
  expectTrue(decision.visible, "scope and search match");

  decision =
      evaluateIoVariableFilterRow(row, {"Drive.Controlword", "0x6040"},
                                  kIoVariableScopeTx, "drive", 3, "Ready");
  expectFalse(decision.visible, "scope mismatch hides row");

  decision = evaluateIoVariableFilterRow(row, {"Drive.Controlword", "0x6040"},
                                         kIoVariableScopeProcess, "status", 3,
                                         "Ready");
  expectFalse(decision.visible, "search mismatch hides row");
}

void testStatsAndSummary() {
  IoVariableFilterStats stats;

  IoVariableTableRow process = baseRow();
  IoVariableFilterDecision decision =
      evaluateIoVariableFilterRow(process, {"Drive.Controlword"},
                                  kIoVariableScopeAll, QString(), 3, "Ready");
  accumulateIoVariableFilterStats(&stats, decision);

  IoVariableTableRow issue = baseRow();
  issue.position = 4;
  issue.direction = "Tx Input";
  issue.source = "Watch";
  issue.raw.clear();
  issue.watch.clear();
  issue.startup = "Diff";
  issue.changed = "Yes";
  issue.plcQuality = "Missing Alias";
  decision = evaluateIoVariableFilterRow(issue, {"Axis.Statusword"},
                                         kIoVariableScopeStartupDiff, QString(),
                                         3, "Ready");
  accumulateIoVariableFilterStats(&stats, decision);

  expectEqual(stats.total, 2, "total rows");
  expectEqual(stats.visible, 2, "visible rows");
  expectEqual(stats.processRows, 1, "process rows");
  expectEqual(stats.watchRows, 1, "watch rows");
  expectEqual(stats.startupDiffs, 1, "startup diff rows");
  expectEqual(stats.missingValues, 1, "missing value rows");
  expectEqual(stats.changedRows, 1, "changed rows");
  expectEqual(stats.plcIssues, 1, "PLC issue rows");
  expectEqual(ioVariableFilterSummaryText(
                  stats, "All",
                  "%1/%2 | %3 | process %4 | watch %5 | startup diff %6 | "
                  "missing %7 | changed %8 | plc issues %9"),
              "2/2 | All | process 1 | watch 1 | startup diff 1 | missing 1 | "
              "changed 1 | plc issues 1",
              "summary text");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testRowStateAndScope();
  testSearchAndDecision();
  testStatsAndSummary();
  return 0;
}
