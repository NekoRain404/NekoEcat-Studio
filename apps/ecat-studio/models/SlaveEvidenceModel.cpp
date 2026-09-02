// Per-slave evidence row model for the commissioning evidence matrix.
#include "SlaveEvidenceModel.h"

#include <algorithm>

namespace {

// PREOP and above states are expected to have PDO/watch evidence available.
bool stateNeedsPdoOrWatch(const QString& state) {
    const QString normalized = state.trimmed().toUpper();
    return normalized.contains("PREOP") || normalized.contains("SAFEOP") || normalized == "OP" ||
           normalized.startsWith("OP ");
}

// SAFEOP and OP states require process image validation evidence.
bool stateNeedsProcessEvidence(const QString& state) {
    const QString normalized = state.trimmed().toUpper();
    return normalized.contains("SAFEOP") || normalized == "OP" || normalized.startsWith("OP ");
}

// Null-safe helper: appends a risk entry with optional count.
void appendRisk(QVector<SlaveEvidenceRisk>* risks, SlaveEvidenceRiskKind kind, int count = 0) {
    if (!risks) {
        return;
    }
    risks->append({kind, count});
}

} // namespace

// Maps priority enum to a numeric sort order (lower = higher priority).
int slaveEvidencePriorityRank(SlaveEvidencePriority priority) {
    switch (priority) {
        case SlaveEvidencePriority::Fault:
            return 0;
        case SlaveEvidencePriority::Risk:
            return 1;
        case SlaveEvidencePriority::Action:
            return 2;
        case SlaveEvidencePriority::Ready:
            return 3;
    }
    return 3;
}

// Clamped 0-100 readiness percentage for progress display.
int slaveEvidenceReadinessPercent(const SlaveEvidenceRow& row) {
    return std::clamp((row.readiness * 100) / std::max(1, row.maxReadiness), 0, 100);
}

// Maps next-action enum to the navigation target view in the UI.
SlaveEvidenceRouteTarget slaveEvidenceRouteTarget(const SlaveEvidenceRow& row) {
    switch (row.nextAction) {
        case SlaveEvidenceNextAction::ReviewOd:
            return SlaveEvidenceRouteTarget::ObjectDictionary;
        case SlaveEvidenceNextAction::LoadPdo:
            return SlaveEvidenceRouteTarget::PdoMap;
        case SlaveEvidenceNextAction::AddWatch:
            return SlaveEvidenceRouteTarget::Watch;
        case SlaveEvidenceNextAction::ReviewStartup:
            return SlaveEvidenceRouteTarget::Startup;
        case SlaveEvidenceNextAction::ValidateProcess:
            return SlaveEvidenceRouteTarget::Process;
        case SlaveEvidenceNextAction::ReviewRisk:
            break;
        case SlaveEvidenceNextAction::Ready:
            return SlaveEvidenceRouteTarget::Overview;
    }

    bool hasRisk = false;
    // Iterate over collection
    for (const auto& risk : row.risks) {
        hasRisk = true;
        if (risk.kind == SlaveEvidenceRiskKind::PdoMapIssue || risk.kind == SlaveEvidenceRiskKind::ProcessMissing) {
            return SlaveEvidenceRouteTarget::Process;
        }
        if (risk.kind == SlaveEvidenceRiskKind::TopologyBaselineIssue) {
            return SlaveEvidenceRouteTarget::StateMachine;
        }
    }
    if (hasRisk) {
        return SlaveEvidenceRouteTarget::StateMachine;
    }

    return SlaveEvidenceRouteTarget::Overview;
}

// Full evidence matrix: scores readiness, collects risks, determines next actions, and sorts by priority.
SlaveEvidenceMatrix buildSlaveEvidenceMatrix(const QVector<SlaveEvidenceInput>& inputs) {
    SlaveEvidenceMatrix matrix;
    matrix.rows.reserve(inputs.size());

    // Iterate over collection
    for (const auto& input : inputs) {
        SlaveEvidenceRow row;
        row.position = input.position;
        row.name = input.name.trimmed();
        row.state = input.state.trimmed();
        row.identityRows = input.identityRows;
        row.odRows = input.odRows;
        row.pdoRows = input.pdoRows;
        row.watchRows = input.watchRows;
        row.watchValueRows = input.watchValueRows;
        row.startupRows = input.startupRows;
        row.startupDiffs = input.startupDiffs;
        row.processRows = input.processRows;
        row.mapIssues = input.mapIssues;
        row.driveStatusword = input.driveStatusword.trimmed();
        row.driveModeDisplay = input.driveModeDisplay.trimmed();
        row.driveErrorCode = input.driveErrorCode.trimmed();
        row.driveEvidence = input.driveEvidence.trimmed();

        row.readiness += 1;
        row.readiness += row.identityRows > 0 ? 1 : 0;
        row.readiness += row.odRows > 0 ? 1 : 0;
        row.readiness += row.pdoRows > 0 ? 1 : 0;
        row.readiness += row.watchValueRows > 0 ? 1 : 0;
        row.readiness += row.processRows > 0 ? 1 : 0;

        if (row.identityRows <= 0) {
            appendRisk(&row.risks, SlaveEvidenceRiskKind::IdentityMissing);
            ++matrix.evidenceGaps;
        }
        if (row.odRows <= 0) {
            appendRisk(&row.risks, SlaveEvidenceRiskKind::OdMissing);
            ++matrix.evidenceGaps;
        }
        if (stateNeedsPdoOrWatch(row.state) && row.pdoRows <= 0) {
            appendRisk(&row.risks, SlaveEvidenceRiskKind::PdoMissing);
            ++matrix.evidenceGaps;
        }
        if (stateNeedsPdoOrWatch(row.state) && row.watchValueRows <= 0) {
            appendRisk(&row.risks, SlaveEvidenceRiskKind::WatchMissing);
            ++matrix.evidenceGaps;
        }
        if (stateNeedsProcessEvidence(row.state) && row.processRows <= 0) {
            appendRisk(&row.risks, SlaveEvidenceRiskKind::ProcessMissing);
            ++matrix.evidenceGaps;
        }
        if (row.startupDiffs > 0) {
            appendRisk(&row.risks, SlaveEvidenceRiskKind::StartupDiff, row.startupDiffs);
        }
        if (row.mapIssues > 0) {
            appendRisk(&row.risks, SlaveEvidenceRiskKind::PdoMapIssue, row.mapIssues);
        }
        if (input.topologyIssue) {
            appendRisk(&row.risks, SlaveEvidenceRiskKind::TopologyBaselineIssue);
        }
        if (input.driveFault) {
            appendRisk(&row.risks, SlaveEvidenceRiskKind::DriveFault);
        }

        if (row.odRows <= 0) {
            row.nextAction = SlaveEvidenceNextAction::ReviewOd;
        } else if (row.pdoRows <= 0) {
            row.nextAction = SlaveEvidenceNextAction::LoadPdo;
        } else if (row.watchValueRows <= 0) {
            row.nextAction = SlaveEvidenceNextAction::AddWatch;
        } else if (row.startupDiffs > 0) {
            row.nextAction = SlaveEvidenceNextAction::ReviewStartup;
        } else if (row.processRows <= 0 && stateNeedsProcessEvidence(row.state)) {
            row.nextAction = SlaveEvidenceNextAction::ValidateProcess;
        } else if (!row.risks.isEmpty()) {
            row.nextAction = SlaveEvidenceNextAction::ReviewRisk;
        } else {
            row.nextAction = SlaveEvidenceNextAction::Ready;
        }

        if (input.driveFault || row.mapIssues > 0 || row.startupDiffs > 0) {
            row.priority = SlaveEvidencePriority::Fault;
        } else if (!row.risks.isEmpty()) {
            row.priority = SlaveEvidencePriority::Risk;
        } else if (row.readiness < row.maxReadiness) {
            row.priority = SlaveEvidencePriority::Action;
        } else {
            row.priority = SlaveEvidencePriority::Ready;
        }

        if (!row.risks.isEmpty()) {
            ++matrix.riskRows;
        } else if (row.readiness >= row.maxReadiness) {
            ++matrix.readyRows;
        } else {
            ++matrix.actionRows;
        }
        matrix.rows.append(row);
    }

    std::stable_sort(matrix.rows.begin(), matrix.rows.end(),
                     [](const SlaveEvidenceRow& left, const SlaveEvidenceRow& right) {
                         const int leftPriority = slaveEvidencePriorityRank(left.priority);
                         const int rightPriority = slaveEvidencePriorityRank(right.priority);
                         if (leftPriority != rightPriority) {
                             return leftPriority < rightPriority;
                         }
                         if (left.risks.size() != right.risks.size()) {
                             return left.risks.size() > right.risks.size();
                         }
                         if (left.readiness != right.readiness) {
                             return left.readiness < right.readiness;
                         }
                         return left.position < right.position;
                     });

    return matrix;
}
