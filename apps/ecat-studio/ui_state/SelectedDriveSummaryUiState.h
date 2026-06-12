#pragma once

// CiA 402 drive summary card for the selected slave panel.


#include "models/Cia402DriveModel.h"
#include "models/WatchStartupModel.h"

#include <QString>
#include <QStringList>
#include <QVector>

struct SelectedDriveSummaryTexts {
  QString noWatchEvidence;
  QString noCia402Evidence;
  QString summaryPattern;
  QString modePattern;
  QString controlwordPattern;
};

struct SelectedDriveSummaryEvidence {
  QString status;
  QString mode;
  QString error;
  QString controlword;
};

struct SelectedDriveSummaryUiState {
  QString text;
  QString severityKey;
  QStringList parts;
  SelectedDriveSummaryEvidence evidence;
};

SelectedDriveSummaryUiState
selectedDriveNoWatchEvidenceState(const SelectedDriveSummaryTexts &texts);
SelectedDriveSummaryEvidence
selectedDriveSummaryEvidence(const QVector<WatchStartupWatchRow> &watchRows,
                             int position);
QString
selectedDriveSummarySeverityKey(const SelectedDriveSummaryEvidence &evidence);
SelectedDriveSummaryUiState
buildSelectedDriveSummaryUiState(const QVector<WatchStartupWatchRow> &watchRows,
                                 int position,
                                 const SelectedDriveSummaryTexts &texts);
Cia402ControlwordRecommendation selectedDriveControlwordRecommendation(
    const QVector<WatchStartupWatchRow> &watchRows, int position);
