// Populates and queries the Watch and Startup SDO QTableWidgets.
#include "WatchStartupTableAdapter.h"

#include "helpers/StudioTableHelpers.h"

#include <QTableWidget>
#include <QTableWidgetItem>

namespace {

// Safely parses a position string, returning -1 on invalid input.
int parsedPosition(const QString &text) {
  bool ok = false;
  const int position = text.toInt(&ok);
  return ok ? position : -1;
}

} // namespace

// Overload without change-tracking set, for simple row extraction.
WatchStartupWatchRow watchStartupWatchRow(QTableWidget *watchTable, int row) {
  return watchStartupWatchRow(watchTable, row, {});
}

// Extracts a watch row and marks it as changed if its target key is in the changed set.
WatchStartupWatchRow watchStartupWatchRow(QTableWidget *watchTable, int row,
                                          const QSet<QString> &changedKeys) {
  WatchStartupWatchRow model;
  model.row = row;
  if (!watchTable || row < 0 || row >= watchTable->rowCount()) {
    return model;
  }

  model.time = tableText(watchTable, row, kWatchStartupWatchTimeColumn);
  model.position = parsedPosition(
      tableText(watchTable, row, kWatchStartupWatchPositionColumn));
  model.index = tableText(watchTable, row, kWatchStartupWatchIndexColumn);
  model.subIndex = tableText(watchTable, row, kWatchStartupWatchSubIndexColumn);
  model.value = tableText(watchTable, row, kWatchStartupWatchValueColumn);
  model.decoded = tableText(watchTable, row, kWatchStartupWatchDecodedColumn);
  model.type = tableText(watchTable, row, kWatchStartupWatchTypeColumn);
  model.mode = tableText(watchTable, row, kWatchStartupWatchModeColumn);
  model.baseline = tableText(watchTable, row, kWatchStartupWatchBaselineColumn);
  model.baselineDelta =
      tableText(watchTable, row, kWatchStartupWatchBaselineDeltaColumn);
  model.startup = tableText(watchTable, row, kWatchStartupWatchExpectedColumn);
  model.startupDelta =
      tableText(watchTable, row, kWatchStartupWatchDeltaColumn);
  model.changed = changedKeys.contains(
      watchStartupTargetKey(model.position, model.index, model.subIndex));
  return model;
}

// Bulk-extracts all watch rows for snapshot comparison or delta evaluation.
QVector<WatchStartupWatchRow> watchStartupWatchRows(QTableWidget *watchTable) {
  QVector<WatchStartupWatchRow> rows;
  if (!watchTable) {
    return rows;
  }

  rows.reserve(watchTable->rowCount());
  for (int row = 0; row < watchTable->rowCount(); ++row) {
    rows.append(watchStartupWatchRow(watchTable, row));
  }
  return rows;
}

// Extracts a startup SDO row including the watch cross-reference values.
WatchStartupStartupRow watchStartupStartupRow(QTableWidget *startupTable,
                                              int row) {
  WatchStartupStartupRow model;
  model.row = row;
  if (!startupTable || row < 0 || row >= startupTable->rowCount()) {
    return model;
  }

  model.positionText =
      tableText(startupTable, row, kWatchStartupStartupPositionColumn);
  model.position = parsedPosition(model.positionText);
  model.index = tableText(startupTable, row, kWatchStartupStartupIndexColumn);
  model.subIndex =
      tableText(startupTable, row, kWatchStartupStartupSubIndexColumn);
  model.value = tableText(startupTable, row, kWatchStartupStartupValueColumn);
  model.type = tableText(startupTable, row, kWatchStartupStartupTypeColumn);
  model.status = tableText(startupTable, row, kWatchStartupStartupStatusColumn);
  model.detail = tableText(startupTable, row, kWatchStartupStartupDetailColumn);
  model.watchValue =
      tableText(startupTable, row, kWatchStartupStartupWatchValueColumn);
  model.watchDelta =
      tableText(startupTable, row, kWatchStartupStartupWatchDeltaColumn);
  return model;
}

// Bulk-extracts all startup rows for delta evaluation against the watch table.
QVector<WatchStartupStartupRow>
watchStartupStartupRows(QTableWidget *startupTable) {
  QVector<WatchStartupStartupRow> rows;
  if (!startupTable) {
    return rows;
  }

  rows.reserve(startupTable->rowCount());
  for (int row = 0; row < startupTable->rowCount(); ++row) {
    rows.append(watchStartupStartupRow(startupTable, row));
  }
  return rows;
}

// Returns the existing item at (row, column) or creates one if absent, to avoid null checks during updates.
QTableWidgetItem *ensureWatchStartupTableItem(QTableWidget *table, int row,
                                              int column) {
  if (!table || row < 0 || row >= table->rowCount() || column < 0 ||
      column >= table->columnCount()) {
    return nullptr;
  }

  auto *item = table->item(row, column);
  if (!item) {
    item = new QTableWidgetItem;
    table->setItem(row, column, item);
  }
  return item;
}
