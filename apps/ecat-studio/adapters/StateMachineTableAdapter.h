#pragma once

// Populates and queries the state machine recommendation QTableWidget.


#include <QString>

class QTableWidget;

inline constexpr int kStateMachinePositionColumn = 0;
inline constexpr int kStateMachineNameColumn = 1;
inline constexpr int kStateMachineCurrentColumn = 2;
inline constexpr int kStateMachineRecommendedColumn = 3;
inline constexpr int kStateMachineEvidenceColumn = 4;
inline constexpr int kStateMachineDriveColumn = 5;
inline constexpr int kStateMachineStartupColumn = 6;
inline constexpr int kStateMachineProcessColumn = 7;
inline constexpr int kStateMachineRiskColumn = 8;
inline constexpr int kStateMachineActionColumn = 9;

struct StateMachineTableRow {
  int row = -1;
  QString position;
  QString name;
  QString current;
  QString recommended;
  QString evidence;
  QString drive;
  QString startup;
  QString process;
  QString risk;
  QString action;
};

StateMachineTableRow stateMachineTableRowFromTable(QTableWidget *table,
                                                   int row);
bool stateMachineTableRowPosition(const StateMachineTableRow &row,
                                  int *position);
bool stateMachineTableRowHasRecommendation(const StateMachineTableRow &row);
int stateMachinePositionFromTable(QTableWidget *table, int row);
bool stateMachineRowHasRecommendation(QTableWidget *table, int row);
