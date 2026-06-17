// Populates and queries the session brief QTableWidget.
#include "SessionBriefTableAdapter.h"

#include "utils/TableHelpers.h"

#include <QTableWidget>

namespace {

constexpr int kSessionBriefActionKeyRole = Qt::UserRole + 33;

} // namespace

// Stores the action routing key on every cell in the row for later retrieval during navigation.
void setSessionBriefActionKey(QTableWidget *table, int row,
                              const QString &actionKey) {
  if (!table || row < 0 || row >= table->rowCount()) {
    return;
  }
    // Iterate over collection
  for (int column = 0; column < table->columnCount(); ++column) {
    if (auto *item = table->item(row, column)) {
      item->setData(kSessionBriefActionKeyRole, actionKey);
      item->setData(Qt::UserRole, actionKey);
    }
  }
}

// Retrieves the stored action key, checking both current and legacy data roles for backwards compatibility.
QString sessionBriefActionKeyForRow(QTableWidget *table, int row) {
  if (!table || row < 0 || row >= table->rowCount()) {
    return QString();
  }
    // Iterate over collection
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

// Finds the first non-empty tooltip across a row for the status bar hover hint.
QString sessionBriefFirstTooltipForRow(QTableWidget *table, int row) {
  if (!table || row < 0 || row >= table->rowCount()) {
    return QString();
  }
    // Iterate over collection
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

// Extracts all session brief columns into a structured row for the overview panel.
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
