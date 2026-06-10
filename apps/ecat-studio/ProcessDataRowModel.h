#pragma once

#include "ProcessDataTypes.h"

#include <QString>

QString processDataTypeFromBits(const QString &bitsText);
QString processDataNormalizedKnownType(const QString &type);
QString processDataIecTypeFromNormalizedType(const QString &type);

bool pdoMapTableRowHasTarget(const PdoMapTableRow &row);
bool freeRunEntryTableRowHasTarget(const FreeRunEntryTableRow &row);
bool ioVariableTableRowHasTarget(const IoVariableTableRow &row);
bool ioVariableTableRowHasValue(const IoVariableTableRow &row);
QString ioVariableTableRowPreferredValue(const IoVariableTableRow &row);
QString ioVariableTableRowStartupValue(const IoVariableTableRow &row);
QString ioVariableTableRowTypeFromBits(const IoVariableTableRow &row);
QString ioVariableTableRowSdoType(const IoVariableTableRow &row);
QString ioVariableTableRowIecType(const IoVariableTableRow &row);
bool ioVariableTableRowHasProcessSource(const IoVariableTableRow &row);
bool ioVariableTableRowHasPdoSource(const IoVariableTableRow &row);
bool ioVariableTableRowHasWatchEvidence(const IoVariableTableRow &row);
bool ioVariableTableRowHasStartupDiff(const IoVariableTableRow &row);
bool ioVariableTableRowHasPdoMapIssue(const IoVariableTableRow &row);
bool ioVariableTableRowHasMissingValue(const IoVariableTableRow &row);
bool ioVariableTableRowHasChangedValue(const IoVariableTableRow &row);
bool ioVariableTableRowHasPlcIssue(const IoVariableTableRow &row,
                                   const QString &readyText);
bool ioVariableTableRowIsRx(const IoVariableTableRow &row);
bool ioVariableTableRowIsTx(const IoVariableTableRow &row);
bool ioVariableTableRowIsCia402(const IoVariableTableRow &row);
QString ioVariableTableObjectKey(int position, const QString &index,
                                 const QString &subIndex);
QString ioVariableTableRowKey(const IoVariableTableRow &row);
