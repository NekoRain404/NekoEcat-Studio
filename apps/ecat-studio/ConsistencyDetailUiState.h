#pragma once

#include <QString>
#include <QStringList>

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

struct ConsistencyDetailRow {
  QString level;
  QString scope;
  QString target;
  QString evidence;
  QString expected;
  QString actual;
  QString action;
};

struct ConsistencyDetailUiState {
  QString text;
  QString severityKey;
  QString route;
  QStringList tooltipLines;
  QString tooltip;
};

ConsistencyDetailUiState
consistencyDetailUnavailableState(const ConsistencyDetailTexts &texts);
ConsistencyDetailUiState
consistencyDetailNoSelectionState(const ConsistencyDetailTexts &texts);
QString consistencyDetailSeverityKey(const QString &level);
QString consistencyDetailRoute(const ConsistencyDetailRow &row,
                               const ConsistencyDetailTexts &texts);
ConsistencyDetailUiState
buildConsistencyDetailUiState(const ConsistencyDetailRow &row,
                              const ConsistencyDetailTexts &texts);
