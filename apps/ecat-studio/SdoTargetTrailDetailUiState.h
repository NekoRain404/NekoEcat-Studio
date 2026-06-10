#pragma once

#include "SdoEvidenceTableAdapter.h"

#include <QString>
#include <QStringList>

struct SdoTargetTrailDetailTexts {
  QString unavailableText;
  QString unavailableTip;
  QString noSelectionText;
  QString noSelectionTip;
  QString timeFallback;
  QString typeFallback;
  QString unknownSource;
  QString noValue;
  QString noWriteValue;
  QString restoreTarget;
  QString readyForReuse;
  QString watchBookmarkOnly;
  QString missingAddress;
  QString noSavedValue;
  QString summaryPattern;
  QString selectedTitle;
  QString timeLabel;
  QString slaveLabel;
  QString objectLabel;
  QString typeLabel;
  QString sourceLabel;
  QString valueLabel;
  QString writeValueLabel;
  QString startupCandidateLabel;
  QString detailLabel;
  QString reuseLabel;
  QString localBoundary;
  QString executionBoundary;
};

struct SdoTargetTrailDetailUiState {
  QString text;
  QString severityKey;
  QString reuse;
  QString startupValue;
  bool hasTarget = false;
  bool canStartup = false;
  bool hasAnyValue = false;
  QStringList tooltipLines;
  QString tooltip;
};

SdoTargetTrailDetailUiState
sdoTargetTrailDetailUnavailableState(const SdoTargetTrailDetailTexts &texts);
SdoTargetTrailDetailUiState
sdoTargetTrailDetailNoSelectionState(const SdoTargetTrailDetailTexts &texts);
QString sdoTargetTrailDetailSeverityKey(const SdoTargetTrailRow &row,
                                        bool canStartup,
                                        const SdoTargetTrailDetailTexts &texts);
SdoTargetTrailDetailUiState
buildSdoTargetTrailDetailUiState(const SdoTargetTrailRow &row, bool canStartup,
                                 const SdoTargetTrailDetailTexts &texts);
