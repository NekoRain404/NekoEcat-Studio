// Unit tests for StateMachineTableAdapter.
#include "adapters/StateMachineTableAdapter.h"

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

void setCell(QTableWidget *table, int row, int column, const QString &text) {
  table->setItem(row, column, new QTableWidgetItem(text));
}

void initStateMachineTable(QTableWidget *table) {
  table->setColumnCount(10);
  table->setRowCount(2);

  setCell(table, 0, kStateMachinePositionColumn, "3");
  setCell(table, 0, kStateMachineNameColumn, "Drive A");
  setCell(table, 0, kStateMachineCurrentColumn, "PREOP");
  setCell(table, 0, kStateMachineRecommendedColumn, "SAFEOP");
  setCell(table, 0, kStateMachineEvidenceColumn, "OD loaded");
  setCell(table, 0, kStateMachineDriveColumn, "Statusword ok");
  setCell(table, 0, kStateMachineStartupColumn, "1 row, 0 diffs");
  setCell(table, 0, kStateMachineProcessColumn, "PDO 4");
  setCell(table, 0, kStateMachineRiskColumn, "Watch missing");
  setCell(table, 0, kStateMachineActionColumn, "Send SAFEOP");

  setCell(table, 1, kStateMachinePositionColumn, "not-a-position");
  setCell(table, 1, kStateMachineNameColumn, "Coupler");
  setCell(table, 1, kStateMachineCurrentColumn, "OP");
  setCell(table, 1, kStateMachineRecommendedColumn, " ");
}

void testStructuredRowExtraction() {
  QTableWidget table;
  initStateMachineTable(&table);

  const StateMachineTableRow row = stateMachineTableRowFromTable(&table, 0);
  expectEqual(row.row, 0, "state-machine row index");
  expectEqual(row.position, "3", "state-machine position text");
  expectEqual(row.name, "Drive A", "state-machine name");
  expectEqual(row.current, "PREOP", "state-machine current state");
  expectEqual(row.recommended, "SAFEOP", "state-machine recommendation");
  expectEqual(row.evidence, "OD loaded", "state-machine evidence");
  expectEqual(row.drive, "Statusword ok", "state-machine drive");
  expectEqual(row.startup, "1 row, 0 diffs", "state-machine startup");
  expectEqual(row.process, "PDO 4", "state-machine process");
  expectEqual(row.risk, "Watch missing", "state-machine risk");
  expectEqual(row.action, "Send SAFEOP", "state-machine action");

  int position = -1;
  expectTrue(stateMachineTableRowPosition(row, &position),
             "valid position parses");
  expectEqual(position, 3, "parsed position");
  expectEqual(stateMachinePositionFromTable(&table, 0), 3,
              "position from table");
  expectTrue(stateMachineTableRowHasRecommendation(row),
             "recommendation from row");
  expectTrue(stateMachineRowHasRecommendation(&table, 0),
             "recommendation from table");
}

void testInvalidRowsAndFallbacks() {
  QTableWidget table;
  initStateMachineTable(&table);

  const StateMachineTableRow invalid =
      stateMachineTableRowFromTable(&table, -1);
  expectEqual(invalid.row, -1, "invalid row index");
  expectEqual(invalid.position, QString(), "invalid row position");
  expectEqual(stateMachinePositionFromTable(&table, -1), -1,
              "invalid table position");
  expectFalse(stateMachineRowHasRecommendation(&table, -1),
              "invalid recommendation");

  const StateMachineTableRow row = stateMachineTableRowFromTable(&table, 1);
  int position = -1;
  expectFalse(stateMachineTableRowPosition(row, &position),
              "invalid position fails");
  expectEqual(position, -1, "invalid position untouched");
  expectFalse(stateMachineTableRowHasRecommendation(row),
              "blank recommendation");
}

} // namespace

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  testStructuredRowExtraction();
  testInvalidRowsAndFallbacks();
  return 0;
}
