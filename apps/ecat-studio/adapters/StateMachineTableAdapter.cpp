// Populates and queries the state machine recommendation QTableWidget.
#include "StateMachineTableAdapter.h"

#include "utils/TableHelpers.h"

#include <QTableWidget>

// Extracts all state machine columns into a structured row for recommendation display.
StateMachineTableRow stateMachineTableRowFromTable(QTableWidget *table,
                                                   int row) {
  StateMachineTableRow result;
  result.row = row;
  if (!table || row < 0 || row >= table->rowCount()) {
    return result;
  }

  result.position = tableText(table, row, kStateMachinePositionColumn);
  result.name = tableText(table, row, kStateMachineNameColumn);
  result.current = tableText(table, row, kStateMachineCurrentColumn);
  result.recommended = tableText(table, row, kStateMachineRecommendedColumn);
  result.evidence = tableText(table, row, kStateMachineEvidenceColumn);
  result.drive = tableText(table, row, kStateMachineDriveColumn);
  result.startup = tableText(table, row, kStateMachineStartupColumn);
  result.process = tableText(table, row, kStateMachineProcessColumn);
  result.risk = tableText(table, row, kStateMachineRiskColumn);
  result.action = tableText(table, row, kStateMachineActionColumn);
  return result;
}

// Parses the position text and outputs it via out-parameter; returns false if non-numeric.
bool stateMachineTableRowPosition(const StateMachineTableRow &row,
                                  int *position) {
  bool ok = false;
  const int parsed = row.position.toInt(&ok);
  if (!ok) {
    return false;
  }
  if (position) {
    *position = parsed;
  }
  return true;
}

// Whether this row has a non-empty recommended state transition.
bool stateMachineTableRowHasRecommendation(const StateMachineTableRow &row) {
  return !row.recommended.trimmed().isEmpty();
}

// Convenience wrapper to extract the slave position from a table row.
int stateMachinePositionFromTable(QTableWidget *table, int row) {
  int position = -1;
  if (stateMachineTableRowPosition(stateMachineTableRowFromTable(table, row),
                                   &position)) {
    return position;
  }
  return -1;
}

// Convenience wrapper to check for a recommendation directly from table indices.
bool stateMachineRowHasRecommendation(QTableWidget *table, int row) {
  return stateMachineTableRowHasRecommendation(
      stateMachineTableRowFromTable(table, row));
}
