// Detail panel text for a selected slave evidence matrix row.
#include "SlaveEvidenceUiState.h"

// Maps a priority tier enum to its localized display string.
QString slaveEvidencePriorityText(SlaveEvidencePriority priority,
                                  const SlaveEvidenceUiTexts &texts) {
  switch (priority) {
  case SlaveEvidencePriority::Fault:
    return texts.p0Fault;
  case SlaveEvidencePriority::Risk:
    return texts.p1Risk;
  case SlaveEvidencePriority::Action:
    return texts.p2Action;
  case SlaveEvidencePriority::Ready:
    return texts.p3Ready;
  }
  return texts.p3Ready;
}

// Maps a next-action enum to its localized display string.
QString slaveEvidenceNextActionText(SlaveEvidenceNextAction action,
                                    const SlaveEvidenceUiTexts &texts) {
  switch (action) {
  case SlaveEvidenceNextAction::ReviewOd:
    return texts.reviewOd;
  case SlaveEvidenceNextAction::LoadPdo:
    return texts.loadPdo;
  case SlaveEvidenceNextAction::AddWatch:
    return texts.addWatch;
  case SlaveEvidenceNextAction::ReviewStartup:
    return texts.reviewStartup;
  case SlaveEvidenceNextAction::ValidateProcess:
    return texts.validateProcess;
  case SlaveEvidenceNextAction::ReviewRisk:
    return texts.reviewRisk;
  case SlaveEvidenceNextAction::Ready:
    return texts.ready;
  }
  return texts.reviewRisk;
}

// Maps a risk kind (with optional count) to its localized description.
QString slaveEvidenceRiskText(const SlaveEvidenceRisk &risk,
                              const SlaveEvidenceUiTexts &texts) {
  switch (risk.kind) {
  case SlaveEvidenceRiskKind::IdentityMissing:
    return texts.identityMissing;
  case SlaveEvidenceRiskKind::OdMissing:
    return texts.odMissing;
  case SlaveEvidenceRiskKind::PdoMissing:
    return texts.pdoMissing;
  case SlaveEvidenceRiskKind::WatchMissing:
    return texts.watchMissing;
  case SlaveEvidenceRiskKind::ProcessMissing:
    return texts.processMissing;
  case SlaveEvidenceRiskKind::StartupDiff:
    return texts.startupDiffPattern.arg(risk.count);
  case SlaveEvidenceRiskKind::PdoMapIssue:
    return texts.pdoMapIssuePattern.arg(risk.count);
  case SlaveEvidenceRiskKind::TopologyBaselineIssue:
    return texts.topologyBaselineIssue;
  case SlaveEvidenceRiskKind::DriveFault:
    return texts.driveFaultEvidence;
  }
  return texts.unknownEvidenceRisk;
}

// Converts all risks to localized text labels.
QStringList slaveEvidenceRiskTexts(const QVector<SlaveEvidenceRisk> &risks,
                                   const SlaveEvidenceUiTexts &texts) {
  QStringList labels;
  labels.reserve(risks.size());
    // Iterate over collection
  for (const auto &risk : risks) {
    labels << slaveEvidenceRiskText(risk, texts);
  }
  return labels;
}

// Returns the display name, falling back to "unnamed" if empty.
QString slaveEvidenceDisplayName(const QString &name,
                                 const SlaveEvidenceUiTexts &texts) {
  const QString trimmed = name.trimmed();
  return trimmed.isEmpty() ? texts.unnamed : trimmed;
}

// Returns the display state, falling back to "unknown" if empty.
QString slaveEvidenceDisplayState(const QString &state,
                                  const SlaveEvidenceUiTexts &texts) {
  const QString trimmed = state.trimmed();
  return trimmed.isEmpty() ? texts.unknown : trimmed;
}

// Returns all 12 localized column headers for the slave evidence matrix.
QStringList slaveEvidenceMatrixHeaders(const SlaveEvidenceUiTexts &texts) {
  return {
      texts.priorityHeader, texts.slaveHeader,     texts.nameHeader,
      texts.stateHeader,    texts.readinessHeader, texts.odHeader,
      texts.pdoHeader,      texts.watchHeader,     texts.startupHeader,
      texts.processHeader,  texts.riskHeader,      texts.nextHeader,
  };
}

// Converts a domain row into a UI row with table cells and detail lines for the panel.
SlaveEvidenceUiRow slaveEvidenceUiRow(const SlaveEvidenceRow &row,
                                      const SlaveEvidenceUiTexts &texts) {
  SlaveEvidenceUiRow uiRow;
  const QStringList rowRisks = slaveEvidenceRiskTexts(row.risks, texts);
  const QString priority = slaveEvidencePriorityText(row.priority, texts);
  const QString next = slaveEvidenceNextActionText(row.nextAction, texts);
  const QString readiness = QString("%1% (%2/%3)")
                                .arg(slaveEvidenceReadinessPercent(row))
                                .arg(row.readiness)
                                .arg(row.maxReadiness);

  uiRow.cells = {
      priority,
      QString::number(row.position),
      slaveEvidenceDisplayName(row.name, texts),
      slaveEvidenceDisplayState(row.state, texts),
      readiness,
      row.odRows > 0 ? QString::number(row.odRows) : texts.missing,
      row.pdoRows > 0 ? QString::number(row.pdoRows) : texts.missing,
      row.watchRows > 0
          ? texts.watchValuesPattern.arg(row.watchValueRows).arg(row.watchRows)
          : texts.missing,
      row.startupRows > 0
          ? texts.startupRowsPattern.arg(row.startupRows).arg(row.startupDiffs)
          : texts.noRows,
      row.processRows > 0
          ? texts.processRowsPattern.arg(row.processRows).arg(row.mapIssues)
          : texts.missing,
      rowRisks.isEmpty() ? texts.none : rowRisks.join("; "),
      next,
  };

  QStringList driveFacts;
  if (!row.driveStatusword.isEmpty()) {
    driveFacts << row.driveStatusword;
  }
  if (!row.driveModeDisplay.isEmpty()) {
    driveFacts << texts.modePattern.arg(row.driveModeDisplay);
  }
  if (!row.driveErrorCode.isEmpty()) {
    driveFacts << row.driveErrorCode;
  }

  uiRow.detailLines << texts.slavePattern.arg(row.position)
                           .arg(slaveEvidenceDisplayName(row.name, texts));
  uiRow.detailLines << texts.priorityPattern.arg(priority);
  uiRow.detailLines << texts.statePattern.arg(
      slaveEvidenceDisplayState(row.state, texts));
  uiRow.detailLines << texts.identityRowsPattern.arg(row.identityRows);
  uiRow.detailLines << texts.odRowsPattern.arg(row.odRows);
  uiRow.detailLines << texts.pdoRowsPattern.arg(row.pdoRows);
  uiRow.detailLines << texts.watchValuesDetailPattern.arg(row.watchValueRows)
                           .arg(row.watchRows);
  uiRow.detailLines << texts.startupRowsDetailPattern.arg(row.startupRows)
                           .arg(row.startupDiffs);
  uiRow.detailLines
      << texts.processRowsDetailPattern.arg(row.processRows).arg(row.mapIssues);
  if (!driveFacts.isEmpty()) {
    uiRow.detailLines << texts.drivePattern.arg(driveFacts.join(" | "));
  }
  uiRow.detailLines << texts.nextPattern.arg(next);
  if (!rowRisks.isEmpty()) {
    uiRow.detailLines << texts.riskPattern.arg(rowRisks.join("; "));
  }
  return uiRow;
}
