#pragma once

#include "SessionBriefModel.h"

#include <QString>
#include <QStringList>

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

struct SessionBriefUiRow {
  QStringList cells;
  QStringList tooltips;
  QString actionKey;
  SessionBriefStatus status = SessionBriefStatus::Info;
};

QString sessionBriefStatusText(SessionBriefStatus status,
                               const SessionBriefUiTexts &texts);
QString sessionBriefStatusColorKey(SessionBriefStatus status);
QStringList sessionBriefTableHeaders(const SessionBriefUiTexts &texts);
SessionBriefUiRow sessionBriefUiRow(const SessionBriefRow &row,
                                    const QString &area,
                                    const QString &evidence,
                                    const QString &next,
                                    const SessionBriefUiTexts &texts);
