#pragma once

// Detail panel text for a selected state machine recommendation row.


#include "adapters/StateMachineTableAdapter.h"

#include <QString>
#include <QStringList>

// Localized text templates for the state machine detail panel.
struct StateMachineRowDetailTexts {
    QString unavailableText;
    QString unavailableTip;
    QString noSelectionText;
    QString noSelectionTip;
    QString unnamed;
    QString unknown;
    QString none;
    QString noRisk;
    QString reviewEvidence;
    QString confirmedRequestBoundary;
    QString localReviewBoundaryLabel;
    QString opBoundary;
    QString safeopBoundary;
    QString preopBoundary;
    QString summaryPattern;
    QString selectedTitle;
    QString slaveLabel;
    QString currentStateLabel;
    QString recommendedStateLabel;
    QString evidenceLabel;
    QString driveLabel;
    QString startupLabel;
    QString processLabel;
    QString riskLabel;
    QString boundaryLabel;
    QString localBoundary;
    QString executionBoundary;
};

// Resolved state machine detail with boundary, display fields, and tooltip.
struct StateMachineRowDetailState {
    QString text;
    QString severityKey;
    QString boundary;
    QString displayPosition;
    QString displayName;
    QString displayCurrent;
    QString displayRecommended;
    QString displayRisk;
    QString displayAction;
    bool hasRecommendation = false;
    bool hasRisk = false;
    bool severeRisk = false;
    QStringList tooltipLines;
    QString tooltip;
};

StateMachineRowDetailState stateMachineRowDetailUnavailableState(const StateMachineRowDetailTexts& texts);
StateMachineRowDetailState stateMachineRowDetailNoSelectionState(const StateMachineRowDetailTexts& texts);
// Maps risk and recommendation to a severity key.
QString stateMachineRowDetailSeverityKey(const StateMachineTableRow& row, const StateMachineRowDetailTexts& texts);
// Assembles the full state machine detail state.
StateMachineRowDetailState buildStateMachineRowDetailState(const StateMachineTableRow& row,
                                                           const StateMachineRowDetailTexts& texts);
