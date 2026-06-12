#include "DiagnosticsEventUiState.h"

QString diagnosticsEventColorKey(const QString &level) {
  if (level == QStringLiteral("Error")) {
    return QStringLiteral("error");
  }
  if (level == QStringLiteral("Warning")) {
    return QStringLiteral("warning");
  }
  return QStringLiteral("info");
}

QStringList diagnosticsEventHeaders(const DiagnosticsEventTexts &texts) {
  return {texts.timeHeader, texts.levelHeader, texts.sourceHeader,
          texts.messageHeader};
}

DiagnosticsEventSummary diagnosticsEventCounts(const QStringList &levels) {
  DiagnosticsEventSummary summary;
  summary.total = levels.size();
  summary.visible = levels.size();
  for (const QString &level : levels) {
    if (level == QStringLiteral("Error")) {
      ++summary.errors;
    } else if (level == QStringLiteral("Warning")) {
      ++summary.warnings;
    } else if (level == QStringLiteral("Info")) {
      ++summary.infos;
    }
  }
  return summary;
}

DiagnosticsEventSummary
diagnosticsEventSummary(const QList<DiagnosticsEventRowState> &rows,
                        const DiagnosticsEventTexts &texts) {
  QStringList levels;
  levels.reserve(rows.size());
  int visible = 0;
  for (const auto &row : rows) {
    levels.append(row.level);
    if (row.visible) {
      ++visible;
    }
  }
  DiagnosticsEventSummary summary = diagnosticsEventCounts(levels);
  summary.visible = visible;

  if (summary.total <= 0) {
    summary.text = texts.noDiagnostics;
    return summary;
  }

  summary.text = QString("%1/%2 %3   %4: %5   %6: %7   %8: %9")
                     .arg(summary.visible)
                     .arg(summary.total)
                     .arg(texts.shown)
                     .arg(texts.errorLabel)
                     .arg(summary.errors)
                     .arg(texts.warningLabel)
                     .arg(summary.warnings)
                     .arg(texts.infoLabel)
                     .arg(summary.infos);
  return summary;
}
