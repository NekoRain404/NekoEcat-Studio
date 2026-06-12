#pragma once

// Evidence summary card for the selected slave panel.


#include "models/SlaveEvidenceModel.h"

#include <QString>
#include <QStringList>

struct SelectedSlaveEvidenceSummaryTexts {
  QString selectSlaveText;
  QString ready;
  QString missing;
  QString summaryPattern;
  QString scorePattern;
  QString missingIdentity;
  QString missingOd;
  QString missingPdo;
  QString missingWatch;
  QString missingProcess;
  QString startupDiffPattern;
  QString mapIssuePattern;
  QString topologyIssuePattern;
};

struct SelectedSlaveEvidenceSummaryUiState {
  QString text;
  QString severityKey;
  int evidenceGroups = 0;
  QStringList tooltipLines;
  QString tooltip;
};

SelectedSlaveEvidenceSummaryUiState selectedSlaveEvidenceNoSelectionState(
    const SelectedSlaveEvidenceSummaryTexts &texts);
int selectedSlaveEvidenceSummaryGroupCount(const SlaveEvidenceInput &input);
QString selectedSlaveEvidenceSummarySeverityKey(const SlaveEvidenceInput &input,
                                                int topologyIssueCount);
SelectedSlaveEvidenceSummaryUiState buildSelectedSlaveEvidenceSummaryUiState(
    const SlaveEvidenceInput &input, int topologyIssueCount,
    const SelectedSlaveEvidenceSummaryTexts &texts);
