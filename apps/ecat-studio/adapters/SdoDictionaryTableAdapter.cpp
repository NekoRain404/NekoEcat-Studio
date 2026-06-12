#include "SdoDictionaryTableAdapter.h"

#include "helpers/StudioTableHelpers.h"
#include "helpers/StudioTextHelpers.h"

#include <QTableWidget>

SdoDictionaryRow sdoDictionaryRowFromTable(QTableWidget *table, int row) {
  SdoDictionaryRow result;
  result.row = row;
  if (!table || row < 0 || row >= table->rowCount()) {
    return result;
  }

  result.object = tableText(table, row, 0);
  result.index = normalizeHexText(tableText(table, row, 1), 4);
  result.subIndex = normalizeHexText(tableText(table, row, 2), 2);
  result.access = tableText(table, row, 3);
  result.type = tableText(table, row, 4);
  result.bits = tableText(table, row, 5);
  result.name = tableText(table, row, 6);
  result.value = tableText(table, row, 7);
  result.status = tableText(table, row, 8);
  return result;
}

bool sdoDictionaryRowHasTarget(const SdoDictionaryRow &row) {
  return !row.index.isEmpty() && !row.subIndex.isEmpty();
}

bool sdoDictionaryRowHasValue(const SdoDictionaryRow &row) {
  return !row.value.isEmpty();
}

bool sdoDictionaryRowIsWritable(const SdoDictionaryRow &row) {
  return row.access.toLower().contains('w');
}

QVector<int> visibleSdoDictionaryRows(QTableWidget *table) {
  return visibleTableRows(table);
}

QVector<int> failedSdoDictionaryRows(QTableWidget *table) {
  QVector<int> rows;
  if (!table) {
    return rows;
  }
  rows.reserve(table->rowCount());
  for (int row = 0; row < table->rowCount(); ++row) {
    const QString status = sdoDictionaryRowFromTable(table, row).status;
    if (status.contains(QStringLiteral("failed"), Qt::CaseInsensitive) ||
        status.contains(QStringLiteral("失败"))) {
      rows.append(row);
    }
  }
  return rows;
}

bool sdoDictionaryRowsContainValue(QTableWidget *table,
                                   const QVector<int> &rows) {
  if (!table) {
    return false;
  }
  for (const int row : rows) {
    if (row < 0 || row >= table->rowCount() || table->isRowHidden(row)) {
      continue;
    }
    if (sdoDictionaryRowHasValue(sdoDictionaryRowFromTable(table, row))) {
      return true;
    }
  }
  return false;
}

SdoDictionaryRow sdoDictionaryRowForTarget(QTableWidget *table,
                                           const QString &index,
                                           const QString &subIndex) {
  const int row = tableRowForObjectIndex(table, index, subIndex, 1, 2);
  return sdoDictionaryRowFromTable(table, row);
}

QVector<SdoDictionaryReadObject>
sdoDictionaryReadObjectsFromRows(QTableWidget *table, const QVector<int> &rows,
                                 int *skipped) {
  if (skipped) {
    *skipped = 0;
  }

  QVector<SdoDictionaryReadObject> objects;
  if (!table) {
    if (skipped) {
      *skipped = rows.size();
    }
    return objects;
  }

  for (const int row : rows) {
    if (row < 0 || row >= table->rowCount() || table->isRowHidden(row)) {
      if (skipped) {
        ++(*skipped);
      }
      continue;
    }

    const SdoDictionaryRow dictionary = sdoDictionaryRowFromTable(table, row);
    if (!sdoDictionaryRowHasTarget(dictionary)) {
      if (skipped) {
        ++(*skipped);
      }
      continue;
    }
    objects.append(
        {row, dictionary.index, dictionary.subIndex, dictionary.type});
  }
  return objects;
}
