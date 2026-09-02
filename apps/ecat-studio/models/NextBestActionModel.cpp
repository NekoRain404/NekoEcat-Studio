// Next-best-action recommendation engine for the commissioning overview.
#include "NextBestActionModel.h"

// Priority-ordered decision tree: returns the highest-urgency action the user should take next.
NextBestActionDecision chooseNextBestAction(const NextBestActionInput& input) {
    const CommissioningWorkflowInput& workflow = input.workflow;
    if (!workflow.connected) {
        return {NextBestActionKind::Connect, NextBestActionSeverity::Action};
    }
    if (input.hasDiagnosticError) {
        return {NextBestActionKind::Diagnostics, NextBestActionSeverity::Error};
    }
    if (!workflow.hasSlaves) {
        return {NextBestActionKind::Rescan, NextBestActionSeverity::Action};
    }
    if (!workflow.hasSelectedSlave) {
        return {NextBestActionKind::SelectSlave, NextBestActionSeverity::Action};
    }
    if (!workflow.hasSdoRows) {
        return {NextBestActionKind::LoadOd, NextBestActionSeverity::Action};
    }
    if (workflow.hasFailedOdEvidence) {
        return {NextBestActionKind::FailedOdEvidence, NextBestActionSeverity::Warning};
    }
    if (!workflow.hasPdoRows) {
        return {NextBestActionKind::LoadPdo, NextBestActionSeverity::Action};
    }
    if (!workflow.hasWatchRows) {
        return {NextBestActionKind::AddWatch, NextBestActionSeverity::Action};
    }
    if (workflow.hasStartupWatchDiffs) {
        return {NextBestActionKind::StartupDiffs, NextBestActionSeverity::Warning};
    }
    if (!workflow.hasConsistencyCheck || workflow.hasConsistencyBlockingIssues) {
        if (workflow.hasConsistencyCheck && input.consistencyBlockingIssueRow >= 0) {
            return {NextBestActionKind::ConsistencyEvidenceIssue, NextBestActionSeverity::Warning};
        }
        return {NextBestActionKind::Consistency, workflow.hasConsistencyBlockingIssues
                                                     ? NextBestActionSeverity::Warning
                                                     : NextBestActionSeverity::Action};
    }
    if (!workflow.freeRunEnabled && !workflow.hasFreeRunRows) {
        return {NextBestActionKind::FreeRun, NextBestActionSeverity::Action};
    }
    if (input.matrixP0 + input.matrixP1 + input.matrixP2 > 0) {
        return {NextBestActionKind::MatrixReview,
                input.matrixP0 + input.matrixP1 > 0 ? NextBestActionSeverity::Warning : NextBestActionSeverity::Action};
    }
    return {NextBestActionKind::CommandPalette, NextBestActionSeverity::Neutral};
}

// Serializes action kind to a stable string key for persistence/QML.
QString nextBestActionKey(NextBestActionKind kind) {
    switch (kind) {
        case NextBestActionKind::Connect:
            return QStringLiteral("connect");
        case NextBestActionKind::Diagnostics:
            return QStringLiteral("diagnostics");
        case NextBestActionKind::Rescan:
            return QStringLiteral("rescan");
        case NextBestActionKind::SelectSlave:
            return QStringLiteral("selectSlave");
        case NextBestActionKind::LoadOd:
            return QStringLiteral("loadOd");
        case NextBestActionKind::FailedOdEvidence:
            return QStringLiteral("failedOdEvidence");
        case NextBestActionKind::LoadPdo:
            return QStringLiteral("loadPdo");
        case NextBestActionKind::AddWatch:
            return QStringLiteral("addWatch");
        case NextBestActionKind::StartupDiffs:
            return QStringLiteral("startupDiffs");
        case NextBestActionKind::ConsistencyEvidenceIssue:
            return QStringLiteral("consistencyEvidenceIssue");
        case NextBestActionKind::Consistency:
            return QStringLiteral("consistency");
        case NextBestActionKind::FreeRun:
            return QStringLiteral("freeRun");
        case NextBestActionKind::MatrixReview:
            return QStringLiteral("matrixReview");
        case NextBestActionKind::CommandPalette:
            return QStringLiteral("commandPalette");
    }
    return QStringLiteral("commandPalette");
}

// Serializes severity enum to a stable string key.
QString nextBestActionSeverityKey(NextBestActionSeverity severity) {
    switch (severity) {
        case NextBestActionSeverity::Ok:
            return QStringLiteral("ok");
        case NextBestActionSeverity::Action:
            return QStringLiteral("action");
        case NextBestActionSeverity::Warning:
            return QStringLiteral("warning");
        case NextBestActionSeverity::Error:
            return QStringLiteral("error");
        case NextBestActionSeverity::Neutral:
            return QStringLiteral("neutral");
    }
    return QStringLiteral("neutral");
}
