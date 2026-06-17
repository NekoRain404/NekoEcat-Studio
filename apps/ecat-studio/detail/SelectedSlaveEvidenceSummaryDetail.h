#pragma once

// Evidence summary card for the selected slave panel.


#include "models/SlaveEvidenceModel.h"

#include <QString>
#include <QStringList>

// Localized text templates for the slave evidence summary card.
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

// Resolved summary card state with evidence group count, severity, and tooltips.
struct SelectedSlaveEvidenceSummaryDetail {
  QString text;
  QString severityKey;
  int evidenceGroups = 0;
  QStringList tooltipLines;
  QString tooltip;
};

SelectedSlaveEvidenceSummaryDetail selectedSlaveEvidenceNoSelectionState(
    const SelectedSlaveEvidenceSummaryTexts &texts);
// Counts evidence categories with at least one row.
int selectedSlaveEvidenceSummaryGroupCount(const SlaveEvidenceInput &input);
// Maps evidence completeness and issues to a severity key.
QString selectedSlaveEvidenceSummarySeverityKey(const SlaveEvidenceInput &input,
                                                int topologyIssueCount);
// Assembles the evidence summary card state.
SelectedSlaveEvidenceSummaryDetail buildSelectedSlaveEvidenceSummaryDetail(
    const SlaveEvidenceInput &input, int topologyIssueCount,
    const SelectedSlaveEvidenceSummaryTexts &texts);
