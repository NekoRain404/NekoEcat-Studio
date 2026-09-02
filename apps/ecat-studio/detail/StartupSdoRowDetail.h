#pragma once

// Detail panel text for a selected Startup SDO row.


#include "models/WatchStartupModel.h"

#include <QString>
#include <QStringList>

// Localized text templates for the startup SDO detail panel.
struct StartupSdoRowDetailTexts {
    QString unavailableText;
    QString unavailableTip;
    QString noSelectionText;
    QString noSelectionTip;
    QString defaultType;
    QString emptyValue;
    QString pendingStatus;
    QString noWatchValue;
    QString watchMismatch;
    QString noWatchEvidence;
    QString pendingComparison;
    QString watchMatches;
    QString reviewRow;
    QString summaryPattern;
    QString selectedTitle;
    QString rowLabel;
    QString slaveLabel;
    QString objectLabel;
    QString valueLabel;
    QString typeLabel;
    QString statusLabel;
    QString detailLabel;
    QString watchValueLabel;
    QString watchDeltaLabel;
    QString localBoundary;
    QString executionBoundary;
};

// Resolved startup detail state with status flags, evidence, and tooltip.
struct StartupSdoRowDetailState {
    QString text;
    QString severityKey;
    QString evidence;
    bool validationIssue = false;
    bool applying = false;
    bool watchDiff = false;
    bool noWatch = false;
    bool pending = false;
    bool match = false;
    bool missingTarget = false;
    QStringList tooltipLines;
    QString tooltip;
};

StartupSdoRowDetailState startupSdoRowDetailUnavailableState(const StartupSdoRowDetailTexts& texts);
StartupSdoRowDetailState startupSdoRowDetailNoSelectionState(const StartupSdoRowDetailTexts& texts);
// Maps status and delta to a severity key.
QString startupSdoRowDetailSeverityKey(const WatchStartupStartupRow& row, const StartupSdoRowDetailTexts& texts);
// Assembles the full startup detail state.
StartupSdoRowDetailState buildStartupSdoRowDetailState(const WatchStartupStartupRow& row,
                                                       const StartupSdoRowDetailTexts& texts);
