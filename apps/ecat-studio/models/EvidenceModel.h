#pragma once

// Evidence assessment models: drive evidence severity and state recommendations.
//
// This file consolidates:
// - EvidenceStatusModel: drive evidence severity classification
// - StateRecommendationModel: EtherCAT state transition recommendations

#include <QString>

// ── Evidence Severity ───────────────────────────────────────────────

enum class DriveEvidenceSeverity {
    Neutral,
    Action,
    Warning,
    Error,
    Ok,
};

bool hasStartupDiffEvidence(const QString& status);
bool hasPdoMapIssueEvidence(const QString& status);
DriveEvidenceSeverity driveEvidenceSeverity(const QString& evidence);
bool hasDriveFaultEvidence(const QString& evidence);

// ── State Recommendation ────────────────────────────────────────────

struct EthercatStateEvidence {
    QString currentState;
    bool pdoLoaded = false;
    int watchValueRows = 0;
    int startupDiffs = 0;
    int freeRunRows = 0;
    int mapIssues = 0;
    bool consistencyOk = false;
};

QString recommendedEthercatState(const EthercatStateEvidence& evidence);
