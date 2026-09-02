#pragma once

// Detail panel text for a selected Watch row.


#include "models/WatchStartupModel.h"

#include <QString>
#include <QStringList>

// Localized text templates for the watch row detail panel.
struct WatchRowDetailTexts {
    QString unavailableText;
    QString unavailableTip;
    QString noSelectionText;
    QString noSelectionTip;
    QString emptyValue;
    QString typeFallback;
    QString noBaseline;
    QString noComparison;
    QString startupMismatch;
    QString baselineDrift;
    QString changed;
    QString stableEvidence;
    QString cia402Candidate;
    QString genericSdo;
    QString matchText;
    QString pendingText;
    QString summaryPattern;
    QString selectedTitle;
    QString timeLabel;
    QString slaveLabel;
    QString objectLabel;
    QString typeLabel;
    QString modeLabel;
    QString valueLabel;
    QString decodedLabel;
    QString baselineLabel;
    QString baselineDeltaLabel;
    QString startupLabel;
    QString startupDeltaLabel;
    QString changedLabel;
    QString yesText;
    QString noText;
    QString driveEvidenceLabel;
    QString localBoundary;
    QString executionBoundary;
};

// Resolved watch detail state with drift flags, CiA 402 detection, and tooltip.
struct WatchRowDetailState {
    QString text;
    QString severityKey;
    QString displayValue;
    QString evidence;
    bool baselineDrift = false;
    bool startupDrift = false;
    bool missingValue = false;
    bool cia402 = false;
    QStringList tooltipLines;
    QString tooltip;
};

WatchRowDetailState watchRowDetailUnavailableState(const WatchRowDetailTexts& texts);
WatchRowDetailState watchRowDetailNoSelectionState(const WatchRowDetailTexts& texts);
// Whether delta text represents a non-issue (match/empty/pending).
bool watchRowDetailIsMatchText(const QString& text, const WatchRowDetailTexts& texts);
// Whether the row addresses a CiA 402 object.
bool watchRowDetailIsCia402(const WatchStartupWatchRow& row);
// Maps drift and change flags to a severity key.
QString watchRowDetailSeverityKey(const WatchStartupWatchRow& row, const WatchRowDetailTexts& texts);
// Assembles the full watch detail state.
WatchRowDetailState buildWatchRowDetailState(const WatchStartupWatchRow& row, const WatchRowDetailTexts& texts);
