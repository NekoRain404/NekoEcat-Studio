// Workspace tab badge text templates and severity color keys.
#include "detail/WorkspaceTabBadgeDetail.h"

// Formats badge text as "Label !Count" for issues or "Label Count" for normal counts.
QString workspaceTabBadgeText(const QString& label, int count, bool issue) {
    if (count <= 0) {
        return label;
    }
    return QString("%1 %2%3").arg(label, issue ? QStringLiteral("!") : QString(), QString::number(count));
}

// Builds all 8 workspace tab badges with formatted text and detailed tooltips.
WorkspaceTabBadgeDetail buildWorkspaceTabBadgeDetail(const WorkspaceTabBadgeCounts& counts,
                                                     const WorkspaceTabBadgeTexts& texts) {
    WorkspaceTabBadgeDetail state;
    const int matrixIssues = counts.matrixP0 + counts.matrixP1 + counts.matrixP2;
    const int consistencyIssues = counts.consistencyErrors + counts.consistencyWarnings;
    const int diagnosticIssues = counts.diagnosticErrors + counts.diagnosticWarnings;

    state.overview = {
        workspaceTabBadgeText(texts.overview, matrixIssues, counts.matrixP0 + counts.matrixP1 > 0),
        texts.overviewTipPattern.arg(counts.matrixP0).arg(counts.matrixP1).arg(counts.matrixP2).arg(counts.matrixP3)};
    state.watch = {workspaceTabBadgeText(texts.watch, counts.watchRows),
                   texts.watchTipPattern.arg(counts.watchRows).arg(counts.watchStartupDiffs)};
    state.startupSdo = {workspaceTabBadgeText(texts.startupSdo,
                                              counts.startupDiffs > 0 ? counts.startupDiffs : counts.startupRows,
                                              counts.startupDiffs > 0),
                        texts.startupSdoTipPattern.arg(counts.startupRows).arg(counts.startupDiffs)};
    state.freeRun = {workspaceTabBadgeText(texts.freeRun, counts.freeRunRows),
                     texts.freeRunTipPattern.arg(counts.freeRunRows)};
    state.ioVariables = {workspaceTabBadgeText(texts.ioVariables, counts.ioIssues > 0 ? counts.ioIssues : counts.ioRows,
                                               counts.ioIssues > 0),
                         texts.ioVariablesTipPattern.arg(counts.ioRows).arg(counts.ioIssues)};
    state.consistency = {workspaceTabBadgeText(texts.consistency,
                                               consistencyIssues > 0 ? consistencyIssues : counts.consistencyRows,
                                               consistencyIssues > 0),
                         texts.consistencyTipPattern.arg(counts.consistencyErrors)
                             .arg(counts.consistencyWarnings)
                             .arg(counts.consistencyInfos)
                             .arg(counts.consistencyReady)};
    state.stateMachine = {workspaceTabBadgeText(texts.stateMachine, counts.stateRiskRows, counts.stateRiskRows > 0),
                          texts.stateMachineTipPattern.arg(counts.stateRiskRows)};
    state.diagnostics = {workspaceTabBadgeText(texts.diagnostics,
                                               diagnosticIssues > 0 ? diagnosticIssues : counts.diagnosticRows,
                                               diagnosticIssues > 0),
                         texts.diagnosticsTipPattern.arg(counts.diagnosticErrors)
                             .arg(counts.diagnosticWarnings)
                             .arg(counts.diagnosticInfos)};
    return state;
}
