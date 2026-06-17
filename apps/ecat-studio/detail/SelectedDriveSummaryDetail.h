#pragma once

// CiA 402 drive summary card for the selected slave panel.


#include "models/Cia402DriveModel.h"
#include "models/WatchStartupModel.h"

#include <QString>
#include <QStringList>
#include <QVector>

// Localized text templates for the drive summary card.
struct SelectedDriveSummaryTexts {
  QString noWatchEvidence;
  QString noCia402Evidence;
  QString summaryPattern;
  QString modePattern;
  QString controlwordPattern;
};

// Extracted CiA 402 object values for a single slave.
struct SelectedDriveSummaryEvidence {
  QString status;
  QString mode;
  QString error;
  QString controlword;
};

// Resolved drive summary card state with evidence, severity, and display parts.
struct SelectedDriveSummaryDetail {
  QString text;
  QString severityKey;
  QStringList parts;
  SelectedDriveSummaryEvidence evidence;
};

SelectedDriveSummaryDetail
selectedDriveNoWatchEvidenceState(const SelectedDriveSummaryTexts &texts);
// Extracts CiA 402 evidence from watch rows for a specific slave.
SelectedDriveSummaryEvidence
selectedDriveSummaryEvidence(const QVector<WatchStartupWatchRow> &watchRows,
                             int position);
QString
selectedDriveSummarySeverityKey(const SelectedDriveSummaryEvidence &evidence);
SelectedDriveSummaryDetail
buildSelectedDriveSummaryDetail(const QVector<WatchStartupWatchRow> &watchRows,
                                 int position,
                                 const SelectedDriveSummaryTexts &texts);
// Recommends the next controlword from the current statusword.
Cia402ControlwordRecommendation selectedDriveControlwordRecommendation(
    const QVector<WatchStartupWatchRow> &watchRows, int position);
