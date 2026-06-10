#include "SelectedDriveSummaryUiState.h"

#include "EvidenceStatusModel.h"

namespace {

QString normalizedIndex(const WatchStartupWatchRow &row) {
  return row.index.trimmed().toLower();
}

bool isZeroErrorValue(const QString &value) {
  const QString trimmed = value.trimmed();
  return trimmed == "0" || trimmed.toLower() == "0x0000";
}

QString driveSeverityKey(DriveEvidenceSeverity severity) {
  switch (severity) {
  case DriveEvidenceSeverity::Error:
    return QStringLiteral("error");
  case DriveEvidenceSeverity::Warning:
    return QStringLiteral("warning");
  case DriveEvidenceSeverity::Ok:
    return QStringLiteral("ok");
  case DriveEvidenceSeverity::Action:
    return QStringLiteral("action");
  case DriveEvidenceSeverity::Neutral:
    return QStringLiteral("neutral");
  }
  return QStringLiteral("neutral");
}

QStringList summaryEvidenceParts(const SelectedDriveSummaryEvidence &evidence) {
  QStringList parts;
  if (!evidence.status.isEmpty()) {
    parts << evidence.status;
  }
  if (!evidence.mode.isEmpty()) {
    parts << evidence.mode;
  }
  if (!evidence.error.isEmpty()) {
    parts << evidence.error;
  }
  if (!evidence.controlword.isEmpty()) {
    parts << evidence.controlword;
  }
  return parts;
}

QStringList displayParts(const SelectedDriveSummaryEvidence &evidence,
                         const SelectedDriveSummaryTexts &texts) {
  QStringList parts;
  if (!evidence.status.isEmpty()) {
    parts << evidence.status;
  }
  if (!evidence.mode.isEmpty()) {
    parts << texts.modePattern.arg(evidence.mode);
  }
  if (!evidence.error.isEmpty()) {
    parts << evidence.error;
  }
  if (!evidence.controlword.isEmpty()) {
    parts << texts.controlwordPattern.arg(evidence.controlword);
  }
  return parts;
}

} // namespace

SelectedDriveSummaryUiState
selectedDriveNoWatchEvidenceState(const SelectedDriveSummaryTexts &texts) {
  return {.text = texts.noWatchEvidence,
          .severityKey = QStringLiteral("neutral")};
}

SelectedDriveSummaryEvidence
selectedDriveSummaryEvidence(const QVector<WatchStartupWatchRow> &watchRows,
                             int position) {
  SelectedDriveSummaryEvidence evidence;
  if (position < 0) {
    return evidence;
  }

  for (const auto &row : watchRows) {
    if (row.position != position) {
      continue;
    }

    const QString index = normalizedIndex(row);
    const QString value = row.value.trimmed();
    const QString decoded = row.decoded.trimmed();
    if (index == "0x6041" && !decoded.isEmpty()) {
      evidence.status = decoded;
    } else if (index == "0x6061" && !decoded.isEmpty()) {
      evidence.mode = decoded;
    } else if (index == "0x603f" && !decoded.isEmpty() &&
               !isZeroErrorValue(value)) {
      evidence.error = decoded;
    } else if (index == "0x6040" && !decoded.isEmpty()) {
      evidence.controlword = decoded;
    }
  }
  return evidence;
}

QString
selectedDriveSummarySeverityKey(const SelectedDriveSummaryEvidence &evidence) {
  return driveSeverityKey(
      driveEvidenceSeverity(summaryEvidenceParts(evidence).join(" | ")));
}

SelectedDriveSummaryUiState
buildSelectedDriveSummaryUiState(const QVector<WatchStartupWatchRow> &watchRows,
                                 int position,
                                 const SelectedDriveSummaryTexts &texts) {
  if (position < 0) {
    return selectedDriveNoWatchEvidenceState(texts);
  }

  SelectedDriveSummaryUiState state;
  state.evidence = selectedDriveSummaryEvidence(watchRows, position);
  state.parts = displayParts(state.evidence, texts);
  state.severityKey = selectedDriveSummarySeverityKey(state.evidence);
  state.text = state.parts.isEmpty()
                   ? texts.noCia402Evidence
                   : texts.summaryPattern.arg(state.parts.join(" | "));
  return state;
}

Cia402ControlwordRecommendation selectedDriveControlwordRecommendation(
    const QVector<WatchStartupWatchRow> &watchRows, int position) {
  if (position < 0) {
    return {};
  }

  QString decodedStatus;
  for (const auto &row : watchRows) {
    if (row.position != position || normalizedIndex(row) != "0x6041") {
      continue;
    }
    decodedStatus = row.decoded.trimmed();
    break;
  }
  if (decodedStatus.isEmpty()) {
    return {};
  }
  return recommendedCia402ControlwordFromStatus(decodedStatus);
}
