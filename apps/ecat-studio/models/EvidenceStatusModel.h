#pragma once

// Drive evidence severity classification and evidence status helpers.


#include <QString>

enum class DriveEvidenceSeverity {
  Neutral,
  Action,
  Warning,
  Error,
  Ok,
};

bool hasStartupDiffEvidence(const QString &status);
bool hasPdoMapIssueEvidence(const QString &status);
DriveEvidenceSeverity driveEvidenceSeverity(const QString &evidence);
bool hasDriveFaultEvidence(const QString &evidence);
