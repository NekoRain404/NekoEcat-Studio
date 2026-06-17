// CiA 402 drive summary card for the selected slave panel.
#include "SelectedDriveSummaryUiState.h"

#include "models/EvidenceStatusModel.h"

namespace {

// Lowercases and trims the index for case-insensitive CiA 402 object matching.
QString normalizedIndex(const WatchStartupWatchRow &row) {
  return row.index.trimmed().toLower();
}

// Whether the raw value represents a zero/no-error state.
bool isZeroErrorValue(const QString &value) {
  const QString trimmed = value.trimmed();
  return trimmed == "0" || trimmed.toLower() == "0x0000";
}

// Maps the drive evidence severity enum to a string key for styling.
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

// Joins non-empty evidence fields into a pipe-separated list for severity evaluation.
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

// Builds the display list with localized labels for mode and controlword.
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

// Neutral state when no CiA 402 watch evidence is available.
SelectedDriveSummaryUiState
selectedDriveNoWatchEvidenceState(const SelectedDriveSummaryTexts &texts) {
  return {.text = texts.noWatchEvidence,
    // Set severityKey field
          .severityKey = QStringLiteral("neutral")};
}

// Extracts CiA 402 statusword, mode display, error code, and controlword from watch rows.
SelectedDriveSummaryEvidence
selectedDriveSummaryEvidence(const QVector<WatchStartupWatchRow> &watchRows,
                             int position) {
  SelectedDriveSummaryEvidence evidence;
  if (position < 0) {
    return evidence;
  }

    // Iterate over collection
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

// Derives severity from the combined drive evidence text.
QString
selectedDriveSummarySeverityKey(const SelectedDriveSummaryEvidence &evidence) {
  return driveSeverityKey(
      driveEvidenceSeverity(summaryEvidenceParts(evidence).join(" | ")));
}

// Assembles the full drive summary card with evidence, severity, and formatted text.
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

// Recommends the next CiA 402 controlword based on the current decoded statusword.
Cia402ControlwordRecommendation selectedDriveControlwordRecommendation(
    const QVector<WatchStartupWatchRow> &watchRows, int position) {
  if (position < 0) {
    return {};
  }

  QString decodedStatus;
    // Iterate over collection
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
