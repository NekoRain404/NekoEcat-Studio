#pragma once

// Populates and queries the Watch and Startup SDO QTableWidgets.


#include "models/WatchStartupModel.h"

#include <QSet>

class QTableWidget;
class QTableWidgetItem;

inline constexpr int kWatchStartupWatchTimeColumn = 0;
inline constexpr int kWatchStartupWatchPositionColumn = 1;
inline constexpr int kWatchStartupWatchIndexColumn = 2;
inline constexpr int kWatchStartupWatchSubIndexColumn = 3;
inline constexpr int kWatchStartupWatchValueColumn = 4;
inline constexpr int kWatchStartupWatchDecodedColumn = 5;
inline constexpr int kWatchStartupWatchTypeColumn = 6;
inline constexpr int kWatchStartupWatchModeColumn = 7;
inline constexpr int kWatchStartupWatchBaselineColumn = 8;
inline constexpr int kWatchStartupWatchBaselineDeltaColumn = 9;
inline constexpr int kWatchStartupWatchExpectedColumn = 10;
inline constexpr int kWatchStartupWatchDeltaColumn = 11;

inline constexpr int kWatchStartupStartupPositionColumn = 0;
inline constexpr int kWatchStartupStartupIndexColumn = 1;
inline constexpr int kWatchStartupStartupSubIndexColumn = 2;
inline constexpr int kWatchStartupStartupValueColumn = 3;
inline constexpr int kWatchStartupStartupTypeColumn = 4;
inline constexpr int kWatchStartupStartupStatusColumn = 5;
inline constexpr int kWatchStartupStartupDetailColumn = 6;
inline constexpr int kWatchStartupStartupWatchValueColumn = 7;
inline constexpr int kWatchStartupStartupWatchDeltaColumn = 8;

// Extracts a watch row without change tracking.
WatchStartupWatchRow watchStartupWatchRow(QTableWidget *watchTable, int row);
// Extracts a watch row with change detection via a pre-computed key set.
WatchStartupWatchRow watchStartupWatchRow(QTableWidget *watchTable, int row,
                                          const QSet<QString> &changedKeys);
// Bulk-extracts all watch rows.
QVector<WatchStartupWatchRow> watchStartupWatchRows(QTableWidget *watchTable);
// Extracts a startup SDO row with watch cross-references.
WatchStartupStartupRow watchStartupStartupRow(QTableWidget *startupTable,
                                              int row);
// Bulk-extracts all startup rows for delta evaluation.
QVector<WatchStartupStartupRow>
watchStartupStartupRows(QTableWidget *startupTable);
// Returns existing item or creates one to avoid null during updates.
QTableWidgetItem *ensureWatchStartupTableItem(QTableWidget *table, int row,
                                              int column);
