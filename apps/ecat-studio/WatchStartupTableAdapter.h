#pragma once

#include "WatchStartupModel.h"

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

WatchStartupWatchRow watchStartupWatchRow(QTableWidget *watchTable, int row);
WatchStartupWatchRow watchStartupWatchRow(QTableWidget *watchTable, int row,
                                          const QSet<QString> &changedKeys);
QVector<WatchStartupWatchRow> watchStartupWatchRows(QTableWidget *watchTable);
WatchStartupStartupRow watchStartupStartupRow(QTableWidget *startupTable,
                                              int row);
QVector<WatchStartupStartupRow>
watchStartupStartupRows(QTableWidget *startupTable);
QTableWidgetItem *ensureWatchStartupTableItem(QTableWidget *table, int row,
                                              int column);
