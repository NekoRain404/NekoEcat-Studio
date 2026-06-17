#pragma once

// Detail panel text for a selected consistency issue row.


#include <QString>
#include <QStringList>

// Localized text templates for the consistency detail panel.
struct ConsistencyDetailTexts {
  QString unavailableText;
  QString unavailableTip;
  QString selectVisibleRowText;
  QString selectVisibleRowTip;
  QString summaryPattern;
  QString selectedRowTitle;
  QString levelLabel;
  QString scopeLabel;
  QString targetLabel;
  QString evidenceLabel;
  QString expectedLabel;
  QString actualLabel;
  QString actionLabel;
  QString routeLabel;
  QString levelFallback;
  QString scopeFallback;
  QString targetFallback;
  QString expectedFallback;
  QString actualFallback;
  QString routeIo;
  QString routeTopology;
  QString routeStartup;
  QString routeWatchOrIo;
  QString routeReady;
  QString localBoundary;
  QString executionBoundary;
};

// Extracted row data for a consistency issue.
struct ConsistencyDetailRow {
  QString level;
  QString scope;
  QString target;
  QString evidence;
  QString expected;
  QString actual;
  QString action;
};

// Resolved detail panel state with severity, route, and tooltip.
struct ConsistencyDetailUiState {
  QString text;
  QString severityKey;
  QString route;
  QStringList tooltipLines;
  QString tooltip;
};

// Neutral state when the consistency table is unavailable.
ConsistencyDetailUiState
consistencyDetailUnavailableState(const ConsistencyDetailTexts &texts);
// Neutral state prompting user to select a row.
ConsistencyDetailUiState
consistencyDetailNoSelectionState(const ConsistencyDetailTexts &texts);
// Maps level text to a canonical severity key.
QString consistencyDetailSeverityKey(const QString &level);
// Determines the navigation target tab from issue scope.
QString consistencyDetailRoute(const ConsistencyDetailRow &row,
                               const ConsistencyDetailTexts &texts);
// Assembles the full detail panel state.
ConsistencyDetailUiState
buildConsistencyDetailUiState(const ConsistencyDetailRow &row,
                              const ConsistencyDetailTexts &texts);
