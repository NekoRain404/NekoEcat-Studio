#include "ProcessDataRowModel.h"

#include "EvidenceStatusModel.h"
#include "StudioTextHelpers.h"

#include <QSet>
#include <QStringList>

QString processDataTypeFromBits(const QString &bitsText) {
  bool ok = false;
  const int bits = bitsText.trimmed().toInt(&ok);
  if (!ok || bits <= 0) {
    return QString();
  }
  if (bits == 1) {
    return QStringLiteral("bool");
  }
  if (bits <= 8) {
    return QStringLiteral("uint8");
  }
  if (bits <= 16) {
    return QStringLiteral("uint16");
  }
  if (bits <= 32) {
    return QStringLiteral("uint32");
  }
  return QStringLiteral("uint64");
}

QString processDataNormalizedKnownType(const QString &type) {
  QString normalized = type.trimmed().toLower().replace(' ', "_");
  static const QSet<QString> knownTypes = {
      "bool",   "int8",   "int16", "int32",  "int64",  "uint8",       "uint16",
      "uint32", "uint64", "float", "double", "string", "octet_string"};
  if (knownTypes.contains(normalized)) {
    return normalized;
  }
  if (normalized == QStringLiteral("byte")) {
    return QStringLiteral("uint8");
  }
  if (normalized == QStringLiteral("word")) {
    return QStringLiteral("uint16");
  }
  if (normalized == QStringLiteral("dword")) {
    return QStringLiteral("uint32");
  }
  if (normalized == QStringLiteral("real32")) {
    return QStringLiteral("float");
  }
  if (normalized == QStringLiteral("real64")) {
    return QStringLiteral("double");
  }
  return QString();
}

QString processDataIecTypeFromNormalizedType(const QString &type) {
  if (type == QStringLiteral("bool")) {
    return QStringLiteral("BOOL");
  }
  if (type == QStringLiteral("int8")) {
    return QStringLiteral("SINT");
  }
  if (type == QStringLiteral("uint8")) {
    return QStringLiteral("USINT");
  }
  if (type == QStringLiteral("int16")) {
    return QStringLiteral("INT");
  }
  if (type == QStringLiteral("uint16")) {
    return QStringLiteral("UINT");
  }
  if (type == QStringLiteral("int32")) {
    return QStringLiteral("DINT");
  }
  if (type == QStringLiteral("uint32")) {
    return QStringLiteral("UDINT");
  }
  if (type == QStringLiteral("int64")) {
    return QStringLiteral("LINT");
  }
  if (type == QStringLiteral("uint64")) {
    return QStringLiteral("ULINT");
  }
  if (type == QStringLiteral("float")) {
    return QStringLiteral("REAL");
  }
  if (type == QStringLiteral("double")) {
    return QStringLiteral("LREAL");
  }
  return QString();
}

bool pdoMapTableRowHasTarget(const PdoMapTableRow &row) {
  return !row.index.isEmpty() && !row.subIndex.isEmpty();
}

bool freeRunEntryTableRowHasTarget(const FreeRunEntryTableRow &row) {
  return row.positionValid && row.position >= 0 && !row.index.isEmpty() &&
         !row.subIndex.isEmpty();
}

bool ioVariableTableRowHasTarget(const IoVariableTableRow &row) {
  return row.positionValid && row.position >= 0 && !row.index.isEmpty() &&
         !row.subIndex.isEmpty();
}

bool ioVariableTableRowHasValue(const IoVariableTableRow &row) {
  return !row.raw.isEmpty() || !row.watch.isEmpty();
}

QString ioVariableTableRowPreferredValue(const IoVariableTableRow &row) {
  return row.raw.isEmpty() ? row.watch : row.raw;
}

QString ioVariableTableRowStartupValue(const IoVariableTableRow &row) {
  return row.watch.isEmpty() ? row.raw : row.watch;
}

QString ioVariableTableRowTypeFromBits(const IoVariableTableRow &row) {
  return processDataTypeFromBits(row.bits);
}

QString ioVariableTableRowSdoType(const IoVariableTableRow &row) {
  const QStringList parts = row.source.toLower().split('|');
  if (parts.size() > 1) {
    const QString type = processDataNormalizedKnownType(parts.last());
    if (!type.isEmpty()) {
      return type;
    }
  }
  return ioVariableTableRowTypeFromBits(row);
}

QString ioVariableTableRowIecType(const IoVariableTableRow &row) {
  const QString type = processDataIecTypeFromNormalizedType(
      processDataNormalizedKnownType(ioVariableTableRowSdoType(row)));
  return type.isEmpty() ? QStringLiteral("ANY") : type;
}

bool ioVariableTableRowHasProcessSource(const IoVariableTableRow &row) {
  const QString source = row.source.toLower();
  return source.contains(QStringLiteral("process")) ||
         source.contains(QStringLiteral("\u8fc7\u7a0b"));
}

bool ioVariableTableRowHasPdoSource(const IoVariableTableRow &row) {
  return row.source.contains(QStringLiteral("pdo"), Qt::CaseInsensitive);
}

bool ioVariableTableRowHasWatchEvidence(const IoVariableTableRow &row) {
  return !row.watch.isEmpty() ||
         row.source.contains(QStringLiteral("watch"), Qt::CaseInsensitive);
}

bool ioVariableTableRowHasStartupDiff(const IoVariableTableRow &row) {
  return hasStartupDiffEvidence(row.startup);
}

bool ioVariableTableRowHasPdoMapIssue(const IoVariableTableRow &row) {
  return hasPdoMapIssueEvidence(row.map);
}

bool ioVariableTableRowHasMissingValue(const IoVariableTableRow &row) {
  return row.raw.isEmpty() && row.watch.isEmpty();
}

bool ioVariableTableRowHasChangedValue(const IoVariableTableRow &row) {
  return !row.changed.isEmpty();
}

bool ioVariableTableRowHasPlcIssue(const IoVariableTableRow &row,
                                   const QString &readyText) {
  return !row.plcQuality.isEmpty() &&
         row.plcQuality.compare(readyText, Qt::CaseInsensitive) != 0;
}

bool ioVariableTableRowIsRx(const IoVariableTableRow &row) {
  const QString direction = row.direction.toLower();
  return direction.contains(QStringLiteral("rx")) ||
         direction.contains(QStringLiteral("output")) ||
         direction.contains(QStringLiteral("\u8f93\u51fa"));
}

bool ioVariableTableRowIsTx(const IoVariableTableRow &row) {
  const QString direction = row.direction.toLower();
  return direction.contains(QStringLiteral("tx")) ||
         direction.contains(QStringLiteral("input")) ||
         direction.contains(QStringLiteral("\u8f93\u5165"));
}

bool ioVariableTableRowIsCia402(const IoVariableTableRow &row) {
  return row.index == QStringLiteral("0x6040") ||
         row.index == QStringLiteral("0x6041") ||
         row.index == QStringLiteral("0x6060") ||
         row.index == QStringLiteral("0x6061") ||
         row.index == QStringLiteral("0x603f") ||
         row.index == QStringLiteral("0x6064") ||
         row.index == QStringLiteral("0x606c") ||
         row.index == QStringLiteral("0x6077") ||
         row.index == QStringLiteral("0x607a") ||
         row.index == QStringLiteral("0x60ff") ||
         row.index == QStringLiteral("0x6071") ||
         row.symbol.contains(QStringLiteral("cia"), Qt::CaseInsensitive) ||
         row.meaning.contains(QStringLiteral("cia"), Qt::CaseInsensitive);
}

QString ioVariableTableObjectKey(int position, const QString &index,
                                 const QString &subIndex) {
  if (position < 0 || index.trimmed().isEmpty() ||
      subIndex.trimmed().isEmpty()) {
    return QString();
  }
  return QString("%1|%2|%3")
      .arg(position)
      .arg(normalizeHexText(index, 4), normalizeHexText(subIndex, 2));
}

QString ioVariableTableRowKey(const IoVariableTableRow &row) {
  if (!ioVariableTableRowHasTarget(row)) {
    return QString();
  }
  return ioVariableTableObjectKey(row.position, row.index, row.subIndex);
}
