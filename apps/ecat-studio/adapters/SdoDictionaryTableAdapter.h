#pragma once

// Populates and queries the Object Dictionary (SDO) QTableWidget.


#include <QString>
#include <QVector>

class QTableWidget;

// Snapshot of a single Object Dictionary entry with access rights and current value.
struct SdoDictionaryRow {
    int row = -1;
    QString object;
    QString index;
    QString subIndex;
    QString access;
    QString type;
    QString bits;
    QString name;
    QString value;
    QString status;
};

// Minimal target descriptor for issuing batch SDO read requests.
struct SdoDictionaryReadObject {
    int row = -1;
    QString index;
    QString subIndex;
    QString type;
};

// Extracts all columns of an Object Dictionary row.
SdoDictionaryRow sdoDictionaryRowFromTable(QTableWidget* table, int row);
// Whether the row identifies a specific index:subIndex target.
bool sdoDictionaryRowHasTarget(const SdoDictionaryRow& row);
// Whether the row has a populated value.
bool sdoDictionaryRowHasValue(const SdoDictionaryRow& row);
// Whether access rights include write permission.
bool sdoDictionaryRowIsWritable(const SdoDictionaryRow& row);
// Indices of all non-hidden rows for batch operations.
QVector<int> visibleSdoDictionaryRows(QTableWidget* table);
// Row indices with a failed SDO read status.
QVector<int> failedSdoDictionaryRows(QTableWidget* table);
// Whether any of the given visible rows have a populated value.
bool sdoDictionaryRowsContainValue(QTableWidget* table, const QVector<int>& rows);
// Looks up the row for a specific index:subIndex pair.
SdoDictionaryRow sdoDictionaryRowForTarget(QTableWidget* table, const QString& index, const QString& subIndex);
// Converts selected rows into SDO read-request objects, reporting how many were skipped.
QVector<SdoDictionaryReadObject> sdoDictionaryReadObjectsFromRows(QTableWidget* table, const QVector<int>& rows,
                                                                  int* skipped = nullptr);
