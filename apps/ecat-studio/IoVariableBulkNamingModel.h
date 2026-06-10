#pragma once

#include "ProcessDataTypes.h"

#include <QHash>
#include <QString>
#include <QStringList>
#include <QVector>

struct IoVariableBulkNamingOptions {
  QString prefix;
  QStringList requestedTags;
  bool includeAddress = true;
  bool keepExistingAliases = true;
  bool addDirectionTags = true;
};

struct IoVariableBulkNamingResult {
  QHash<QString, QStringList> metadataUpdates;
  int updated = 0;
  int skippedExistingAliases = 0;
  int skippedInvalidRows = 0;
};

int countIoVariableBulkNamingExistingAliases(
    const QVector<IoVariableTableRow> &rows);
IoVariableBulkNamingResult
buildIoVariableBulkNamingPlan(const QVector<IoVariableTableRow> &allRows,
                              const QVector<int> &targetRows,
                              const QHash<QString, QStringList> &metadataByKey,
                              const IoVariableBulkNamingOptions &options);
