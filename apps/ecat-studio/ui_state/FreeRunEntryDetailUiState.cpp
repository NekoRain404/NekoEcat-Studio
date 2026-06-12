// Detail panel text for a selected Free Run entry row.
#include "FreeRunEntryDetailUiState.h"

namespace {

// Detects PDO map warnings (missing, no map, direction/bit mismatch) using localized keywords.
bool freeRunEntryMapWarning(const FreeRunEntryTableRow &row) {
  const QString mapLower = row.mapStatus.toLower();
  return mapLower.contains(QStringLiteral("warning")) ||
         mapLower.contains(QStringLiteral("missing")) ||
         mapLower.contains(QStringLiteral("no pdo map")) ||
         row.mapStatus.contains(QStringLiteral("警告")) ||
         row.mapStatus.contains(QStringLiteral("缺失")) ||
         row.mapStatus.contains(QStringLiteral("无 PDO 映射"));
}

// Whether the map status indicates a direction mismatch between PDO and IO variable.
bool freeRunEntryDirectionMismatch(const FreeRunEntryTableRow &row) {
  const QString mapLower = row.mapStatus.toLower();
  return mapLower.contains(QStringLiteral("direction mismatch")) ||
         row.mapStatus.contains(QStringLiteral("方向不一致"));
}

// Whether the map status indicates a bit-width mismatch.
bool freeRunEntryBitMismatch(const FreeRunEntryTableRow &row) {
  const QString mapLower = row.mapStatus.toLower();
  return mapLower.contains(QStringLiteral("bit mismatch")) ||
         row.mapStatus.contains(QStringLiteral("位宽不一致"));
}

// Prefers decoded value over raw, falling back to the empty-value placeholder.
QString freeRunEntryValueText(const FreeRunEntryTableRow &row,
                              const FreeRunEntryDetailTexts &texts) {
  const QString value = row.decoded.isEmpty() ? row.raw : row.decoded;
  return value.isEmpty() ? texts.emptyValue : value;
}

// Returns the appropriate boundary text (input/output) based on direction.
QString freeRunEntryBoundaryText(const FreeRunEntryTableRow &row,
                                 const FreeRunEntryDetailTexts &texts) {
  return freeRunEntryDetailIsOutputLike(row) ? texts.outputBoundary
                                             : texts.inputBoundary;
}

} // namespace

// Neutral state when the free-run table is not available.
FreeRunEntryDetailUiState
freeRunEntryDetailUnavailableState(const FreeRunEntryDetailTexts &texts) {
  return {.text = texts.unavailableText,
          .severityKey = QStringLiteral("neutral"),
          .tooltip = texts.unavailableTip};
}

// Neutral state prompting the user to select a row.
FreeRunEntryDetailUiState
freeRunEntryDetailNoSelectionState(const FreeRunEntryDetailTexts &texts) {
  return {.text = texts.noSelectionText,
          .severityKey = QStringLiteral("neutral"),
          .tooltip = texts.noSelectionTip};
}

// Whether the entry direction is RX/output (vs TX/input).
bool freeRunEntryDetailIsOutputLike(const FreeRunEntryTableRow &row) {
  const QString directionLower = row.direction.toLower();
  return directionLower.contains(QStringLiteral("rx")) ||
         directionLower.contains(QStringLiteral("out")) ||
         row.direction.contains(QStringLiteral("输出"));
}

// Extracts the origin of the variable name from the map detail text.
QString freeRunEntryDetailNameSource(const FreeRunEntryTableRow &row,
                                     const FreeRunEntryDetailTexts &texts) {
  QString nameSource = texts.unknown;
  for (const QString &marker : texts.nameSourceMarkers) {
    if (marker.isEmpty()) {
      continue;
    }
    const int markerIndex =
        row.mapDetail.indexOf(marker, 0, Qt::CaseInsensitive);
    if (markerIndex < 0) {
      continue;
    }
    nameSource = row.mapDetail.mid(markerIndex + marker.size()).trimmed();
    break;
  }
  if (nameSource.contains('|')) {
    nameSource = nameSource.section('|', 0, 0).trimmed();
  }
  return nameSource;
}

// Maps map status, direction, and change flags to a severity key for styling.
QString freeRunEntryDetailSeverityKey(const FreeRunEntryTableRow &row,
                                      const FreeRunEntryDetailTexts &texts) {
  if (freeRunEntryDirectionMismatch(row) || freeRunEntryBitMismatch(row)) {
    return QStringLiteral("error");
  }
  if (freeRunEntryDetailIsOutputLike(row) || freeRunEntryMapWarning(row)) {
    return QStringLiteral("warning");
  }
  if (row.changed) {
    return QStringLiteral("action");
  }
  if (row.mapStatus == texts.mappedText) {
    return QStringLiteral("ok");
  }
  return QStringLiteral("neutral");
}

// Assembles the full detail panel state: summary text, severity, boundary, name source, and tooltip.
FreeRunEntryDetailUiState
buildFreeRunEntryDetailUiState(const FreeRunEntryTableRow &row,
                               const FreeRunEntryDetailTexts &texts) {
  FreeRunEntryDetailUiState state;
  const QString slave =
      row.positionValid ? QString::number(row.position) : QString();
  state.nameSource = freeRunEntryDetailNameSource(row, texts);
  state.boundary = freeRunEntryBoundaryText(row, texts);
  state.severityKey = freeRunEntryDetailSeverityKey(row, texts);
  state.text =
      texts.summaryPattern
          .arg(slave.isEmpty() ? QString::number(row.row) : slave)
          .arg(row.direction.isEmpty() ? texts.directionFallback
                                       : row.direction)
          .arg(row.index.isEmpty() ? QStringLiteral("----") : row.index)
          .arg(row.subIndex.isEmpty() ? QStringLiteral("--") : row.subIndex)
          .arg(row.name.isEmpty() ? texts.unnamed : row.name)
          .arg(row.bits.isEmpty() ? QStringLiteral("?") : row.bits)
          .arg(row.offset.isEmpty() ? QStringLiteral("?") : row.offset)
          .arg(row.bit.isEmpty() ? QStringLiteral("?") : row.bit)
          .arg(state.boundary)
          .arg(freeRunEntryValueText(row, texts))
          .arg(row.mapStatus.isEmpty() ? texts.noMapEvidence : row.mapStatus);

  state.tooltipLines << texts.selectedTitle;
  state.tooltipLines << QString("%1: #%2").arg(texts.slaveLabel, slave);
  state.tooltipLines << QString("%1: %2").arg(texts.syncManagerLabel,
                                              row.syncManager);
  state.tooltipLines << QString("%1: %2").arg(texts.directionLabel,
                                              row.direction);
  state.tooltipLines << QString("%1: %2").arg(texts.pdoLabel, row.pdo);
  state.tooltipLines
      << QString("%1: %2:%3").arg(texts.objectLabel, row.index, row.subIndex);
  state.tooltipLines << QString("%1: %2").arg(texts.nameLabel, row.name);
  state.tooltipLines << QString("%1: %2").arg(texts.nameSourceLabel,
                                              state.nameSource);
  state.tooltipLines << QString("%1: %2 bit @ %3.%4")
                            .arg(texts.locationLabel, row.bits, row.offset,
                                 row.bit);
  state.tooltipLines << QString("%1: %2").arg(texts.rawLabel, row.raw);
  state.tooltipLines << QString("%1: %2").arg(texts.decodedLabel, row.decoded);
  state.tooltipLines << QString("%1: %2").arg(texts.meaningLabel, row.meaning);
  state.tooltipLines << QString("%1: %2").arg(texts.mapStatusLabel,
                                              row.mapStatus);
  state.tooltipLines << QString("%1: %2").arg(texts.mapDetailLabel,
                                              row.mapDetail);
  state.tooltipLines << QString("%1: %2").arg(
      texts.changedLabel, row.changed ? texts.yesText : texts.noText);
  state.tooltipLines << QString("%1: %2").arg(texts.boundaryLabel,
                                              state.boundary);
  state.tooltipLines << texts.localBoundary;
  state.tooltipLines << texts.executionBoundary;
  state.tooltip = state.tooltipLines.join('\n');
  return state;
}
