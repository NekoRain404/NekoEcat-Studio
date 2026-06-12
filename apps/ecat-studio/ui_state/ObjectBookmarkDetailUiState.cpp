// Detail panel text for a selected object bookmark row.
#include "ObjectBookmarkDetailUiState.h"

// Neutral state when the bookmark table is not available.
ObjectBookmarkDetailUiState
objectBookmarkDetailUnavailableState(const ObjectBookmarkDetailTexts &texts) {
  return {.text = texts.unavailableText,
          .severityKey = QStringLiteral("neutral"),
          .tooltip = texts.unavailableTip};
}

// Neutral state prompting the user to select a bookmark.
ObjectBookmarkDetailUiState
objectBookmarkDetailNoSelectionState(const ObjectBookmarkDetailTexts &texts) {
  return {.text = texts.noSelectionText,
          .severityKey = QStringLiteral("neutral"),
          .tooltip = texts.noSelectionTip};
}

// Maps target validity, value presence, and access rights to a severity key.
QString
objectBookmarkDetailSeverityKey(const SdoObjectBookmarkRow &row,
                                const ObjectBookmarkDetailTexts &texts) {
  const bool readOnly =
      sdoObjectAccessIsReadOnly(row.access, texts.readOnlyText);
  const bool hasTarget = sdoObjectBookmarkRowHasTarget(row);
  const bool hasValue = !row.lastValue.isEmpty();

  if (!hasTarget) {
    return QStringLiteral("warning");
  }
  if (hasValue && !readOnly) {
    return QStringLiteral("ok");
  }
  if (hasValue || readOnly) {
    return QStringLiteral("action");
  }
  return QStringLiteral("neutral");
}

// Assembles the full bookmark detail state: summary, reuse guidance, severity, and tooltip.
ObjectBookmarkDetailUiState
buildObjectBookmarkDetailUiState(const SdoObjectBookmarkRow &row,
                                 const ObjectBookmarkDetailTexts &texts) {
  ObjectBookmarkDetailUiState state;
  state.readOnly = sdoObjectAccessIsReadOnly(row.access, texts.readOnlyText);
  state.hasTarget = sdoObjectBookmarkRowHasTarget(row);
  state.hasValue = !row.lastValue.isEmpty();
  state.severityKey = objectBookmarkDetailSeverityKey(row, texts);

  state.reuse = texts.fillTarget;
  if (state.hasValue && !state.readOnly) {
    state.reuse = texts.readyForWatchStartup;
  } else if (state.hasValue && state.readOnly) {
    state.reuse = texts.readOnlyWatchOnly;
  } else if (!state.hasTarget) {
    state.reuse = texts.missingAddress;
  } else {
    state.reuse = texts.noSavedValue;
  }

  const QString displayName =
      row.name.isEmpty()
          ? (row.slaveName.isEmpty() ? texts.unnamed : row.slaveName)
          : row.name;
  state.text =
      texts.summaryPattern
          .arg(row.positionText.isEmpty() ? QStringLiteral("?")
                                          : row.positionText)
          .arg(row.index.isEmpty() ? QStringLiteral("----") : row.index)
          .arg(row.subIndex.isEmpty() ? QStringLiteral("--") : row.subIndex)
          .arg(row.type.isEmpty() ? texts.typeFallback : row.type)
          .arg(row.access.isEmpty() ? texts.accessFallback : row.access)
          .arg(displayName)
          .arg(row.lastValue.isEmpty() ? texts.noValue : row.lastValue)
          .arg(row.source.isEmpty() ? texts.projectSource : row.source)
          .arg(state.reuse);

  state.tooltipLines << texts.selectedTitle;
  state.tooltipLines << QString("%1: #%2 %3")
                            .arg(texts.slaveLabel, row.positionText,
                                 row.slaveName);
  state.tooltipLines
      << QString("%1: %2:%3").arg(texts.objectLabel, row.index, row.subIndex);
  state.tooltipLines << QString("%1: %2").arg(texts.accessLabel, row.access);
  state.tooltipLines << QString("%1: %2").arg(texts.typeLabel, row.type);
  state.tooltipLines << QString("%1: %2").arg(texts.bitsLabel, row.bits);
  state.tooltipLines << QString("%1: %2").arg(texts.nameLabel, row.name);
  state.tooltipLines << QString("%1: %2").arg(texts.lastValueLabel,
                                              row.lastValue);
  state.tooltipLines << QString("%1: %2").arg(texts.sourceLabel, row.source);
  state.tooltipLines << QString("%1: %2").arg(texts.reuseLabel, state.reuse);
  state.tooltipLines << texts.localBoundary;
  state.tooltipLines << texts.executionBoundary;
  state.tooltip = state.tooltipLines.join('\n');
  return state;
}
