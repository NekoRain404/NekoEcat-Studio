#pragma once

#include "WatchStartupModel.h"

#include <QString>
#include <QStringList>

struct StartupSdoRowDetailTexts {
  QString unavailableText;
  QString unavailableTip;
  QString noSelectionText;
  QString noSelectionTip;
  QString defaultType;
  QString emptyValue;
  QString pendingStatus;
  QString noWatchValue;
  QString watchMismatch;
  QString noWatchEvidence;
  QString pendingComparison;
  QString watchMatches;
  QString reviewRow;
  QString summaryPattern;
  QString selectedTitle;
  QString rowLabel;
  QString slaveLabel;
  QString objectLabel;
  QString valueLabel;
  QString typeLabel;
  QString statusLabel;
  QString detailLabel;
  QString watchValueLabel;
  QString watchDeltaLabel;
  QString localBoundary;
  QString executionBoundary;
};

struct StartupSdoRowDetailUiState {
  QString text;
  QString severityKey;
  QString evidence;
  bool validationIssue = false;
  bool applying = false;
  bool watchDiff = false;
  bool noWatch = false;
  bool pending = false;
  bool match = false;
  bool missingTarget = false;
  QStringList tooltipLines;
  QString tooltip;
};

StartupSdoRowDetailUiState
startupSdoRowDetailUnavailableState(const StartupSdoRowDetailTexts &texts);
StartupSdoRowDetailUiState
startupSdoRowDetailNoSelectionState(const StartupSdoRowDetailTexts &texts);
QString startupSdoRowDetailSeverityKey(const WatchStartupStartupRow &row,
                                       const StartupSdoRowDetailTexts &texts);
StartupSdoRowDetailUiState
buildStartupSdoRowDetailUiState(const WatchStartupStartupRow &row,
                                const StartupSdoRowDetailTexts &texts);
