// Drive evidence severity classification and evidence status helpers.
#include "EvidenceStatusModel.h"

// Detects mismatch/diff indicators in startup evidence text (English and Chinese).
bool hasStartupDiffEvidence(const QString &status) {
  const QString normalized = status.trimmed().toLower();
  return normalized.contains("diff") || normalized == "不一致" ||
         normalized.contains("mismatch") || normalized.contains("不匹配") ||
         normalized.contains("偏差");
}

// Detects PDO mapping problems from status text across supported languages.
bool hasPdoMapIssueEvidence(const QString &status) {
  const QString normalized = status.toLower();
  return normalized.contains("warning") || normalized.contains("missing") ||
         normalized.contains("no pdo map") || normalized.contains("警告") ||
         normalized.contains("缺失") || normalized.contains("无 pdo 映射");
}

// Classifies drive evidence text into a severity level for the evidence matrix.
DriveEvidenceSeverity driveEvidenceSeverity(const QString &evidence) {
  const QString normalized = evidence.toLower();
  if (normalized.trimmed().isEmpty()) {
    return DriveEvidenceSeverity::Neutral;
  }
  if (normalized.contains("fault") || normalized.contains("error code") ||
      evidence.contains("错误")) {
    return DriveEvidenceSeverity::Error;
  }
  if (normalized.contains("quick stop") || normalized.contains("warning") ||
      normalized.contains("internal limit")) {
    return DriveEvidenceSeverity::Warning;
  }
  if (normalized.contains("operation enabled")) {
    return DriveEvidenceSeverity::Ok;
  }
  return DriveEvidenceSeverity::Action;
}

// Convenience wrapper: true when evidence indicates a drive fault condition.
bool hasDriveFaultEvidence(const QString &evidence) {
  return driveEvidenceSeverity(evidence) == DriveEvidenceSeverity::Error;
}
