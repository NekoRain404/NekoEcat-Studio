#include "StateMachineTableAdapter.h"

#include "StudioTableHelpers.h"

#include <QTableWidget>

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

bool stateMachineTableRowHasRecommendation(const StateMachineTableRow &row) {
  return !row.recommended.trimmed().isEmpty();
}

int stateMachinePositionFromTable(QTableWidget *table, int row) {
  int position = -1;
  if (stateMachineTableRowPosition(stateMachineTableRowFromTable(table, row),
                                   &position)) {
    return position;
  }
  return -1;
}

bool stateMachineRowHasRecommendation(QTableWidget *table, int row) {
  return stateMachineTableRowHasRecommendation(
      stateMachineTableRowFromTable(table, row));
}
