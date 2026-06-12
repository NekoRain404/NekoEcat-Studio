#pragma once

// Populates and queries the PDO map / process data QTableWidget.


#include "infra/ProcessDataTypes.h"

#include <QString>
#include <QVector>

class QTableWidget;

inline constexpr int kPdoMapSyncManagerColumn = 0;
inline constexpr int kPdoMapPdoColumn = 1;
inline constexpr int kPdoMapIndexColumn = 2;
inline constexpr int kPdoMapSubIndexColumn = 3;
inline constexpr int kPdoMapBitsColumn = 4;
inline constexpr int kPdoMapNameColumn = 5;

inline constexpr int kFreeRunEntryPositionColumn = 0;
inline constexpr int kFreeRunEntrySyncManagerColumn = 1;
inline constexpr int kFreeRunEntryDirectionColumn = 2;
inline constexpr int kFreeRunEntryPdoColumn = 3;
inline constexpr int kFreeRunEntryIndexColumn = 4;
inline constexpr int kFreeRunEntrySubIndexColumn = 5;
inline constexpr int kFreeRunEntryBitsColumn = 6;
inline constexpr int kFreeRunEntryOffsetColumn = 7;
inline constexpr int kFreeRunEntryBitColumn = 8;
inline constexpr int kFreeRunEntryNameColumn = 9;
inline constexpr int kFreeRunEntryRawColumn = 10;
inline constexpr int kFreeRunEntryDecodedColumn = 11;
inline constexpr int kFreeRunEntryMeaningColumn = 12;
inline constexpr int kFreeRunEntryMapStatusColumn = 13;
inline constexpr int kFreeRunEntryMapDetailColumn = 14;

inline constexpr int kIoVariablePositionColumn = 0;
inline constexpr int kIoVariableDirectionColumn = 1;
inline constexpr int kIoVariableSymbolColumn = 2;
inline constexpr int kIoVariableIndexColumn = 3;
inline constexpr int kIoVariableSubIndexColumn = 4;
inline constexpr int kIoVariableBitsColumn = 5;
inline constexpr int kIoVariablePdoColumn = 6;
inline constexpr int kIoVariableSourceColumn = 7;
inline constexpr int kIoVariableRawColumn = 8;
inline constexpr int kIoVariableDecodedColumn = 9;
inline constexpr int kIoVariableMeaningColumn = 10;
inline constexpr int kIoVariableWatchColumn = 11;
inline constexpr int kIoVariableStartupColumn = 12;
inline constexpr int kIoVariableMapColumn = 13;
inline constexpr int kIoVariableChangedColumn = 14;
inline constexpr int kIoVariablePlcQualityColumn = 15;
inline constexpr int kIoVariableAliasColumn = 16;
inline constexpr int kIoVariableTagsColumn = 17;
inline constexpr int kIoVariableNoteColumn = 18;

PdoMapTableRow pdoMapTableRowFromTable(QTableWidget *table, int row);
FreeRunEntryTableRow freeRunEntryTableRowFromTable(QTableWidget *table,
                                                   int row);
IoVariableTableRow ioVariableTableRowFromTable(QTableWidget *table, int row);
bool ioVariableTableRowHasValue(QTableWidget *table, int row);
QString ioVariableTableRowKey(QTableWidget *table, int row);
QVector<int> selectedIoVariableTableRows(QTableWidget *table,
                                         bool visibleOnly = false);
QVector<int> visibleIoVariableTableRows(QTableWidget *table);
bool ioVariableTableRowsContainValue(QTableWidget *table,
                                     const QVector<int> &rows);
