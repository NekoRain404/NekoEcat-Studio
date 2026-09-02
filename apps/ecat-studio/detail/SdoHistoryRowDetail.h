#pragma once

// Detail panel text for a selected SDO history row.


#include "adapters/SdoEvidenceTableAdapter.h"

#include <QString>
#include <QStringList>

// Localized text templates for the SDO history detail panel.
struct SdoHistoryRowDetailTexts {
    QString unavailableText;
    QString unavailableTip;
    QString noSelectionText;
    QString noSelectionTip;
    QString timeFallback;
    QString actionFallback;
    QString typeFallback;
    QString noValue;
    QString noStatus;
    QString fillTargetOnly;
    QString reusableValue;
    QString reviewFailure;
    QString waitingRuntime;
    QString summaryPattern;
    QString selectedTitle;
    QString timeLabel;
    QString actionLabel;
    QString slaveLabel;
    QString objectLabel;
    QString typeLabel;
    QString valueLabel;
    QString statusLabel;
    QString detailLabel;
    QString reuseLabel;
    QString localBoundary;
    QString executionBoundary;
};

// Resolved history detail state with status flags, reuse guidance, and tooltip.
struct SdoHistoryRowDetailState {
    QString text;
    QString severityKey;
    QString reuse;
    bool failed = false;
    bool requested = false;
    bool complete = false;
    bool writeAction = false;
    bool verifyAction = false;
    bool readAction = false;
    bool hasReusableValue = false;
    bool hasTarget = false;
    QStringList tooltipLines;
    QString tooltip;
};

SdoHistoryRowDetailState sdoHistoryRowDetailUnavailableState(const SdoHistoryRowDetailTexts& texts);
SdoHistoryRowDetailState sdoHistoryRowDetailNoSelectionState(const SdoHistoryRowDetailTexts& texts);
// Maps status and action to a severity key.
QString sdoHistoryRowDetailSeverityKey(const SdoHistoryRow& row, const SdoHistoryRowDetailTexts& texts);
// Assembles the full history detail state.
SdoHistoryRowDetailState buildSdoHistoryRowDetailState(const SdoHistoryRow& row, const SdoHistoryRowDetailTexts& texts);
