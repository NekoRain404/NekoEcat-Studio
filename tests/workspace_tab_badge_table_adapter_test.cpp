#include "WorkspaceTabBadgeTableAdapter.h"

#include "ConsistencyTableAdapter.h"
#include "ProcessDataTableAdapter.h"
#include "SlaveEvidenceTableAdapter.h"
#include "WatchStartupTableAdapter.h"

#include <QApplication>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <cstdlib>
#include <iostream>

namespace {

void fail(const QString &message) {
  std::cerr << message.toStdString() << '\n';
  std::exit(1);
}

void expectTrue(bool condition, const QString &message) {
  if (!condition) {
    fail(message);
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

void setCell(QTableWidget *table, int row, int column, const QString &text) {
  table->setItem(row, column, new QTableWidgetItem(text));
}

void initTable(QTableWidget *table, int rows, int columns) {
  table->setColumnCount(columns);
  table->setRowCount(rows);
}

void testSimpleIssueClassifiers() {
  expectTrue(!workspaceTabBadgeWatchDeltaIsIssue("match"),
             "watch match is not an issue");
  expectTrue(!workspaceTabBadgeWatchDeltaIsIssue("待比较"),
             "watch pending zh is not an issue");
  expectTrue(workspaceTabBadgeWatchDeltaIsIssue("diff"),
             "watch diff is an issue");
}

void testWorkspaceTabBadgeCountsFromTables() {
  QTableWidget watch;
  initTable(&watch, 3, 12);
  setCell(&watch, 0, kWatchStartupWatchPositionColumn, "1");
  setCell(&watch, 0, kWatchStartupWatchIndexColumn, "0x6040");
  setCell(&watch, 0, kWatchStartupWatchSubIndexColumn, "0x00");
  setCell(&watch, 0, kWatchStartupWatchValueColumn, "0x0006");
  setCell(&watch, 0, kWorkspaceTabBadgeWatchDeltaColumn, "match");
  setCell(&watch, 1, kWatchStartupWatchPositionColumn, "1");
  setCell(&watch, 1, kWatchStartupWatchIndexColumn, "0x6060");
  setCell(&watch, 1, kWatchStartupWatchSubIndexColumn, "0x00");
  setCell(&watch, 1, kWatchStartupWatchValueColumn, "8");
  setCell(&watch, 1, kWorkspaceTabBadgeWatchDeltaColumn, "diff");
  setCell(&watch, 2, kWatchStartupWatchPositionColumn, "1");
  setCell(&watch, 2, kWorkspaceTabBadgeWatchDeltaColumn, "待比较");

  QTableWidget startup;
  initTable(&startup, 3, 9);
  setCell(&startup, 0, kWatchStartupStartupPositionColumn, "1");
  setCell(&startup, 0, kWatchStartupStartupIndexColumn, "0x6040");
  setCell(&startup, 0, kWatchStartupStartupSubIndexColumn, "0x00");
  setCell(&startup, 0, kWatchStartupStartupValueColumn, "0x0006");
  setCell(&startup, 0, kWatchStartupStartupTypeColumn, "uint16");
  setCell(&startup, 1, kWatchStartupStartupPositionColumn, "1");
  setCell(&startup, 1, kWatchStartupStartupIndexColumn, "0x6060");
  setCell(&startup, 1, kWatchStartupStartupSubIndexColumn, "0x00");
  setCell(&startup, 1, kWatchStartupStartupValueColumn, "9");
  setCell(&startup, 1, kWatchStartupStartupTypeColumn, "uint8");
  setCell(&startup, 2, kWatchStartupStartupPositionColumn, "2");
  setCell(&startup, 2, kWatchStartupStartupIndexColumn, "0x6040");
  setCell(&startup, 2, kWatchStartupStartupSubIndexColumn, "0x00");
  setCell(&startup, 2, kWatchStartupStartupValueColumn, "0x0006");

  QTableWidget freeRun;
  initTable(&freeRun, 4, 2);

  QTableWidget io;
  initTable(&io, 5, 16);
  setCell(&io, 0, kIoVariableRawColumn, "0x01");
  setCell(&io, 0, kIoVariableWatchColumn, "0x01");
  setCell(&io, 0, kIoVariableStartupColumn, "match");
  setCell(&io, 0, kIoVariableMapColumn, "mapped");
  setCell(&io, 0, kIoVariablePlcQualityColumn, "Ready");
  setCell(&io, 1, kIoVariableStartupColumn, "diff");
  setCell(&io, 2, kIoVariableMapColumn, "missing pdo map");
  setCell(&io, 3, kIoVariablePlcQualityColumn, "Alias missing");
  setCell(&io, 4, kIoVariableMapColumn, "mapped");

  QTableWidget consistency;
  initTable(&consistency, 4, 1);
  setCell(&consistency, 0, kConsistencyLevelColumn, "Error");
  setCell(&consistency, 1, kConsistencyLevelColumn, "Warning");
  setCell(&consistency, 2, kConsistencyLevelColumn, "Ready");
  setCell(&consistency, 3, kConsistencyLevelColumn, "Info");

  QTableWidget stateMachine;
  initTable(&stateMachine, 3, 9);
  setCell(&stateMachine, 0, kWorkspaceTabBadgeStateRiskColumn, "");
  setCell(&stateMachine, 1, kWorkspaceTabBadgeStateRiskColumn, "Needs PREOP");
  setCell(&stateMachine, 2, kWorkspaceTabBadgeStateRiskColumn, "Fault");

  QTableWidget diagnostics;
  initTable(&diagnostics, 4, 2);
  setCell(&diagnostics, 0, kWorkspaceTabBadgeDiagnosticsLevelColumn, "Error");
  setCell(&diagnostics, 1, kWorkspaceTabBadgeDiagnosticsLevelColumn, "Warning");
  setCell(&diagnostics, 2, kWorkspaceTabBadgeDiagnosticsLevelColumn, "Info");
  setCell(&diagnostics, 3, kWorkspaceTabBadgeDiagnosticsLevelColumn, "Debug");

  QTableWidget matrix;
  initTable(&matrix, 4, 12);
  setCell(&matrix, 0, kSlaveEvidenceMatrixPriorityColumn, "P0 Fault");
  setCell(&matrix, 1, kSlaveEvidenceMatrixPriorityColumn, "P1 Risk");
  setCell(&matrix, 2, kSlaveEvidenceMatrixPriorityColumn, "P2 Action");
  setCell(&matrix, 3, kSlaveEvidenceMatrixPriorityColumn, "P3 Ready");

  const WorkspaceTabBadgeCounts counts =
      workspaceTabBadgeCounts({.watchTable = &watch,
                               .startupSdoTable = &startup,
                               .freeRunEntryTable = &freeRun,
                               .ioVariableTable = &io,
                               .consistencyTable = &consistency,
                               .stateMachineTable = &stateMachine,
                               .diagnosticsTable = &diagnostics,
                               .slaveEvidenceMatrixTable = &matrix});

  expectEqual(counts.watchRows, 3, "watch rows");
  expectEqual(counts.watchStartupDiffs, 1, "watch startup diff badges");
  expectEqual(counts.startupRows, 3, "startup rows");
  expectEqual(counts.startupDiffs, 1, "startup diff rows");
  expectEqual(counts.freeRunRows, 4, "free run rows");
  expectEqual(counts.ioRows, 5, "I/O rows");
  expectEqual(counts.ioIssues, 4, "I/O issues");
  expectEqual(counts.consistencyRows, 4, "consistency rows");
  expectEqual(counts.consistencyErrors, 1, "consistency errors");
  expectEqual(counts.consistencyWarnings, 1, "consistency warnings");
  expectEqual(counts.consistencyInfos, 1, "consistency infos");
  expectEqual(counts.consistencyReady, 1, "consistency ready");
  expectEqual(counts.stateRiskRows, 2, "state risk rows");
  expectEqual(counts.diagnosticRows, 4, "diagnostic rows");
  expectEqual(counts.diagnosticErrors, 1, "diagnostic errors");
  expectEqual(counts.diagnosticWarnings, 1, "diagnostic warnings");
  expectEqual(counts.diagnosticInfos, 1, "diagnostic infos");
  expectEqual(counts.matrixP0, 1, "matrix P0");
  expectEqual(counts.matrixP1, 1, "matrix P1");
  expectEqual(counts.matrixP2, 1, "matrix P2");
  expectEqual(counts.matrixP3, 1, "matrix P3");
}

} // namespace

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  testSimpleIssueClassifiers();
  testWorkspaceTabBadgeCountsFromTables();
  return 0;
}
