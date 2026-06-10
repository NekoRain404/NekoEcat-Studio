#include "SessionBriefTableAdapter.h"

#include "StudioTableHelpers.h"

#include <QTableWidget>

namespace {

constexpr int kSessionBriefActionKeyRole = Qt::UserRole + 33;

} // namespace

void setSessionBriefActionKey(QTableWidget *table, int row,
                              const QString &actionKey) {
  if (!table || row < 0 || row >= table->rowCount()) {
    return;
  }
  for (int column = 0; column < table->columnCount(); ++column) {
    if (auto *item = table->item(row, column)) {
      item->setData(kSessionBriefActionKeyRole, actionKey);
      item->setData(Qt::UserRole, actionKey);
    }
  }
}

QString sessionBriefActionKeyForRow(QTableWidget *table, int row) {
  if (!table || row < 0 || row >= table->rowCount()) {
    return QString();
  }
  for (int column = 0; column < table->columnCount(); ++column) {
    if (auto *item = table->item(row, column)) {
      const QString stableKey =
          item->data(kSessionBriefActionKeyRole).toString();
      if (!stableKey.isEmpty()) {
        return stableKey;
      }
      const QString legacyKey = item->data(Qt::UserRole).toString();
      if (!legacyKey.isEmpty()) {
        return legacyKey;
      }
    }
  }
  return QString();
}

QString sessionBriefFirstTooltipForRow(QTableWidget *table, int row) {
  if (!table || row < 0 || row >= table->rowCount()) {
    return QString();
  }
  for (int column = 0; column < table->columnCount(); ++column) {
    if (auto *item = table->item(row, column)) {
      const QString tooltip = item->toolTip().trimmed();
      if (!tooltip.isEmpty()) {
        return tooltip;
      }
    }
  }
  return QString();
}

SessionBriefTableRow sessionBriefTableRowFromTable(QTableWidget *table,
                                                   int row) {
  SessionBriefTableRow result;
  result.row = row;
  if (!table || row < 0 || row >= table->rowCount()) {
    return result;
  }

  result.area = tableText(table, row, kSessionBriefAreaColumn);
  result.status = tableText(table, row, kSessionBriefStatusColumn);
  result.evidence = tableText(table, row, kSessionBriefEvidenceColumn);
  result.next = tableText(table, row, kSessionBriefNextColumn);
  result.actionKey = sessionBriefActionKeyForRow(table, row);
  result.firstTooltip = sessionBriefFirstTooltipForRow(table, row);
  return result;
}
