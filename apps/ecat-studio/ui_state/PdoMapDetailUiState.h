#pragma once

// Detail panel text for a selected PDO map row.


#include "infra/ProcessDataTypes.h"

#include <QString>
#include <QStringList>

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

struct PdoMapDetailUiState {
  QString text;
  QString severityKey;
  QString direction;
  QString role;
  QString inferredType;
  bool cia402 = false;
  QStringList tooltipLines;
  QString tooltip;
};

PdoMapDetailUiState
pdoMapDetailUnavailableState(const PdoMapDetailTexts &texts);
PdoMapDetailUiState
pdoMapDetailNoSelectionState(const PdoMapDetailTexts &texts);
bool pdoMapDetailIsRxOutput(const PdoMapTableRow &row);
bool pdoMapDetailIsTxInput(const PdoMapTableRow &row);
bool pdoMapDetailIsCia402(const PdoMapTableRow &row);
QString pdoMapDetailSeverityKey(const PdoMapTableRow &row);
PdoMapDetailUiState buildPdoMapDetailUiState(const PdoMapTableRow &row,
                                             int selectedPosition,
                                             const PdoMapDetailTexts &texts);
