#pragma once

// Populates and queries the session brief QTableWidget.


#include <QString>

class QTableWidget;

inline constexpr int kSessionBriefAreaColumn = 0;
inline constexpr int kSessionBriefStatusColumn = 1;
inline constexpr int kSessionBriefEvidenceColumn = 2;
inline constexpr int kSessionBriefNextColumn = 3;

struct SessionBriefTableRow {
  int row = -1;
  QString area;
  QString status;
  QString evidence;
  QString next;
  QString actionKey;
  QString firstTooltip;
};

void setSessionBriefActionKey(QTableWidget *table, int row,
                              const QString &actionKey);
QString sessionBriefActionKeyForRow(QTableWidget *table, int row);
QString sessionBriefFirstTooltipForRow(QTableWidget *table, int row);
SessionBriefTableRow sessionBriefTableRowFromTable(QTableWidget *table,
                                                   int row);
