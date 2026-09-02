// Detail panel text for a selected SDO target trail row.
#include "detail/SdoTargetTrailDetail.h"

// Neutral state when the target trail table is not available.
SdoTargetTrailDetailState sdoTargetTrailDetailUnavailableState(const SdoTargetTrailDetailTexts& texts) {
    return {.text = texts.unavailableText,
            // Set severityKey field
            .severityKey = QStringLiteral("neutral"),
            // Set tooltip field
            .tooltip = texts.unavailableTip};
}

// Neutral state prompting the user to select a trail row.
SdoTargetTrailDetailState sdoTargetTrailDetailNoSelectionState(const SdoTargetTrailDetailTexts& texts) {
    return {.text = texts.noSelectionText,
            // Set severityKey field
            .severityKey = QStringLiteral("neutral"),
            // Set tooltip field
            .tooltip = texts.noSelectionTip};
}

// Maps target validity, startup eligibility, and value presence to a severity key.
QString sdoTargetTrailDetailSeverityKey(const SdoTargetTrailRow& row, bool canStartup,
                                        const SdoTargetTrailDetailTexts&) {
    const bool hasTarget = sdoTargetTrailRowHasTarget(row);
    const bool hasAnyValue = !row.value.isEmpty() || !row.writeValue.isEmpty();

    if (!hasTarget) {
        return QStringLiteral("warning");
    }
    if (canStartup) {
        return QStringLiteral("ok");
    }
    if (hasAnyValue) {
        return QStringLiteral("action");
    }
    return QStringLiteral("neutral");
}

// Assembles the full trail detail state: startup value, reuse guidance, severity, and tooltip.
SdoTargetTrailDetailState buildSdoTargetTrailDetailState(const SdoTargetTrailRow& row, bool canStartup,
                                                         const SdoTargetTrailDetailTexts& texts) {
    SdoTargetTrailDetailState state;
    state.startupValue = sdoTargetTrailRowStartupValue(row);
    state.hasTarget = sdoTargetTrailRowHasTarget(row);
    state.canStartup = canStartup;
    state.hasAnyValue = !row.value.isEmpty() || !row.writeValue.isEmpty();
    state.severityKey = sdoTargetTrailDetailSeverityKey(row, canStartup, texts);

    state.reuse = texts.restoreTarget;
    if (state.canStartup) {
        state.reuse = texts.readyForReuse;
    } else if (state.hasAnyValue) {
        state.reuse = texts.watchBookmarkOnly;
    } else if (!state.hasTarget) {
        state.reuse = texts.missingAddress;
    } else {
        state.reuse = texts.noSavedValue;
    }

    state.text = texts.summaryPattern.arg(row.time.isEmpty() ? texts.timeFallback : row.time)
                     .arg(row.positionText.isEmpty() ? QStringLiteral("?") : row.positionText)
                     .arg(row.index.isEmpty() ? QStringLiteral("----") : row.index)
                     .arg(row.subIndex.isEmpty() ? QStringLiteral("--") : row.subIndex)
                     .arg(row.type.isEmpty() ? texts.typeFallback : row.type)
                     .arg(row.source.isEmpty() ? texts.unknownSource : row.source)
                     .arg(row.value.isEmpty() ? texts.noValue : row.value)
                     .arg(row.writeValue.isEmpty() ? texts.noWriteValue : row.writeValue)
                     .arg(state.reuse);

    state.tooltipLines << texts.selectedTitle;
    state.tooltipLines << QString("%1: %2").arg(texts.timeLabel, row.time);
    state.tooltipLines << QString("%1: #%2").arg(texts.slaveLabel, row.positionText);
    state.tooltipLines << QString("%1: %2:%3").arg(texts.objectLabel, row.index, row.subIndex);
    state.tooltipLines << QString("%1: %2").arg(texts.typeLabel, row.type);
    state.tooltipLines << QString("%1: %2").arg(texts.sourceLabel, row.source);
    state.tooltipLines << QString("%1: %2").arg(texts.valueLabel, row.value);
    state.tooltipLines << QString("%1: %2").arg(texts.writeValueLabel, row.writeValue);
    state.tooltipLines << QString("%1: %2").arg(texts.startupCandidateLabel, state.startupValue);
    state.tooltipLines << QString("%1: %2").arg(texts.detailLabel, row.detail);
    state.tooltipLines << QString("%1: %2").arg(texts.reuseLabel, state.reuse);
    state.tooltipLines << texts.localBoundary;
    state.tooltipLines << texts.executionBoundary;
    state.tooltip = state.tooltipLines.join('\n');
    return state;
}
