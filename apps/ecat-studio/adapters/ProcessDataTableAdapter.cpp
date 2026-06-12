#include "ProcessDataTableAdapter.h"

#include "models/ProcessDataRowModel.h"
#include "helpers/StudioTableHelpers.h"
#include "helpers/StudioTextHelpers.h"

#include <QTableWidget>

PdoMapTableRow pdoMapTableRowFromTable(QTableWidget *table, int row) {
  PdoMapTableRow result;
  result.row = row;
  if (!table || row < 0 || row >= table->rowCount()) {
    return result;
  }

  result.syncManager = tableText(table, row, kPdoMapSyncManagerColumn);
  result.pdo = tableText(table, row, kPdoMapPdoColumn);
  result.index = normalizeHexText(tableText(table, row, kPdoMapIndexColumn), 4);
  result.subIndex =
      normalizeHexText(tableText(table, row, kPdoMapSubIndexColumn), 2);
  result.bits = tableText(table, row, kPdoMapBitsColumn);
  result.name = tableText(table, row, kPdoMapNameColumn);
  return result;
}

FreeRunEntryTableRow freeRunEntryTableRowFromTable(QTableWidget *table,
                                                   int row) {
  FreeRunEntryTableRow result;
  result.row = row;
  if (!table || row < 0 || row >= table->rowCount()) {
    return result;
  }

  result.position = tableText(table, row, kFreeRunEntryPositionColumn)
                        .toInt(&result.positionValid);
  result.syncManager = tableText(table, row, kFreeRunEntrySyncManagerColumn);
  result.direction = tableText(table, row, kFreeRunEntryDirectionColumn);
  result.pdo = tableText(table, row, kFreeRunEntryPdoColumn);
  result.index =
      normalizeHexText(tableText(table, row, kFreeRunEntryIndexColumn), 4);
  result.subIndex =
      normalizeHexText(tableText(table, row, kFreeRunEntrySubIndexColumn), 2);
  result.bits = tableText(table, row, kFreeRunEntryBitsColumn);
  result.offset = tableText(table, row, kFreeRunEntryOffsetColumn);
  result.bit = tableText(table, row, kFreeRunEntryBitColumn);
  result.name = tableText(table, row, kFreeRunEntryNameColumn);
  result.raw = tableText(table, row, kFreeRunEntryRawColumn);
  result.decoded = tableText(table, row, kFreeRunEntryDecodedColumn);
  result.meaning = tableText(table, row, kFreeRunEntryMeaningColumn);
  result.mapStatus = tableText(table, row, kFreeRunEntryMapStatusColumn);
  result.mapDetail = tableText(table, row, kFreeRunEntryMapDetailColumn);
  const auto *changedItem = table->item(row, kFreeRunEntryPositionColumn);
  result.changed = changedItem && changedItem->data(Qt::UserRole).toBool();
  return result;
}

IoVariableTableRow ioVariableTableRowFromTable(QTableWidget *table, int row) {
  IoVariableTableRow result;
  result.row = row;
  if (!table || row < 0 || row >= table->rowCount()) {
    return result;
  }

  result.position = tableText(table, row, kIoVariablePositionColumn)
                        .toInt(&result.positionValid);
  result.direction = tableText(table, row, kIoVariableDirectionColumn);
  result.symbol = tableText(table, row, kIoVariableSymbolColumn);
  result.index =
      normalizeHexText(tableText(table, row, kIoVariableIndexColumn), 4);
  result.subIndex =
      normalizeHexText(tableText(table, row, kIoVariableSubIndexColumn), 2);
  result.bits = tableText(table, row, kIoVariableBitsColumn);
  result.pdo = tableText(table, row, kIoVariablePdoColumn);
  result.source = tableText(table, row, kIoVariableSourceColumn);
  result.raw = tableText(table, row, kIoVariableRawColumn);
  result.decoded = tableText(table, row, kIoVariableDecodedColumn);
  result.meaning = tableText(table, row, kIoVariableMeaningColumn);
  result.watch = tableText(table, row, kIoVariableWatchColumn);
  result.startup = tableText(table, row, kIoVariableStartupColumn);
  result.map = tableText(table, row, kIoVariableMapColumn);
  result.changed = tableText(table, row, kIoVariableChangedColumn);
  result.plcQuality = tableText(table, row, kIoVariablePlcQualityColumn);
  result.alias = tableText(table, row, kIoVariableAliasColumn);
  result.tags = tableText(table, row, kIoVariableTagsColumn);
  result.note = tableText(table, row, kIoVariableNoteColumn);
  return result;
}

bool ioVariableTableRowHasValue(QTableWidget *table, int row) {
  if (!table || row < 0 || row >= table->rowCount() ||
      table->isRowHidden(row)) {
    return false;
  }
  return ioVariableTableRowHasValue(ioVariableTableRowFromTable(table, row));
}

QString ioVariableTableRowKey(QTableWidget *table, int row) {
  return ioVariableTableRowKey(ioVariableTableRowFromTable(table, row));
}

QVector<int> selectedIoVariableTableRows(QTableWidget *table,
                                         bool visibleOnly) {
  return selectedTableRows(table, visibleOnly);
}

QVector<int> visibleIoVariableTableRows(QTableWidget *table) {
  return visibleTableRows(table);
}

bool ioVariableTableRowsContainValue(QTableWidget *table,
                                     const QVector<int> &rows) {
  for (const int row : rows) {
    if (ioVariableTableRowHasValue(table, row)) {
      return true;
    }
  }
  return false;
}
