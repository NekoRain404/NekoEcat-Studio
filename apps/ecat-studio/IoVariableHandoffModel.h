#pragma once

#include "ProcessDataTypes.h"

#include <QSet>
#include <QString>
#include <QStringList>
#include <QVector>

enum class IoVariableHandoffIssue {
  MissingAlias,
  AutoName,
  NoTags,
  DuplicateSymbol,
};

struct IoVariableHandoffName {
  QString alias;
  QString fallbackAlias;
  QString symbol;
};

struct IoVariableHandoffCsvRow {
  QStringList values;
};

QString suggestedIoVariableAlias(const IoVariableTableRow &row,
                                 const QString &prefix, bool includeAddress);
IoVariableHandoffName ioVariableHandoffName(const IoVariableTableRow &row);
QVector<IoVariableHandoffIssue>
ioVariableHandoffIssues(const IoVariableTableRow &row,
                        const QSet<QString> *duplicateSymbols);
QSet<QString>
duplicateIoVariableHandoffSymbols(const QVector<IoVariableTableRow> &rows);
QStringList
ioVariableHandoffIssueKeys(const QVector<IoVariableHandoffIssue> &issues);
bool ioVariableHandoffHasIssue(const QVector<IoVariableHandoffIssue> &issues,
                               IoVariableHandoffIssue issue);
QString ioVariableHandoffPlcDirection(const IoVariableTableRow &row);
QString ioVariableHandoffPlcType(const IoVariableTableRow &row);
QString ioVariableHandoffComment(const IoVariableTableRow &row,
                                 const QStringList &qualityLabels);
QString ioVariableHandoffUniqueSymbol(const IoVariableTableRow &row,
                                      QSet<QString> *usedSymbols);
QString ioVariableHandoffDeclarationLine(const IoVariableTableRow &row,
                                         QSet<QString> *usedSymbols,
                                         const QStringList &qualityLabels);
QString ioVariableHandoffDeclarationBlock(
    const QVector<IoVariableTableRow> &rows,
    const QVector<QStringList> &qualityLabelsByRow);
QStringList ioVariableHandoffCsvHeaders();
IoVariableHandoffCsvRow ioVariableHandoffCsvRow(const IoVariableTableRow &row,
                                                QSet<QString> *usedSymbols,
                                                const QString &exportedAt);
