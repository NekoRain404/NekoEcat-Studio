// Detail panel text for a selected Watch row.
#include "detail/WatchRowDetail.h"

#include "utils/TextHelpers.h"

// Neutral state when the watch table is not available.
WatchRowDetailState watchRowDetailUnavailableState(const WatchRowDetailTexts& texts) {
    return {.text = texts.unavailableText,
            // Set severityKey field
            .severityKey = QStringLiteral("neutral"),
            // Set tooltip field
            .tooltip = texts.unavailableTip};
}

// Neutral state prompting the user to select a watch row.
WatchRowDetailState watchRowDetailNoSelectionState(const WatchRowDetailTexts& texts) {
    return {.text = texts.noSelectionText,
            // Set severityKey field
            .severityKey = QStringLiteral("neutral"),
            // Set tooltip field
            .tooltip = texts.noSelectionTip};
}

// Whether the delta text represents a match/empty/pending state (not a real change).
bool watchRowDetailIsMatchText(const QString& text, const WatchRowDetailTexts& texts) {
    const QString normalized = text.trimmed().toLower();
    return normalized.isEmpty() || normalized == QStringLiteral("0") || normalized == QStringLiteral("match") ||
           normalized == QStringLiteral("pending") || text == texts.matchText || text == texts.pendingText ||
           text == QStringLiteral("匹配") || text == QStringLiteral("待比较");
}

// Whether the watch row addresses a CiA 402 drive profile object.
bool watchRowDetailIsCia402(const WatchStartupWatchRow& row) {
    return row.mode.contains(QStringLiteral("cia"), Qt::CaseInsensitive) ||
           normalizeHexText(row.index, 4).startsWith(QStringLiteral("0x60"));
}

// Maps startup drift, baseline drift, missing value, and change flags to a severity key.
QString watchRowDetailSeverityKey(const WatchStartupWatchRow& row, const WatchRowDetailTexts& texts) {
    const bool missingValue = row.value.isEmpty();
    const bool baselineDrift = !watchRowDetailIsMatchText(row.baselineDelta, texts);
    const bool startupDrift = !watchRowDetailIsMatchText(row.startupDelta, texts);

    if (startupDrift) {
        return QStringLiteral("error");
    }
    if (baselineDrift || missingValue) {
        return QStringLiteral("warning");
    }
    if (row.changed) {
        return QStringLiteral("action");
    }
    if (!row.value.isEmpty()) {
        return QStringLiteral("ok");
    }
    return QStringLiteral("neutral");
}

// Assembles the full watch detail state: drift flags, CiA 402 detection, evidence, and tooltip.
WatchRowDetailState buildWatchRowDetailState(const WatchStartupWatchRow& row, const WatchRowDetailTexts& texts) {
    WatchRowDetailState state;
    state.missingValue = row.value.isEmpty();
    state.baselineDrift = !watchRowDetailIsMatchText(row.baselineDelta, texts);
    state.startupDrift = !watchRowDetailIsMatchText(row.startupDelta, texts);
    state.cia402 = watchRowDetailIsCia402(row);
    state.severityKey = watchRowDetailSeverityKey(row, texts);
    state.displayValue = !row.decoded.isEmpty() ? row.decoded : (row.value.isEmpty() ? texts.emptyValue : row.value);
    state.evidence = state.startupDrift ? texts.startupMismatch
                                        : (state.baselineDrift ? texts.baselineDrift
                                                               : (row.changed ? texts.changed : texts.stableEvidence));

    const QString slave = row.position >= 0 ? QString::number(row.position) : QString();
    state.text = texts.summaryPattern.arg(slave.isEmpty() ? QString::number(row.row) : slave)
                     .arg(row.index.isEmpty() ? QStringLiteral("----") : row.index)
                     .arg(row.subIndex.isEmpty() ? QStringLiteral("--") : row.subIndex)
                     .arg(row.type.isEmpty() ? texts.typeFallback : row.type)
                     .arg(state.displayValue)
                     .arg(row.baselineDelta.isEmpty() ? texts.noBaseline : row.baselineDelta)
                     .arg(row.startupDelta.isEmpty() ? texts.noComparison : row.startupDelta)
                     .arg(state.evidence);

    state.tooltipLines << texts.selectedTitle;
    state.tooltipLines << QString("%1: %2").arg(texts.timeLabel, row.time);
    state.tooltipLines << QString("%1: #%2").arg(texts.slaveLabel, slave);
    state.tooltipLines << QString("%1: %2:%3").arg(texts.objectLabel, row.index, row.subIndex);
    state.tooltipLines << QString("%1: %2").arg(texts.typeLabel, row.type);
    state.tooltipLines << QString("%1: %2").arg(texts.modeLabel, row.mode);
    state.tooltipLines << QString("%1: %2").arg(texts.valueLabel, row.value);
    state.tooltipLines << QString("%1: %2").arg(texts.decodedLabel, row.decoded);
    state.tooltipLines << QString("%1: %2").arg(texts.baselineLabel, row.baseline);
    state.tooltipLines << QString("%1: %2").arg(texts.baselineDeltaLabel, row.baselineDelta);
    state.tooltipLines << QString("%1: %2").arg(texts.startupLabel, row.startup);
    state.tooltipLines << QString("%1: %2").arg(texts.startupDeltaLabel, row.startupDelta);
    state.tooltipLines << QString("%1: %2").arg(texts.changedLabel, row.changed ? texts.yesText : texts.noText);
    state.tooltipLines << QString("%1: %2").arg(texts.driveEvidenceLabel,
                                                state.cia402 ? texts.cia402Candidate : texts.genericSdo);
    state.tooltipLines << texts.localBoundary;
    state.tooltipLines << texts.executionBoundary;
    state.tooltip = state.tooltipLines.join('\n');
    return state;
}
