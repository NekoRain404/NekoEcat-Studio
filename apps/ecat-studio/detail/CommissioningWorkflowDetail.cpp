// Summary and scope-filter state for the commissioning workflow panel.
#include "detail/CommissioningWorkflowDetail.h"

// Maps a status enum to its localized display string.
QString commissioningWorkflowStatusText(CommissioningWorkflowStatus status, const CommissioningWorkflowTexts& texts) {
    switch (status) {
        case CommissioningWorkflowStatus::Ready:
            return texts.ready;
        case CommissioningWorkflowStatus::Action:
            return texts.action;
        case CommissioningWorkflowStatus::Blocked:
            return texts.blocked;
    }
    return texts.blocked;
}

// Returns the color/semantic key string for a status enum value.
QString commissioningWorkflowColorKey(CommissioningWorkflowStatus status) {
    switch (status) {
        case CommissioningWorkflowStatus::Ready:
            return QStringLiteral("ready");
        case CommissioningWorkflowStatus::Action:
            return QStringLiteral("action");
        case CommissioningWorkflowStatus::Blocked:
            return QStringLiteral("blocked");
    }
    return QStringLiteral("blocked");
}

// Maps each workflow step to its boundary kind and detail text for the step boundary display.
CommissioningWorkflowStepBoundary commissioningWorkflowStepBoundary(CommissioningWorkflowStep step,
                                                                    const CommissioningWorkflowTexts& texts) {
    switch (step) {
        case CommissioningWorkflowStep::ConnectRuntime:
            return {texts.onlineRuntimeAction, texts.connectRuntimeBoundary};
        case CommissioningWorkflowStep::ScanTopology:
            return {texts.onlineTopologyAction, texts.scanTopologyBoundary};
        case CommissioningWorkflowStep::SelectSlave:
            return {texts.localTargetSelection, texts.selectSlaveBoundary};
        case CommissioningWorkflowStep::InspectObjectDictionary:
            return {texts.onlineOdRead, texts.inspectObjectDictionaryBoundary};
        case CommissioningWorkflowStep::ReviewObjectDictionaryEvidence:
            return {texts.localEvidenceReview, texts.reviewObjectDictionaryEvidenceBoundary};
        case CommissioningWorkflowStep::ReviewPdoMap:
            return {texts.onlinePdoRead, texts.reviewPdoMapBoundary};
        case CommissioningWorkflowStep::MonitorWatch:
            return {texts.localWatchEdit, texts.monitorWatchBoundary};
        case CommissioningWorkflowStep::ReviewStartupDiffs:
            return {texts.localStartupReview, texts.reviewStartupDiffsBoundary};
        case CommissioningWorkflowStep::RunConsistencyGate:
            return {texts.consistencyGate, texts.runConsistencyGateBoundary};
        case CommissioningWorkflowStep::ValidateProcessImage:
            return {texts.processDataAction, texts.validateProcessImageBoundary};
    }
    return {texts.localEvidenceReview, texts.reviewObjectDictionaryEvidenceBoundary};
}

// Returns the localized column header labels for the workflow table.
QStringList commissioningWorkflowHeaders(const CommissioningWorkflowTexts& texts) {
    return {texts.phaseHeader, texts.statusHeader,   texts.stepHeader,
            texts.riskHeader,  texts.evidenceHeader, texts.nextActionHeader};
}

// Converts a domain row into a UI row with cells, tooltip, and color key.
CommissioningWorkflowUiRow commissioningWorkflowUiRow(const CommissioningWorkflowRow& row,
                                                      const CommissioningWorkflowTexts& texts) {
    CommissioningWorkflowUiRow uiRow;
    const QString status = commissioningWorkflowStatusText(row.status, texts);
    uiRow.cells = {row.phase, status, row.step, row.risk, row.evidence, row.action};
    uiRow.tooltip = texts.tooltipPattern.arg(row.phase, status, row.risk, row.evidence, row.action);
    uiRow.colorKey = commissioningWorkflowColorKey(row.status);
    uiRow.status = row.status;
    return uiRow;
}

// Flattens UI rows into a list of string lists for QTableWidget population.
QList<QStringList> commissioningWorkflowTableRows(const QVector<CommissioningWorkflowUiRow>& rows) {
    QList<QStringList> tableRows;
    tableRows.reserve(rows.size());
    // Iterate over collection
    for (const auto& row : rows) {
        tableRows.append(row.cells);
    }
    return tableRows;
}

// Tallies ready/action/blocked counts across all UI rows for summary badges.
CommissioningWorkflowStats commissioningWorkflowStats(const QVector<CommissioningWorkflowUiRow>& rows) {
    CommissioningWorkflowStats stats;
    // Iterate over collection
    for (const auto& row : rows) {
        switch (row.status) {
            case CommissioningWorkflowStatus::Ready:
                ++stats.ready;
                break;
            case CommissioningWorkflowStatus::Action:
                ++stats.action;
                break;
            case CommissioningWorkflowStatus::Blocked:
                ++stats.blocked;
                break;
        }
    }
    return stats;
}
