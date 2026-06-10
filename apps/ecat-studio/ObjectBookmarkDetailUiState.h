#pragma once

#include "SdoEvidenceTableAdapter.h"

#include <QString>
#include <QStringList>

struct ObjectBookmarkDetailTexts {
  QString unavailableText;
  QString unavailableTip;
  QString noSelectionText;
  QString noSelectionTip;
  QString readOnlyText;
  QString typeFallback;
  QString accessFallback;
  QString unnamed;
  QString noValue;
  QString projectSource;
  QString fillTarget;
  QString readyForWatchStartup;
  QString readOnlyWatchOnly;
  QString missingAddress;
  QString noSavedValue;
  QString summaryPattern;
  QString selectedTitle;
  QString slaveLabel;
  QString objectLabel;
  QString accessLabel;
  QString typeLabel;
  QString bitsLabel;
  QString nameLabel;
  QString lastValueLabel;
  QString sourceLabel;
  QString reuseLabel;
  QString localBoundary;
  QString executionBoundary;
};

struct ObjectBookmarkDetailUiState {
  QString text;
  QString severityKey;
  QString reuse;
  bool readOnly = false;
  bool hasTarget = false;
  bool hasValue = false;
  QStringList tooltipLines;
  QString tooltip;
};

ObjectBookmarkDetailUiState
objectBookmarkDetailUnavailableState(const ObjectBookmarkDetailTexts &texts);
ObjectBookmarkDetailUiState
objectBookmarkDetailNoSelectionState(const ObjectBookmarkDetailTexts &texts);
QString objectBookmarkDetailSeverityKey(const SdoObjectBookmarkRow &row,
                                        const ObjectBookmarkDetailTexts &texts);
ObjectBookmarkDetailUiState
buildObjectBookmarkDetailUiState(const SdoObjectBookmarkRow &row,
                                 const ObjectBookmarkDetailTexts &texts);
