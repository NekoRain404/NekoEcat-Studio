#pragma once

// Detail panel text for a selected I/O variable row.


#include "infra/ProcessDataTypes.h"

#include <QString>
#include <QStringList>

struct IoVariableDetailTexts {
  QString unavailableText;
  QString unavailableTip;
  QString noSelectionText;
  QString noSelectionTip;
  QString readyText;
  QString noValue;
  QString directionFallback;
  QString unnamedSignal;
  QString noComparison;
  QString noMapEvidence;
  QString notReviewed;
  QString startupMismatch;
  QString mapIssue;
  QString plcReview;
  QString missingValue;
  QString changed;
  QString readyEvidence;
  QString summaryPattern;
  QString selectedTitle;
  QString slaveLabel;
  QString directionLabel;
  QString symbolLabel;
  QString aliasLabel;
  QString objectLabel;
  QString bitsLabel;
  QString pdoLabel;
  QString sourceLabel;
  QString rawLabel;
  QString decodedLabel;
  QString meaningLabel;
  QString watchLabel;
  QString startupLabel;
  QString mapLabel;
  QString changedLabel;
  QString plcLabel;
  QString tagsLabel;
  QString noteLabel;
  QString signalStateLabel;
  QString localBoundary;
  QString executionBoundary;
};

struct IoVariableDetailUiState {
  QString text;
  QString severityKey;
  QString signalState;
  QStringList tooltipLines;
  QString tooltip;
};

IoVariableDetailUiState
ioVariableDetailUnavailableState(const IoVariableDetailTexts &texts);
IoVariableDetailUiState
ioVariableDetailNoSelectionState(const IoVariableDetailTexts &texts);
QString ioVariableDetailSeverityKey(const IoVariableTableRow &row,
                                    const QString &readyText);
QString ioVariableDetailSignalState(const IoVariableTableRow &row,
                                    const IoVariableDetailTexts &texts);
IoVariableDetailUiState
buildIoVariableDetailUiState(const IoVariableTableRow &row,
                             const IoVariableDetailTexts &texts);
