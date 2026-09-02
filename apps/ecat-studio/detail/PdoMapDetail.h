#pragma once

// Detail panel text for a selected PDO map row.


#include "infra/ProcessDataTypes.h"

#include <QString>
#include <QStringList>

// Localized text templates for the PDO map detail panel.
struct PdoMapDetailTexts {
    QString unavailableText;
    QString unavailableTip;
    QString noSelectionText;
    QString noSelectionTip;
    QString directionRxOutput;
    QString directionTxInput;
    QString directionUnknown;
    QString roleRxOutput;
    QString roleTxInput;
    QString roleGeneric;
    QString typeFallback;
    QString unnamed;
    QString cia402Candidate;
    QString genericEntry;
    QString summaryPattern;
    QString selectedTitle;
    QString slaveLabel;
    QString syncManagerLabel;
    QString pdoLabel;
    QString objectLabel;
    QString bitsLabel;
    QString inferredTypeLabel;
    QString nameLabel;
    QString directionLabel;
    QString roleLabel;
    QString driveEvidenceLabel;
    QString localBoundary;
    QString executionBoundary;
};

// Resolved PDO map detail state with direction, role, type, and CiA 402 flag.
struct PdoMapDetailState {
    QString text;
    QString severityKey;
    QString direction;
    QString role;
    QString inferredType;
    bool cia402 = false;
    QStringList tooltipLines;
    QString tooltip;
};

PdoMapDetailState pdoMapDetailUnavailableState(const PdoMapDetailTexts& texts);
PdoMapDetailState pdoMapDetailNoSelectionState(const PdoMapDetailTexts& texts);
// Whether the PDO indicates RxPDO/output direction.
bool pdoMapDetailIsRxOutput(const PdoMapTableRow& row);
// Whether the PDO indicates TxPDO/input direction.
bool pdoMapDetailIsTxInput(const PdoMapTableRow& row);
// Whether the entry matches CiA 402 drive profile objects.
bool pdoMapDetailIsCia402(const PdoMapTableRow& row);
// Maps row attributes to a severity key.
QString pdoMapDetailSeverityKey(const PdoMapTableRow& row);
// Assembles the full PDO map detail state.
PdoMapDetailState buildPdoMapDetailState(const PdoMapTableRow& row, int selectedPosition,
                                         const PdoMapDetailTexts& texts);
