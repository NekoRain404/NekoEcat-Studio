#include "StudioTableHelpers.h"

#include "StudioTextHelpers.h"

#include <algorithm>

#include <QAbstractItemView>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QModelIndexList>
#include <QTableWidget>
#include <QTextStream>
#include <QtGlobal>

QString csvCell(QString text) {
  text.replace('"', "\"\"");
  text.replace('\r', " ");
  text.replace('\n', " ");
  return QString("\"%1\"").arg(text);
}

QString markdownCell(QString text) {
  text.replace('\\', "\\\\");
  text.replace('|', "\\|");
  text.replace('\r', " ");
  text.replace('\n', "<br>");
  return text;
}

QString tableText(QTableWidget *table, int row, int column) {
  if (!table || row < 0 || row >= table->rowCount() || column < 0 ||
      column >= table->columnCount()) {
    return QString();
  }
  const auto *item = table->item(row, column);
  return item ? item->text().trimmed() : QString();
}

int tableRowForPosition(QTableWidget *table, int position, int positionColumn) {
  if (!table || position < 0) {
    return -1;
  }

  for (int row = 0; row < table->rowCount(); ++row) {
    bool ok = false;
    const int rowPosition = tableText(table, row, positionColumn).toInt(&ok);
    if (ok && rowPosition == position) {
      return row;
    }
  }
  return -1;
}

bool tableObjectIndexMatches(QTableWidget *table, int row, const QString &index,
                             const QString &subIndex, int indexColumn,
                             int subIndexColumn) {
  if (!table || row < 0 || row >= table->rowCount() ||
      index.trimmed().isEmpty() || subIndex.trimmed().isEmpty()) {
    return false;
  }

  return normalizeHexText(tableText(table, row, indexColumn), 4) ==
             normalizeHexText(index, 4) &&
         normalizeHexText(tableText(table, row, subIndexColumn), 2) ==
             normalizeHexText(subIndex, 2);
}

bool tableObjectAddressMatches(QTableWidget *table, int row, int position,
                               const QString &index, const QString &subIndex,
                               int positionColumn, int indexColumn,
                               int subIndexColumn) {
  if (!table || position < 0) {
    return false;
  }

  bool ok = false;
  const int rowPosition = tableText(table, row, positionColumn).toInt(&ok);
  return ok && rowPosition == position &&
         tableObjectIndexMatches(table, row, index, subIndex, indexColumn,
                                 subIndexColumn);
}

int tableRowForObjectIndex(QTableWidget *table, const QString &index,
                           const QString &subIndex, int indexColumn,
                           int subIndexColumn) {
  if (!table || index.trimmed().isEmpty() || subIndex.trimmed().isEmpty()) {
    return -1;
  }

  for (int row = 0; row < table->rowCount(); ++row) {
    if (tableObjectIndexMatches(table, row, index, subIndex, indexColumn,
                                subIndexColumn)) {
      return row;
    }
  }
  return -1;
}

int tableRowForObjectAddress(QTableWidget *table, int position,
                             const QString &index, const QString &subIndex,
                             int positionColumn, int indexColumn,
                             int subIndexColumn) {
  if (!table || position < 0 || index.trimmed().isEmpty() ||
      subIndex.trimmed().isEmpty()) {
    return -1;
  }

  for (int row = 0; row < table->rowCount(); ++row) {
    if (tableObjectAddressMatches(table, row, position, index, subIndex,
                                  positionColumn, indexColumn,
                                  subIndexColumn)) {
      return row;
    }
  }
  return -1;
}

int firstVisibleTableRow(QTableWidget *table) {
  if (!table) {
    return -1;
  }

  for (int row = 0; row < table->rowCount(); ++row) {
    if (!table->isRowHidden(row)) {
      return row;
    }
  }
  return -1;
}

QVector<int> selectedTableRows(QTableWidget *table, bool visibleOnly,
                               bool includeCurrentFallback) {
  QVector<int> rows;
  if (!table) {
    return rows;
  }

  if (table->selectionModel()) {
    const QModelIndexList selected = table->selectionModel()->selectedRows();
    rows.reserve(selected.size());
    for (const auto &index : selected) {
      const int row = index.row();
      if (!index.isValid() || row < 0 || row >= table->rowCount()) {
        continue;
      }
      if (visibleOnly && table->isRowHidden(row)) {
        continue;
      }
      rows.append(row);
    }
  }

  const int current = table->currentRow();
  if (rows.isEmpty() && includeCurrentFallback && current >= 0 &&
      current < table->rowCount() &&
      (!visibleOnly || !table->isRowHidden(current))) {
    rows.append(current);
  }

  std::sort(rows.begin(), rows.end());
  rows.erase(std::unique(rows.begin(), rows.end()), rows.end());
  return rows;
}

QVector<int> visibleTableRows(QTableWidget *table) {
  QVector<int> rows;
  if (!table) {
    return rows;
  }
  rows.reserve(table->rowCount());
  for (int row = 0; row < table->rowCount(); ++row) {
    if (!table->isRowHidden(row)) {
      rows.append(row);
    }
  }
  return rows;
}

QList<QStringList> copyTableRows(QTableWidget *table) {
  QList<QStringList> rows;
  if (!table) {
    return rows;
  }
  for (int row = 0; row < table->rowCount(); ++row) {
    QStringList values;
    for (int column = 0; column < table->columnCount(); ++column) {
      values << (table->item(row, column) ? table->item(row, column)->text()
                                          : QString());
    }
    rows.append(values);
  }
  return rows;
}

bool selectAndFocusTableRow(QTableWidget *table, int row, int column) {
  if (!table || row < 0 || row >= table->rowCount()) {
    return false;
  }

  const int safeColumn =
      column >= 0 && column < table->columnCount() ? column : 0;
  table->selectRow(row);
  table->setCurrentCell(row, safeColumn);
  if (auto *item = table->item(row, safeColumn)) {
    table->scrollToItem(item, QAbstractItemView::PositionAtCenter);
  }
  table->setFocus();
  return true;
}

void fitTableColumnsToViewport(QTableWidget *table, int stretchColumn) {
  if (!table || table->columnCount() <= 0) {
    return;
  }

  auto *header = table->horizontalHeader();
  if (!header) {
    return;
  }

  header->setStretchLastSection(false);
  for (int column = 0; column < table->columnCount(); ++column) {
    header->setSectionResizeMode(column, QHeaderView::ResizeToContents);
  }

  const int safeStretchColumn =
      stretchColumn >= 0 && stretchColumn < table->columnCount()
          ? stretchColumn
          : table->columnCount() - 1;
  header->setSectionResizeMode(safeStretchColumn, QHeaderView::Stretch);
}

void writeMarkdownTable(QTextStream &out, QTableWidget *table) {
  if (!table || table->columnCount() <= 0) {
    out << "_No data._\n\n";
    return;
  }

  out << "|";
  for (int column = 0; column < table->columnCount(); ++column) {
    const auto *header = table->horizontalHeaderItem(column);
    out << " "
        << markdownCell(header ? header->text()
                               : QString("Column %1").arg(column + 1))
        << " |";
  }
  out << "\n|";
  for (int column = 0; column < table->columnCount(); ++column) {
    out << " --- |";
  }
  out << "\n";

  for (int row = 0; row < table->rowCount(); ++row) {
    out << "|";
    for (int column = 0; column < table->columnCount(); ++column) {
      const auto *item = table->item(row, column);
      out << " " << markdownCell(item ? item->text() : QString()) << " |";
    }
    out << "\n";
  }
  out << "\n";
}
