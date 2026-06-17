// Detail panel text for a selected consistency issue row.
#include "ConsistencyDetailUiState.h"

// Returns a neutral state when the consistency table is not available.
ConsistencyDetailUiState
consistencyDetailUnavailableState(const ConsistencyDetailTexts &texts) {
  return {.text = texts.unavailableText,
    // Set severityKey field
          .severityKey = QStringLiteral("neutral"),
    // Set tooltip field
          .tooltip = texts.unavailableTip};
}

// Returns a neutral state prompting the user to select a visible row.
ConsistencyDetailUiState
consistencyDetailNoSelectionState(const ConsistencyDetailTexts &texts) {
  return {.text = texts.selectVisibleRowText,
    // Set severityKey field
          .severityKey = QStringLiteral("neutral"),
    // Set tooltip field
          .tooltip = texts.selectVisibleRowTip};
}

// Maps a localized severity level text to a canonical severity key (error, warning, ok, action).
QString consistencyDetailSeverityKey(const QString &level) {
  const QString normalized = level.toLower();
  if (normalized.contains("error") || level.contains("错误")) {
    return QStringLiteral("error");
  }
  if (normalized.contains("warning") || level.contains("警告")) {
    return QStringLiteral("warning");
  }
  if (normalized.contains("ready") || level.contains("就绪")) {
    return QStringLiteral("ok");
  }
  return QStringLiteral("action");
}

// Determines which workspace tab to navigate to based on the issue scope and content.
QString consistencyDetailRoute(const ConsistencyDetailRow &row,
                               const ConsistencyDetailTexts &texts) {
  const QString scopeLower = row.scope.toLower();
  const QString combined =
      QString("%1 %2 %3 %4")
          .arg(row.scope, row.evidence, row.target, row.action)
          .toLower();

  if (scopeLower.contains("topology") || row.scope.contains("拓扑")) {
    return texts.routeTopology;
  }
  if (scopeLower.contains("startup") || row.scope.contains("启动") ||
      combined.contains("startup") || combined.contains("启动")) {
    return texts.routeStartup;
  }
  if (combined.contains("watch") || combined.contains("值证据") ||
      combined.contains("missing") || combined.contains("缺失")) {
    return texts.routeWatchOrIo;
  }
  if (scopeLower.contains("project") || row.scope.contains("工程")) {
    return texts.routeReady;
  }
  return texts.routeIo;
}

// Assembles the full detail panel state: summary text, severity, route, and tooltip.
ConsistencyDetailUiState
buildConsistencyDetailUiState(const ConsistencyDetailRow &row,
                              const ConsistencyDetailTexts &texts) {
  ConsistencyDetailUiState state;
  state.severityKey = consistencyDetailSeverityKey(row.level);
  state.route = consistencyDetailRoute(row, texts);
  state.text =
      texts.summaryPattern
          .arg(row.level.isEmpty() ? texts.levelFallback : row.level)
          .arg(row.scope.isEmpty() ? texts.scopeFallback : row.scope)
          .arg(row.target.isEmpty() ? texts.targetFallback : row.target)
          .arg(row.expected.isEmpty() ? texts.expectedFallback : row.expected)
          .arg(row.actual.isEmpty() ? texts.actualFallback : row.actual)
          .arg(row.action.isEmpty() ? state.route : row.action);

  state.tooltipLines << texts.selectedRowTitle;
  state.tooltipLines << QString("%1: %2").arg(texts.levelLabel, row.level);
  state.tooltipLines << QString("%1: %2").arg(texts.scopeLabel, row.scope);
  state.tooltipLines << QString("%1: %2").arg(texts.targetLabel, row.target);
  state.tooltipLines << QString("%1: %2").arg(texts.evidenceLabel,
                                              row.evidence);
  state.tooltipLines << QString("%1: %2").arg(texts.expectedLabel,
                                              row.expected);
  state.tooltipLines << QString("%1: %2").arg(texts.actualLabel, row.actual);
  state.tooltipLines << QString("%1: %2").arg(texts.actionLabel, row.action);
  state.tooltipLines << QString("%1: %2").arg(texts.routeLabel, state.route);
  state.tooltipLines << texts.localBoundary;
  state.tooltipLines << texts.executionBoundary;
  state.tooltip = state.tooltipLines.join('\n');
  return state;
}
