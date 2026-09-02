// Detail panel text for a selected session brief row.
#include "detail/SessionBriefDetail.h"

// Maps a session brief status enum to its localized display string.
QString sessionBriefStatusText(SessionBriefStatus status, const SessionBriefUiTexts& texts) {
    switch (status) {
        case SessionBriefStatus::Ready:
            return texts.ready;
        case SessionBriefStatus::Action:
            return texts.action;
        case SessionBriefStatus::Warning:
            return texts.warning;
        case SessionBriefStatus::Error:
            return texts.error;
        case SessionBriefStatus::Info:
            return texts.info;
    }
    return texts.info;
}

// Maps a session brief status to its color/semantic key.
QString sessionBriefStatusColorKey(SessionBriefStatus status) {
    switch (status) {
        case SessionBriefStatus::Ready:
            return QStringLiteral("ready");
        case SessionBriefStatus::Warning:
            return QStringLiteral("warning");
        case SessionBriefStatus::Error:
            return QStringLiteral("error");
        case SessionBriefStatus::Action:
            return QStringLiteral("action");
        case SessionBriefStatus::Info:
            return QStringLiteral("info");
    }
    return QStringLiteral("info");
}

// Returns localized column headers for the session brief table.
QStringList sessionBriefTableHeaders(const SessionBriefUiTexts& texts) {
    return {texts.areaHeader, texts.statusHeader, texts.evidenceHeader, texts.nextHeader};
}

// Converts a domain row into a UI row with cells, tooltips, and action key.
SessionBriefUiRow sessionBriefUiRow(const SessionBriefRow& row, const QString& area, const QString& evidence,
                                    const QString& next, const SessionBriefUiTexts& texts) {
    SessionBriefUiRow uiRow;
    uiRow.cells = {area, sessionBriefStatusText(row.status, texts), evidence, next};
    uiRow.actionKey = row.actionKey;
    uiRow.status = row.status;
    for (const QString& cell : uiRow.cells) {
        uiRow.tooltips << texts.openLocalEvidenceTooltipPattern.arg(cell);
    }
    return uiRow;
}
