#include "SelectedSlaveEvidenceSummaryUiState.h"

SelectedSlaveEvidenceSummaryUiState selectedSlaveEvidenceNoSelectionState(
    const SelectedSlaveEvidenceSummaryTexts &texts) {
  return {.text = texts.selectSlaveText,
          .severityKey = QStringLiteral("neutral")};
}

int selectedSlaveEvidenceSummaryGroupCount(const SlaveEvidenceInput &input) {
  int groups = 0;
  groups += input.identityRows > 0 ? 1 : 0;
  groups += input.odRows > 0 ? 1 : 0;
  groups += input.pdoRows > 0 ? 1 : 0;
  groups += input.watchValueRows > 0 ? 1 : 0;
  groups += input.processRows > 0 ? 1 : 0;
  return groups;
}

QString selectedSlaveEvidenceSummarySeverityKey(const SlaveEvidenceInput &input,
                                                int topologyIssueCount) {
  const int groups = selectedSlaveEvidenceSummaryGroupCount(input);
  if (topologyIssueCount > 0 || input.startupDiffs > 0 || input.mapIssues > 0) {
    return QStringLiteral("warning");
  }
  if (groups >= 4) {
    return QStringLiteral("ok");
  }
  if (groups >= 2) {
    return QStringLiteral("action");
  }
  return QStringLiteral("neutral");
}

SelectedSlaveEvidenceSummaryUiState buildSelectedSlaveEvidenceSummaryUiState(
    const SlaveEvidenceInput &input, int topologyIssueCount,
    const SelectedSlaveEvidenceSummaryTexts &texts) {
  SelectedSlaveEvidenceSummaryUiState state;
  state.evidenceGroups = selectedSlaveEvidenceSummaryGroupCount(input);
  state.severityKey =
      selectedSlaveEvidenceSummarySeverityKey(input, topologyIssueCount);

  state.text = texts.summaryPattern.arg(state.evidenceGroups)
                   .arg(input.identityRows > 0 ? texts.ready : texts.missing)
                   .arg(input.odRows)
                   .arg(input.pdoRows)
                   .arg(input.watchValueRows)
                   .arg(input.watchRows)
                   .arg(input.startupRows)
                   .arg(input.startupDiffs)
                   .arg(input.processRows)
                   .arg(input.mapIssues);

  state.tooltipLines << texts.scorePattern.arg(state.evidenceGroups);
  if (input.identityRows <= 0) {
    state.tooltipLines << texts.missingIdentity;
  }
  if (input.odRows <= 0) {
    state.tooltipLines << texts.missingOd;
  }
  if (input.pdoRows <= 0) {
    state.tooltipLines << texts.missingPdo;
  }
  if (input.watchValueRows <= 0) {
    state.tooltipLines << texts.missingWatch;
  }
  if (input.processRows <= 0) {
    state.tooltipLines << texts.missingProcess;
  }
  if (input.startupDiffs > 0) {
    state.tooltipLines << texts.startupDiffPattern.arg(input.startupDiffs);
  }
  if (input.mapIssues > 0) {
    state.tooltipLines << texts.mapIssuePattern.arg(input.mapIssues);
  }
  if (topologyIssueCount > 0) {
    state.tooltipLines << texts.topologyIssuePattern.arg(topologyIssueCount);
  }
  state.tooltip = state.tooltipLines.join('\n');
  return state;
}
