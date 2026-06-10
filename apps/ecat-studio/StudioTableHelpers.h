#pragma once

#include <QList>
#include <QString>
#include <QStringList>
#include <QVector>

class QTableWidget;
class QTextStream;

QString csvCell(QString text);
QString markdownCell(QString text);
QString tableText(QTableWidget *table, int row, int column);
int tableRowForPosition(QTableWidget *table, int position, int positionColumn);
bool tableObjectIndexMatches(QTableWidget *table, int row, const QString &index,
                             const QString &subIndex, int indexColumn,
                             int subIndexColumn);
bool tableObjectAddressMatches(QTableWidget *table, int row, int position,
                               const QString &index, const QString &subIndex,
                               int positionColumn, int indexColumn,
                               int subIndexColumn);
int tableRowForObjectIndex(QTableWidget *table, const QString &index,
                           const QString &subIndex, int indexColumn,
                           int subIndexColumn);
int tableRowForObjectAddress(QTableWidget *table, int position,
                             const QString &index, const QString &subIndex,
                             int positionColumn, int indexColumn,
                             int subIndexColumn);
int firstVisibleTableRow(QTableWidget *table);
QVector<int> selectedTableRows(QTableWidget *table, bool visibleOnly = false,
                               bool includeCurrentFallback = true);
QVector<int> visibleTableRows(QTableWidget *table);
QList<QStringList> copyTableRows(QTableWidget *table);
bool selectAndFocusTableRow(QTableWidget *table, int row, int column = 0);
void fitTableColumnsToViewport(QTableWidget *table, int stretchColumn = -1);
void writeMarkdownTable(QTextStream &out, QTableWidget *table);
