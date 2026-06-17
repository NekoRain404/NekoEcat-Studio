#pragma once

// Detail panel text for a selected session brief row.


#include "models/SessionBriefModel.h"

#include <QString>
#include <QStringList>

// Localized strings for the session brief table and status mapping.
struct SessionBriefUiTexts {
  QString ready;
  QString action;
  QString warning;
  QString error;
  QString info;
  QString areaHeader;
  QString statusHeader;
  QString evidenceHeader;
  QString nextHeader;
  QString openLocalEvidenceTooltipPattern;
};

// UI-ready session brief row with cells, tooltips, and action key.
struct SessionBriefUiRow {
  QStringList cells;
  QStringList tooltips;
  QString actionKey;
  SessionBriefStatus status = SessionBriefStatus::Info;
};

// Maps status enum to localized display string.
QString sessionBriefStatusText(SessionBriefStatus status,
                               const SessionBriefUiTexts &texts);
// Maps status to its color/semantic key.
QString sessionBriefStatusColorKey(SessionBriefStatus status);
// Localized column headers for the session brief table.
QStringList sessionBriefTableHeaders(const SessionBriefUiTexts &texts);
// Converts a domain row into a UI row.
SessionBriefUiRow sessionBriefUiRow(const SessionBriefRow &row,
                                    const QString &area,
                                    const QString &evidence,
                                    const QString &next,
                                    const SessionBriefUiTexts &texts);
