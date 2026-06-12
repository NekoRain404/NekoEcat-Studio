#pragma once

// Populates and queries the Object Dictionary (SDO) QTableWidget.


#include <QString>
#include <QVector>

class QTableWidget;

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

struct SdoDictionaryReadObject {
  int row = -1;
  QString index;
  QString subIndex;
  QString type;
};

SdoDictionaryRow sdoDictionaryRowFromTable(QTableWidget *table, int row);
bool sdoDictionaryRowHasTarget(const SdoDictionaryRow &row);
bool sdoDictionaryRowHasValue(const SdoDictionaryRow &row);
bool sdoDictionaryRowIsWritable(const SdoDictionaryRow &row);
QVector<int> visibleSdoDictionaryRows(QTableWidget *table);
QVector<int> failedSdoDictionaryRows(QTableWidget *table);
bool sdoDictionaryRowsContainValue(QTableWidget *table,
                                   const QVector<int> &rows);
SdoDictionaryRow sdoDictionaryRowForTarget(QTableWidget *table,
                                           const QString &index,
                                           const QString &subIndex);
QVector<SdoDictionaryReadObject>
sdoDictionaryReadObjectsFromRows(QTableWidget *table, const QVector<int> &rows,
                                 int *skipped = nullptr);
