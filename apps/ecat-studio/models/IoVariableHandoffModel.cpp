// PLC handoff issue detection: missing values, duplicate symbols, unmapped PDOs.
#include "IoVariableHandoffModel.h"

#include "ProcessDataRowModel.h"
#include "utils/TextHelpers.h"

#include <QHash>
#include <QStringList>

// Derives a PLC-friendly alias from available row metadata, with cascading fallbacks.
QString suggestedIoVariableAlias(const IoVariableTableRow &row,
                                 const QString &prefix, bool includeAddress) {
  QString seed = row.alias;
  if (seed.isEmpty()) {
    seed = row.symbol;
  }
  if (seed.isEmpty()) {
    seed = row.meaning;
  }
  if (seed.isEmpty()) {
    seed = QString("Obj_%1_%2").arg(row.index, row.subIndex);
  }

  QString fallbackIndex = row.index;
  QString fallbackSubIndex = row.subIndex;
  fallbackIndex.remove("0x", Qt::CaseInsensitive);
  fallbackSubIndex.remove("0x", Qt::CaseInsensitive);
  const QString fallback =
      QString("S%1_%2_%3")
          .arg(row.positionValid ? QString::number(row.position) : QString())
          .arg(fallbackIndex, fallbackSubIndex);
  QString alias = plcIdentifier(seed, fallback);
  if (includeAddress) {
    const QString address = plcIdentifier(fallback, fallback);
    if (!alias.contains(address, Qt::CaseInsensitive)) {
      alias = QString("%1_%2").arg(address, alias);
    }
  }

  const QString normalizedPrefix =
      prefix.trimmed().isEmpty() ? QString() : plcIdentifier(prefix, QString());
  if (!normalizedPrefix.isEmpty() &&
      !alias.startsWith(normalizedPrefix + "_", Qt::CaseInsensitive)) {
    alias = QString("%1_%2").arg(normalizedPrefix, alias);
  }
  return alias;
}

// Resolves alias, fallback, and final PLC symbol for a row.
IoVariableHandoffName ioVariableHandoffName(const IoVariableTableRow &row) {
  IoVariableHandoffName result;
  result.alias = row.alias;
  result.fallbackAlias = suggestedIoVariableAlias(row, QString(), true);
  result.symbol = plcIdentifier(result.alias, result.fallbackAlias);
  return result;
}

// Flags naming quality problems (missing alias, auto-generated name, no tags, duplicates).
QVector<IoVariableHandoffIssue>
ioVariableHandoffIssues(const IoVariableTableRow &row,
                        const QSet<QString> *duplicateSymbols) {
  const IoVariableHandoffName name = ioVariableHandoffName(row);
  QVector<IoVariableHandoffIssue> issues;
  if (name.alias.trimmed().isEmpty()) {
    issues.append(IoVariableHandoffIssue::MissingAlias);
  } else if (name.alias.compare(name.fallbackAlias, Qt::CaseInsensitive) == 0 ||
             name.alias.startsWith("S" + QString::number(row.position) + "_",
                                   Qt::CaseInsensitive)) {
    issues.append(IoVariableHandoffIssue::AutoName);
  }
  if (row.tags.trimmed().isEmpty()) {
    issues.append(IoVariableHandoffIssue::NoTags);
  }
  if (duplicateSymbols && duplicateSymbols->contains(name.symbol.toLower())) {
    issues.append(IoVariableHandoffIssue::DuplicateSymbol);
  }
  return issues;
}

// Scans all rows to find symbol name collisions for the duplicate check.
QSet<QString>
duplicateIoVariableHandoffSymbols(const QVector<IoVariableTableRow> &rows) {
  QHash<QString, int> counts;
  QSet<QString> duplicates;
    // Iterate over collection
  for (const IoVariableTableRow &row : rows) {
    const QString symbol = ioVariableHandoffName(row).symbol.toLower();
    const int count = counts.value(symbol, 0) + 1;
    counts.insert(symbol, count);
    if (count > 1) {
      duplicates.insert(symbol);
    }
  }
  return duplicates;
}

// Serializes issue enums to stable string keys for QML consumption.
QStringList
ioVariableHandoffIssueKeys(const QVector<IoVariableHandoffIssue> &issues) {
  QStringList keys;
    // Iterate over collection
  for (const IoVariableHandoffIssue issue : issues) {
    switch (issue) {
    case IoVariableHandoffIssue::MissingAlias:
      keys << QStringLiteral("missingAlias");
      break;
    case IoVariableHandoffIssue::AutoName:
      keys << QStringLiteral("autoName");
      break;
    case IoVariableHandoffIssue::NoTags:
      keys << QStringLiteral("noTags");
      break;
    case IoVariableHandoffIssue::DuplicateSymbol:
      keys << QStringLiteral("duplicateSymbol");
      break;
    }
  }
  return keys;
}

// Tests for a specific issue type in a list.
bool ioVariableHandoffHasIssue(const QVector<IoVariableHandoffIssue> &issues,
                               IoVariableHandoffIssue issue) {
  return issues.contains(issue);
}

// Maps row direction metadata to PLC I/O classification (Output/Input/Parameter).
QString ioVariableHandoffPlcDirection(const IoVariableTableRow &row) {
  if (ioVariableTableRowIsRx(row)) {
    return QStringLiteral("Output");
  }
  if (ioVariableTableRowIsTx(row)) {
    return QStringLiteral("Input");
  }
  return QStringLiteral("Parameter");
}

// Delegates to IEC type resolution for PLC variable declaration.
QString ioVariableHandoffPlcType(const IoVariableTableRow &row) {
  return ioVariableTableRowIecType(row);
}

// Builds a structured IEC comment with position, OD address, and quality info.
QString ioVariableHandoffComment(const IoVariableTableRow &row,
                                 const QStringList &qualityLabels) {
  QString comment = QString("#%1 %2:%3")
                        .arg(row.positionValid ? QString::number(row.position)
                                               : QStringLiteral("?"),
                             row.index, row.subIndex);
  QStringList facts;
  const QString direction = ioVariableHandoffPlcDirection(row);
  if (!direction.isEmpty()) {
    facts << direction;
  }
  if (!row.pdo.isEmpty()) {
    facts << row.pdo;
  }
  if (!row.meaning.isEmpty()) {
    facts << row.meaning;
  }
  if (!qualityLabels.isEmpty()) {
    facts << QString("Quality: %1").arg(qualityLabels.join(" | "));
  }
  if (!facts.isEmpty()) {
    comment += " | " + facts.join(" | ");
  }
  comment.replace("(*", "(");
  comment.replace("*)", ")");
  return comment;
}

// Appends numeric suffixes to ensure each exported PLC symbol is unique.
QString ioVariableHandoffUniqueSymbol(const IoVariableTableRow &row,
                                      QSet<QString> *usedSymbols) {
  QString symbol = ioVariableHandoffName(row).symbol;
  if (!usedSymbols) {
    return symbol;
  }

  const QString baseSymbol = symbol;
  int duplicate = 2;
  while (usedSymbols->contains(symbol.toLower())) {
    symbol = QString("%1_%2").arg(baseSymbol).arg(duplicate++);
  }
  usedSymbols->insert(symbol.toLower());
  return symbol;
}

// Renders one IEC 61131-3 VAR_GLOBAL declaration line with inline comment.
QString ioVariableHandoffDeclarationLine(const IoVariableTableRow &row,
                                         QSet<QString> *usedSymbols,
                                         const QStringList &qualityLabels) {
  const QString symbol = ioVariableHandoffUniqueSymbol(row, usedSymbols);
  const QString comment = ioVariableHandoffComment(row, qualityLabels);
  return QString("    %1 : %2; (* %3 *)")
      .arg(symbol, ioVariableHandoffPlcType(row), comment);
}

// Renders the complete IEC variable declaration block for PLC export.
QString ioVariableHandoffDeclarationBlock(
    const QVector<IoVariableTableRow> &rows,
    const QVector<QStringList> &qualityLabelsByRow) {
  QSet<QString> usedSymbols;
  QStringList lines;
  lines << QStringLiteral("VAR_GLOBAL");
    // Iterate over collection
  for (int i = 0; i < rows.size(); ++i) {
    lines << ioVariableHandoffDeclarationLine(rows.at(i), &usedSymbols,
                                              qualityLabelsByRow.value(i));
  }
  lines << QStringLiteral("END_VAR");
  return lines.join('\n');
}

// Column headers for the CSV export format.
QStringList ioVariableHandoffCsvHeaders() {
  return {"Symbol",   "Direction",   "Type",      "Slave",   "Index",
          "SubIndex", "Bits",        "Pdo",       "Source",  "Raw",
          "Decoded",  "Meaning",     "Watch",     "Startup", "Map",
          "Changed",  "Quality",     "Alias",     "Tags",    "Note",
          "Address",  "DefaultName", "ExportedAt"};
}

// Builds a flat CSV row from an I/O variable table row for export.
IoVariableHandoffCsvRow ioVariableHandoffCsvRow(const IoVariableTableRow &row,
                                                QSet<QString> *usedSymbols,
                                                const QString &exportedAt) {
  const IoVariableHandoffName name = ioVariableHandoffName(row);
  const QString symbol = ioVariableHandoffUniqueSymbol(row, usedSymbols);
  const QString address =
      QString("#%1 %2:%3")
          .arg(row.positionValid ? QString::number(row.position)
                                 : QStringLiteral("?"),
               row.index, row.subIndex);
  const QString defaultName =
      row.symbol.isEmpty() ? name.fallbackAlias : row.symbol;
  return {{symbol,
           ioVariableHandoffPlcDirection(row),
           ioVariableHandoffPlcType(row),
           row.positionValid ? QString::number(row.position) : QString(),
           row.index,
           row.subIndex,
           row.bits,
           row.pdo,
           row.source,
           row.raw,
           row.decoded,
           row.meaning,
           row.watch,
           row.startup,
           row.map,
           row.changed,
           row.plcQuality,
           name.alias,
           row.tags,
           row.note,
           address,
           defaultName,
           exportedAt}};
}
