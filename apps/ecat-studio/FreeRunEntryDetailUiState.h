#pragma once

#include "ProcessDataTypes.h"

#include <QString>
#include <QStringList>

struct FreeRunEntryDetailTexts {
  QString unavailableText;
  QString unavailableTip;
  QString noSelectionText;
  QString noSelectionTip;
  QString unknown;
  QString directionFallback;
  QString unnamed;
  QString emptyValue;
  QString noMapEvidence;
  QString outputBoundary;
  QString inputBoundary;
  QString mappedText;
  QString summaryPattern;
  QStringList nameSourceMarkers;
  QString selectedTitle;
  QString slaveLabel;
  QString syncManagerLabel;
  QString directionLabel;
  QString pdoLabel;
  QString objectLabel;
  QString nameLabel;
  QString nameSourceLabel;
  QString locationLabel;
  QString rawLabel;
  QString decodedLabel;
  QString meaningLabel;
  QString mapStatusLabel;
  QString mapDetailLabel;
  QString changedLabel;
  QString yesText;
  QString noText;
  QString boundaryLabel;
  QString localBoundary;
  QString executionBoundary;
};

struct FreeRunEntryDetailUiState {
  QString text;
  QString severityKey;
  QString nameSource;
  QString boundary;
  QStringList tooltipLines;
  QString tooltip;
};

FreeRunEntryDetailUiState
freeRunEntryDetailUnavailableState(const FreeRunEntryDetailTexts &texts);
FreeRunEntryDetailUiState
freeRunEntryDetailNoSelectionState(const FreeRunEntryDetailTexts &texts);
bool freeRunEntryDetailIsOutputLike(const FreeRunEntryTableRow &row);
QString freeRunEntryDetailNameSource(const FreeRunEntryTableRow &row,
                                     const FreeRunEntryDetailTexts &texts);
QString freeRunEntryDetailSeverityKey(const FreeRunEntryTableRow &row,
                                      const FreeRunEntryDetailTexts &texts);
FreeRunEntryDetailUiState
buildFreeRunEntryDetailUiState(const FreeRunEntryTableRow &row,
                               const FreeRunEntryDetailTexts &texts);
