// Evidence summary card for the selected slave panel.
#include "detail/SlaveEvidenceSummaryDetail.h"

// Neutral state prompting the user to select a slave.
SlaveEvidenceSummaryDetail slaveEvidenceNoSelectionState(const SlaveEvidenceSummaryTexts& texts) {
    return {.text = texts.selectSlaveText,
            // Set severityKey field
            .severityKey = QStringLiteral("neutral")};
}

// Counts how many evidence categories have at least one row (identity, OD, PDO, watch, process).
int slaveEvidenceSummaryGroupCount(const SlaveEvidenceInput& input) {
    int groups = 0;
    groups += input.identityRows > 0 ? 1 : 0;
    groups += input.odRows > 0 ? 1 : 0;
    groups += input.pdoRows > 0 ? 1 : 0;
    groups += input.watchValueRows > 0 ? 1 : 0;
    groups += input.processRows > 0 ? 1 : 0;
    return groups;
}

// Maps evidence group count, topology issues, startup diffs, and map issues to severity.
QString slaveEvidenceSummarySeverityKey(const SlaveEvidenceInput& input, int topologyIssueCount) {
    const int groups = slaveEvidenceSummaryGroupCount(input);
    if (topologyIssueCount > 0 || input.startupDiffs > 0 || input.mapIssues > 0) {
        return QStringLiteral("warning");
    }
    if (groups >= 4) {
        return QStringLiteral("ok");
    }
    if (groups >= 2) {
        return QStringLiteral("action");
    }
    return QStringLiteral("neutral");
}

// Assembles the evidence summary card with group counts, severity, and missing-evidence tooltips.
SlaveEvidenceSummaryDetail buildSlaveEvidenceSummaryDetail(const SlaveEvidenceInput& input, int topologyIssueCount,
                                                           const SlaveEvidenceSummaryTexts& texts) {
    SlaveEvidenceSummaryDetail state;
    state.evidenceGroups = slaveEvidenceSummaryGroupCount(input);
    state.severityKey = slaveEvidenceSummarySeverityKey(input, topologyIssueCount);

    state.text = texts.summaryPattern.arg(state.evidenceGroups)
                     .arg(input.identityRows > 0 ? texts.ready : texts.missing)
                     .arg(input.odRows)
                     .arg(input.pdoRows)
                     .arg(input.watchValueRows)
                     .arg(input.watchRows)
                     .arg(input.startupRows)
                     .arg(input.startupDiffs)
                     .arg(input.processRows)
                     .arg(input.mapIssues);

    state.tooltipLines << texts.scorePattern.arg(state.evidenceGroups);
    if (input.identityRows <= 0) {
        state.tooltipLines << texts.missingIdentity;
    }
    if (input.odRows <= 0) {
        state.tooltipLines << texts.missingOd;
    }
    if (input.pdoRows <= 0) {
        state.tooltipLines << texts.missingPdo;
    }
    if (input.watchValueRows <= 0) {
        state.tooltipLines << texts.missingWatch;
    }
    if (input.processRows <= 0) {
        state.tooltipLines << texts.missingProcess;
    }
    if (input.startupDiffs > 0) {
        state.tooltipLines << texts.startupDiffPattern.arg(input.startupDiffs);
    }
    if (input.mapIssues > 0) {
        state.tooltipLines << texts.mapIssuePattern.arg(input.mapIssues);
    }
    if (topologyIssueCount > 0) {
        state.tooltipLines << texts.topologyIssuePattern.arg(topologyIssueCount);
    }
    state.tooltip = state.tooltipLines.join('\n');
    return state;
}
