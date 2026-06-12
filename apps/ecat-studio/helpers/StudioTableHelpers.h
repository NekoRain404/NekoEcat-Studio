#pragma once

// Generic QTableWidget utilities: row selection, CSV, markdown export.


#include <QList>
#include <QString>
#include <QStringList>
#include <QVector>

class QTableWidget;
class QTextStream;

// Escapes and quotes text for safe CSV output.
QString csvCell(QString text);
// Escapes text for safe markdown table cells.
QString markdownCell(QString text);
// Safely reads trimmed text from a table cell.
QString tableText(QTableWidget *table, int row, int column);
// First row matching a numeric position in a given column.
int tableRowForPosition(QTableWidget *table, int position, int positionColumn);
// Whether the row's index:subIndex matches a target pair (hex-normalized).
bool tableObjectIndexMatches(QTableWidget *table, int row, const QString &index,
                             const QString &subIndex, int indexColumn,
                             int subIndexColumn);
// Whether the row matches full position + index:subIndex.
bool tableObjectAddressMatches(QTableWidget *table, int row, int position,
                               const QString &index, const QString &subIndex,
                               int positionColumn, int indexColumn,
                               int subIndexColumn);
// First row matching an index:subIndex pair.
int tableRowForObjectIndex(QTableWidget *table, const QString &index,
                           const QString &subIndex, int indexColumn,
                           int subIndexColumn);
// First row matching position + index:subIndex.
int tableRowForObjectAddress(QTableWidget *table, int position,
                             const QString &index, const QString &subIndex,
                             int positionColumn, int indexColumn,
                             int subIndexColumn);
// First non-hidden row index, or -1.
int firstVisibleTableRow(QTableWidget *table);
// Sorted, deduplicated selected row indices with current-row fallback.
QVector<int> selectedTableRows(QTableWidget *table, bool visibleOnly = false,
                               bool includeCurrentFallback = true);
// Indices of all non-hidden rows.
QVector<int> visibleTableRows(QTableWidget *table);
// Snapshots all rows as string lists for clipboard/export.
QList<QStringList> copyTableRows(QTableWidget *table);
// Selects a row, scrolls into view, and gives keyboard focus.
bool selectAndFocusTableRow(QTableWidget *table, int row, int column = 0);
// Auto-sizes columns to content with one stretch column.
void fitTableColumnsToViewport(QTableWidget *table, int stretchColumn = -1);
// Renders the table as markdown to a text stream.
void writeMarkdownTable(QTextStream &out, QTableWidget *table);
