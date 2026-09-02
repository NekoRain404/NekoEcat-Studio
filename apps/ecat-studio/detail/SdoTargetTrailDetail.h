#pragma once

// Detail panel text for a selected SDO target trail row.


#include "adapters/SdoEvidenceTableAdapter.h"

#include <QString>
#include <QStringList>

// Localized text templates for the SDO target trail detail panel.
struct SdoTargetTrailDetailTexts {
    QString unavailableText;
    QString unavailableTip;
    QString noSelectionText;
    QString noSelectionTip;
    QString timeFallback;
    QString typeFallback;
    QString unknownSource;
    QString noValue;
    QString noWriteValue;
    QString restoreTarget;
    QString readyForReuse;
    QString watchBookmarkOnly;
    QString missingAddress;
    QString noSavedValue;
    QString summaryPattern;
    QString selectedTitle;
    QString timeLabel;
    QString slaveLabel;
    QString objectLabel;
    QString typeLabel;
    QString sourceLabel;
    QString valueLabel;
    QString writeValueLabel;
    QString startupCandidateLabel;
    QString detailLabel;
    QString reuseLabel;
    QString localBoundary;
    QString executionBoundary;
};

// Resolved trail detail state with startup value, reuse guidance, and tooltip.
struct SdoTargetTrailDetailState {
    QString text;
    QString severityKey;
    QString reuse;
    QString startupValue;
    bool hasTarget = false;
    bool canStartup = false;
    bool hasAnyValue = false;
    QStringList tooltipLines;
    QString tooltip;
};

SdoTargetTrailDetailState sdoTargetTrailDetailUnavailableState(const SdoTargetTrailDetailTexts& texts);
SdoTargetTrailDetailState sdoTargetTrailDetailNoSelectionState(const SdoTargetTrailDetailTexts& texts);
// Maps row attributes and startup eligibility to a severity key.
QString sdoTargetTrailDetailSeverityKey(const SdoTargetTrailRow& row, bool canStartup,
                                        const SdoTargetTrailDetailTexts& texts);
// Assembles the full trail detail state.
SdoTargetTrailDetailState buildSdoTargetTrailDetailState(const SdoTargetTrailRow& row, bool canStartup,
                                                         const SdoTargetTrailDetailTexts& texts);
