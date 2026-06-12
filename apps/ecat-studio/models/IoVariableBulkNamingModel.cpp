// Bulk PLC symbol naming rules and collision detection for I/O variables.
#include "IoVariableBulkNamingModel.h"

#include "IoVariableHandoffModel.h"
#include "ProcessDataRowModel.h"

#include <QSet>

namespace {

// Deduplicates tags case-insensitively before appending to avoid redundant labels.
void appendUniqueTag(QStringList *tags, QString tag) {
  if (!tags) {
    return;
  }

  tag = tag.trimmed();
  if (tag.isEmpty()) {
    return;
  }
  for (const QString &existing : *tags) {
    if (existing.trimmed().compare(tag, Qt::CaseInsensitive) == 0) {
      return;
    }
  }
  tags->append(tag);
}

} // namespace

// Counts how many rows already have user-assigned aliases.
int countIoVariableBulkNamingExistingAliases(
    const QVector<IoVariableTableRow> &rows) {
  int count = 0;
  for (const IoVariableTableRow &row : rows) {
    if (!row.alias.trimmed().isEmpty()) {
      ++count;
    }
  }
  return count;
}

// Generates alias and tag metadata for selected I/O rows, resolving naming collisions.
IoVariableBulkNamingResult
buildIoVariableBulkNamingPlan(const QVector<IoVariableTableRow> &allRows,
                              const QVector<int> &targetRows,
                              const QHash<QString, QStringList> &metadataByKey,
                              const IoVariableBulkNamingOptions &options) {
  IoVariableBulkNamingResult result;

  QHash<int, IoVariableTableRow> rowsByNumber;
  rowsByNumber.reserve(allRows.size());
  for (const IoVariableTableRow &row : allRows) {
    rowsByNumber.insert(row.row, row);
  }

  QSet<int> scopedRows;
  for (const int row : targetRows) {
    scopedRows.insert(row);
  }

  QSet<QString> usedAliases;
  for (const IoVariableTableRow &row : allRows) {
    if (scopedRows.contains(row.row) && !options.keepExistingAliases) {
      continue;
    }
    const QString alias = row.alias.trimmed();
    if (!alias.isEmpty()) {
      usedAliases.insert(alias.toLower());
    }
  }

  for (const int rowNumber : targetRows) {
    if (!rowsByNumber.contains(rowNumber)) {
      ++result.skippedInvalidRows;
      continue;
    }

    const IoVariableTableRow row = rowsByNumber.value(rowNumber);
    const QString key = ioVariableTableRowKey(row);
    if (key.isEmpty()) {
      ++result.skippedInvalidRows;
      continue;
    }

    QStringList metadata = metadataByKey.value(key);
    while (metadata.size() < 3) {
      metadata.append(QString());
    }

    if (options.keepExistingAliases && !metadata.value(0).trimmed().isEmpty()) {
      ++result.skippedExistingAliases;
    } else {
      QString alias =
          suggestedIoVariableAlias(row, options.prefix, options.includeAddress);
      const QString baseAlias = alias;
      int duplicate = 2;
      while (usedAliases.contains(alias.toLower())) {
        alias = QString("%1_%2").arg(baseAlias).arg(duplicate++);
      }
      usedAliases.insert(alias.toLower());
      metadata[0] = alias;
    }

    QStringList tags = metadata.value(1).split(',', Qt::SkipEmptyParts);
    for (const QString &tag : options.requestedTags) {
      appendUniqueTag(&tags, tag);
    }
    if (options.addDirectionTags) {
      appendUniqueTag(&tags, ioVariableHandoffPlcDirection(row).toLower());
      appendUniqueTag(&tags, ioVariableHandoffPlcType(row).toLower());
    }
    metadata[1] = tags.join(", ");

    result.metadataUpdates.insert(key, metadata);
    ++result.updated;
  }

  return result;
}
