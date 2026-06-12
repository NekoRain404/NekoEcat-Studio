#pragma once

// Detail panel text for a selected SDO history row.


#include "adapters/SdoEvidenceTableAdapter.h"

#include <QString>
#include <QStringList>

struct SdoHistoryRowDetailTexts {
  QString unavailableText;
  QString unavailableTip;
  QString noSelectionText;
  QString noSelectionTip;
  QString timeFallback;
  QString actionFallback;
  QString typeFallback;
  QString noValue;
  QString noStatus;
  QString fillTargetOnly;
  QString reusableValue;
  QString reviewFailure;
  QString waitingRuntime;
  QString summaryPattern;
  QString selectedTitle;
  QString timeLabel;
  QString actionLabel;
  QString slaveLabel;
  QString objectLabel;
  QString typeLabel;
  QString valueLabel;
  QString statusLabel;
  QString detailLabel;
  QString reuseLabel;
  QString localBoundary;
  QString executionBoundary;
};

struct SdoHistoryRowDetailUiState {
  QString text;
  QString severityKey;
  QString reuse;
  bool failed = false;
  bool requested = false;
  bool complete = false;
  bool writeAction = false;
  bool verifyAction = false;
  bool readAction = false;
  bool hasReusableValue = false;
  bool hasTarget = false;
  QStringList tooltipLines;
  QString tooltip;
};

SdoHistoryRowDetailUiState
sdoHistoryRowDetailUnavailableState(const SdoHistoryRowDetailTexts &texts);
SdoHistoryRowDetailUiState
sdoHistoryRowDetailNoSelectionState(const SdoHistoryRowDetailTexts &texts);
QString sdoHistoryRowDetailSeverityKey(const SdoHistoryRow &row,
                                       const SdoHistoryRowDetailTexts &texts);
SdoHistoryRowDetailUiState
buildSdoHistoryRowDetailUiState(const SdoHistoryRow &row,
                                const SdoHistoryRowDetailTexts &texts);
