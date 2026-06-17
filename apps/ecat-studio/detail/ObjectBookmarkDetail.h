#pragma once

// Detail panel text for a selected object bookmark row.


#include "adapters/SdoEvidenceTableAdapter.h"

#include <QString>
#include <QStringList>

// Localized text templates for the object bookmark detail panel.
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

// Resolved bookmark detail state with severity, reuse guidance, and tooltip.
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
// Maps row attributes to a severity key.
QString objectBookmarkDetailSeverityKey(const SdoObjectBookmarkRow &row,
                                        const ObjectBookmarkDetailTexts &texts);
// Assembles the full bookmark detail state.
ObjectBookmarkDetailUiState
buildObjectBookmarkDetailUiState(const SdoObjectBookmarkRow &row,
                                 const ObjectBookmarkDetailTexts &texts);
