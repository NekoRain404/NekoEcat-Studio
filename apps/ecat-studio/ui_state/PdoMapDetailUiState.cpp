// Detail panel text for a selected PDO map row.
#include "PdoMapDetailUiState.h"

#include "models/ProcessDataRowModel.h"

namespace {

// Whether the row identifies a specific index:subIndex address.
bool pdoMapDetailHasAddress(const PdoMapTableRow &row) {
  return !row.index.isEmpty() && !row.subIndex.isEmpty();
}

// Whether the bit-width field is a positive integer.
bool pdoMapDetailHasValidBits(const PdoMapTableRow &row) {
  bool ok = false;
  return row.bits.toInt(&ok) > 0 && ok;
}

// Returns a localized direction label (RX/output, TX/input, or unknown).
QString pdoMapDetailDirectionText(const PdoMapTableRow &row,
                                  const PdoMapDetailTexts &texts) {
  if (pdoMapDetailIsRxOutput(row)) {
    return texts.directionRxOutput;
  }
  if (pdoMapDetailIsTxInput(row)) {
    return texts.directionTxInput;
  }
  return texts.directionUnknown;
}

// Returns a localized role label describing the PDO entry's data flow role.
QString pdoMapDetailRoleText(const PdoMapTableRow &row,
                             const PdoMapDetailTexts &texts) {
  if (pdoMapDetailIsRxOutput(row)) {
    return texts.roleRxOutput;
  }
  if (pdoMapDetailIsTxInput(row)) {
    return texts.roleTxInput;
  }
  return texts.roleGeneric;
}

} // namespace

// Neutral state when the PDO map table is not available.
PdoMapDetailUiState
pdoMapDetailUnavailableState(const PdoMapDetailTexts &texts) {
  return {.text = texts.unavailableText,
    // Set severityKey field
          .severityKey = QStringLiteral("neutral"),
    // Set tooltip field
          .tooltip = texts.unavailableTip};
}

// Neutral state prompting the user to select a PDO map row.
PdoMapDetailUiState
pdoMapDetailNoSelectionState(const PdoMapDetailTexts &texts) {
  return {.text = texts.noSelectionText,
    // Set severityKey field
          .severityKey = QStringLiteral("neutral"),
    // Set tooltip field
          .tooltip = texts.noSelectionTip};
}

// Whether the PDO name or sync manager indicates an RxPDO/output mapping.
bool pdoMapDetailIsRxOutput(const PdoMapTableRow &row) {
  const QString pdoLower = row.pdo.toLower();
  const QString smLower = row.syncManager.toLower();
  return pdoLower.contains(QStringLiteral("rxpdo")) ||
         pdoLower.contains(QStringLiteral("rx")) ||
         pdoLower.contains(QStringLiteral("output")) ||
         smLower.contains(QStringLiteral("rx"));
}

// Whether the PDO name or sync manager indicates a TxPDO/input mapping.
bool pdoMapDetailIsTxInput(const PdoMapTableRow &row) {
  const QString pdoLower = row.pdo.toLower();
  const QString smLower = row.syncManager.toLower();
  return pdoLower.contains(QStringLiteral("txpdo")) ||
         pdoLower.contains(QStringLiteral("tx")) ||
         pdoLower.contains(QStringLiteral("input")) ||
         smLower.contains(QStringLiteral("tx"));
}

// Whether the index matches a CiA 402 drive profile object or name hints.
bool pdoMapDetailIsCia402(const PdoMapTableRow &row) {
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
         row.name.contains(QStringLiteral("cia"), Qt::CaseInsensitive) ||
         row.name.contains(QStringLiteral("controlword"),
                           Qt::CaseInsensitive) ||
         row.name.contains(QStringLiteral("statusword"), Qt::CaseInsensitive);
}

// Maps address validity, bit width, and direction to a severity key.
QString pdoMapDetailSeverityKey(const PdoMapTableRow &row) {
  if (!pdoMapDetailHasAddress(row) || !pdoMapDetailHasValidBits(row)) {
    return QStringLiteral("warning");
  }
  if (pdoMapDetailIsRxOutput(row)) {
    return QStringLiteral("action");
  }
  return QStringLiteral("ok");
}

// Assembles the full PDO map detail state: direction, role, inferred type, CiA 402 flag, and tooltip.
PdoMapDetailUiState buildPdoMapDetailUiState(const PdoMapTableRow &row,
                                             int selectedPosition,
                                             const PdoMapDetailTexts &texts) {
  PdoMapDetailUiState state;
  state.severityKey = pdoMapDetailSeverityKey(row);
  state.direction = pdoMapDetailDirectionText(row, texts);
  state.role = pdoMapDetailRoleText(row, texts);
  state.inferredType = processDataTypeFromBits(row.bits);
  state.cia402 = pdoMapDetailIsCia402(row);
  state.text =
      texts.summaryPattern.arg(state.direction)
          .arg(row.index.isEmpty() ? QStringLiteral("----") : row.index)
          .arg(row.subIndex.isEmpty() ? QStringLiteral("--") : row.subIndex)
          .arg(row.bits.isEmpty() ? QStringLiteral("?") : row.bits)
          .arg(state.inferredType.isEmpty() ? texts.typeFallback
                                            : state.inferredType)
          .arg(row.name.isEmpty() ? texts.unnamed : row.name)
          .arg(state.role);

  state.tooltipLines << texts.selectedTitle;
  state.tooltipLines << QString("%1: #%2").arg(
      texts.slaveLabel, QString::number(selectedPosition));
  state.tooltipLines << QString("%1: %2").arg(texts.syncManagerLabel,
                                              row.syncManager);
  state.tooltipLines << QString("%1: %2").arg(texts.pdoLabel, row.pdo);
  state.tooltipLines
      << QString("%1: %2:%3").arg(texts.objectLabel, row.index, row.subIndex);
  state.tooltipLines << QString("%1: %2").arg(texts.bitsLabel, row.bits);
  state.tooltipLines << QString("%1: %2").arg(texts.inferredTypeLabel,
                                              state.inferredType);
  state.tooltipLines << QString("%1: %2").arg(texts.nameLabel, row.name);
  state.tooltipLines << QString("%1: %2").arg(texts.directionLabel,
                                              state.direction);
  state.tooltipLines << QString("%1: %2").arg(texts.roleLabel, state.role);
  state.tooltipLines << QString("%1: %2").arg(
      texts.driveEvidenceLabel,
      state.cia402 ? texts.cia402Candidate : texts.genericEntry);
  state.tooltipLines << texts.localBoundary;
  state.tooltipLines << texts.executionBoundary;
  state.tooltip = state.tooltipLines.join('\n');
  return state;
}
