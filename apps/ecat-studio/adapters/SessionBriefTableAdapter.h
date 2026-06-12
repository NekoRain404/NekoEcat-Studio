#pragma once

// Populates and queries the session brief QTableWidget.


#include <QString>

class QTableWidget;

inline constexpr int kSessionBriefAreaColumn = 0;
inline constexpr int kSessionBriefStatusColumn = 1;
inline constexpr int kSessionBriefEvidenceColumn = 2;
inline constexpr int kSessionBriefNextColumn = 3;

// Snapshot of a session brief row: area, status, evidence, next action, and routing key.
struct SessionBriefTableRow {
  int row = -1;
  QString area;
  QString status;
  QString evidence;
  QString next;
  QString actionKey;
  QString firstTooltip;
};

// Stores the action routing key on all cells in the row.
void setSessionBriefActionKey(QTableWidget *table, int row,
                              const QString &actionKey);
// Retrieves the stored action key with legacy fallback.
QString sessionBriefActionKeyForRow(QTableWidget *table, int row);
// First non-empty tooltip in the row for status bar hints.
QString sessionBriefFirstTooltipForRow(QTableWidget *table, int row);
// Extracts all columns into a structured row model.
SessionBriefTableRow sessionBriefTableRowFromTable(QTableWidget *table,
                                                   int row);
