#pragma once

// Detail panel text for a selected Free Run entry row.


#include "infra/ProcessDataTypes.h"

#include <QString>
#include <QStringList>

// Localized text templates for the free-run entry detail panel.
struct FreeRunEntryDetailTexts {
    QString unavailableText;
    QString unavailableTip;
    QString noSelectionText;
    QString noSelectionTip;
    QString unknown;
    QString directionFallback;
    QString unnamed;
    QString emptyValue;
    QString noMapEvidence;
    QString outputBoundary;
    QString inputBoundary;
    QString mappedText;
    QString summaryPattern;
    QStringList nameSourceMarkers;
    QString selectedTitle;
    QString slaveLabel;
    QString syncManagerLabel;
    QString directionLabel;
    QString pdoLabel;
    QString objectLabel;
    QString nameLabel;
    QString nameSourceLabel;
    QString locationLabel;
    QString rawLabel;
    QString decodedLabel;
    QString meaningLabel;
    QString mapStatusLabel;
    QString mapDetailLabel;
    QString changedLabel;
    QString yesText;
    QString noText;
    QString boundaryLabel;
    QString localBoundary;
    QString executionBoundary;
};

// Resolved detail panel state with severity, boundary, name source, and tooltip.
struct FreeRunEntryDetailState {
    QString text;
    QString severityKey;
    QString nameSource;
    QString boundary;
    QStringList tooltipLines;
    QString tooltip;
};

FreeRunEntryDetailState freeRunEntryDetailUnavailableState(const FreeRunEntryDetailTexts& texts);
FreeRunEntryDetailState freeRunEntryDetailNoSelectionState(const FreeRunEntryDetailTexts& texts);
// Whether the entry direction is RX/output.
bool freeRunEntryDetailIsOutputLike(const FreeRunEntryTableRow& row);
// Extracts the origin of the variable name from map detail.
QString freeRunEntryDetailNameSource(const FreeRunEntryTableRow& row, const FreeRunEntryDetailTexts& texts);
// Maps status flags to a severity key.
QString freeRunEntryDetailSeverityKey(const FreeRunEntryTableRow& row, const FreeRunEntryDetailTexts& texts);
// Assembles the full detail panel state.
FreeRunEntryDetailState buildFreeRunEntryDetailState(const FreeRunEntryTableRow& row,
                                                     const FreeRunEntryDetailTexts& texts);
