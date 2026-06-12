#pragma once

// Detail panel text for a selected slave evidence matrix row.


#include "models/SlaveEvidenceModel.h"

#include <QString>
#include <QStringList>

// All localized strings for the slave evidence matrix and detail panel.
struct SlaveEvidenceUiTexts {
  QString p0Fault;
  QString p1Risk;
  QString p2Action;
  QString p3Ready;
  QString reviewOd;
  QString loadPdo;
  QString addWatch;
  QString reviewStartup;
  QString validateProcess;
  QString reviewRisk;
  QString ready;
  QString identityMissing;
  QString odMissing;
  QString pdoMissing;
  QString watchMissing;
  QString processMissing;
  QString startupDiffPattern;
  QString pdoMapIssuePattern;
  QString topologyBaselineIssue;
  QString driveFaultEvidence;
  QString unknownEvidenceRisk;
  QString unnamed;
  QString unknown;
  QString missing;
  QString noRows;
  QString none;
  QString watchValuesPattern;
  QString startupRowsPattern;
  QString processRowsPattern;
  QString modePattern;
  QString slavePattern;
  QString priorityPattern;
  QString statePattern;
  QString identityRowsPattern;
  QString odRowsPattern;
  QString pdoRowsPattern;
  QString watchValuesDetailPattern;
  QString startupRowsDetailPattern;
  QString processRowsDetailPattern;
  QString drivePattern;
  QString nextPattern;
  QString riskPattern;
  QString priorityHeader;
  QString slaveHeader;
  QString nameHeader;
  QString stateHeader;
  QString readinessHeader;
  QString odHeader;
  QString pdoHeader;
  QString watchHeader;
  QString startupHeader;
  QString processHeader;
  QString riskHeader;
  QString nextHeader;
};

// UI-ready row with table cells and detail lines for the evidence panel.
struct SlaveEvidenceUiRow {
  QStringList cells;
  QStringList detailLines;
};

// Maps priority tier to localized display string.
QString slaveEvidencePriorityText(SlaveEvidencePriority priority,
                                  const SlaveEvidenceUiTexts &texts);
// Maps next-action enum to localized display string.
QString slaveEvidenceNextActionText(SlaveEvidenceNextAction action,
                                    const SlaveEvidenceUiTexts &texts);
// Maps risk kind to localized description.
QString slaveEvidenceRiskText(const SlaveEvidenceRisk &risk,
                              const SlaveEvidenceUiTexts &texts);
// Converts all risks to localized text labels.
QStringList slaveEvidenceRiskTexts(const QVector<SlaveEvidenceRisk> &risks,
                                   const SlaveEvidenceUiTexts &texts);
// Display name with unnamed fallback.
QString slaveEvidenceDisplayName(const QString &name,
                                 const SlaveEvidenceUiTexts &texts);
// Display state with unknown fallback.
QString slaveEvidenceDisplayState(const QString &state,
                                  const SlaveEvidenceUiTexts &texts);
// All 12 localized column headers.
QStringList slaveEvidenceMatrixHeaders(const SlaveEvidenceUiTexts &texts);
// Converts a domain row into UI cells and detail lines.
SlaveEvidenceUiRow slaveEvidenceUiRow(const SlaveEvidenceRow &row,
                                      const SlaveEvidenceUiTexts &texts);
