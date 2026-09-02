#pragma once

// Evidence summary card for the selected slave panel.


#include "models/SlaveEvidenceModel.h"

#include <QString>
#include <QStringList>

// Localized text templates for the slave evidence summary card.
struct SlaveEvidenceSummaryTexts {
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
struct SlaveEvidenceSummaryDetail {
    QString text;
    QString severityKey;
    int evidenceGroups = 0;
    QStringList tooltipLines;
    QString tooltip;
};

SlaveEvidenceSummaryDetail slaveEvidenceNoSelectionState(const SlaveEvidenceSummaryTexts& texts);
// Counts evidence categories with at least one row.
int slaveEvidenceSummaryGroupCount(const SlaveEvidenceInput& input);
// Maps evidence completeness and issues to a severity key.
QString slaveEvidenceSummarySeverityKey(const SlaveEvidenceInput& input, int topologyIssueCount);
// Assembles the evidence summary card state.
SlaveEvidenceSummaryDetail buildSlaveEvidenceSummaryDetail(const SlaveEvidenceInput& input, int topologyIssueCount,
                                                           const SlaveEvidenceSummaryTexts& texts);
