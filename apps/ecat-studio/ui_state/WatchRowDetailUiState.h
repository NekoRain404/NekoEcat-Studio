#pragma once

// Detail panel text for a selected Watch row.


#include "models/WatchStartupModel.h"

#include <QString>
#include <QStringList>

struct WatchRowDetailTexts {
  QString unavailableText;
  QString unavailableTip;
  QString noSelectionText;
  QString noSelectionTip;
  QString emptyValue;
  QString typeFallback;
  QString noBaseline;
  QString noComparison;
  QString startupMismatch;
  QString baselineDrift;
  QString changed;
  QString stableEvidence;
  QString cia402Candidate;
  QString genericSdo;
  QString matchText;
  QString pendingText;
  QString summaryPattern;
  QString selectedTitle;
  QString timeLabel;
  QString slaveLabel;
  QString objectLabel;
  QString typeLabel;
  QString modeLabel;
  QString valueLabel;
  QString decodedLabel;
  QString baselineLabel;
  QString baselineDeltaLabel;
  QString startupLabel;
  QString startupDeltaLabel;
  QString changedLabel;
  QString yesText;
  QString noText;
  QString driveEvidenceLabel;
  QString localBoundary;
  QString executionBoundary;
};

struct WatchRowDetailUiState {
  QString text;
  QString severityKey;
  QString displayValue;
  QString evidence;
  bool baselineDrift = false;
  bool startupDrift = false;
  bool missingValue = false;
  bool cia402 = false;
  QStringList tooltipLines;
  QString tooltip;
};

WatchRowDetailUiState
watchRowDetailUnavailableState(const WatchRowDetailTexts &texts);
WatchRowDetailUiState
watchRowDetailNoSelectionState(const WatchRowDetailTexts &texts);
bool watchRowDetailIsMatchText(const QString &text,
                               const WatchRowDetailTexts &texts);
bool watchRowDetailIsCia402(const WatchStartupWatchRow &row);
QString watchRowDetailSeverityKey(const WatchStartupWatchRow &row,
                                  const WatchRowDetailTexts &texts);
WatchRowDetailUiState
buildWatchRowDetailUiState(const WatchStartupWatchRow &row,
                           const WatchRowDetailTexts &texts);
