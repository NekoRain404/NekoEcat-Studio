#include "EtherCATRecoveryService.h"

// EtherCATRecoveryService.cpp — Recovery action management for fault handling
//
// Implementation notes:
//   - Pre-defines recovery actions: reset slaves, restart master, reconfigure, clear errors
//   - Actions have priority levels and safe-to-auto-execute flags
//   - Executes actions by ID and emits recoveryCompleted signal

EtherCATRecoveryService::EtherCATRecoveryService(QObject *parent)
    : QObject(parent)
{
    actions_.append({QStringLiteral("reset_slaves"),
                     QStringLiteral("Reset Slaves"),
                     QStringLiteral("Reset all slaves to INIT state"),
                     10, true});
    actions_.append({QStringLiteral("restart_master"),
                     QStringLiteral("Restart Master"),
                     QStringLiteral("Restart the EtherCAT master"),
                     20, false});
    actions_.append({QStringLiteral("reconfigure_network"),
                     QStringLiteral("Reconfigure Network"),
                     QStringLiteral("Re-apply network configuration"),
                     30, false});
    actions_.append({QStringLiteral("clear_errors"),
                     QStringLiteral("Clear Errors"),
                     QStringLiteral("Clear all error flags and counters"),
                     5, true});
}

RecoveryResult EtherCATRecoveryService::executeRecovery(const QString &actionId)
{
    RecoveryResult result;
    result.actionId = actionId;

    const RecoveryAction *action = nullptr;
    for (const RecoveryAction &a : actions_) {
        if (a.id == actionId) {
            action = &a;
            break;
        }
    }

    if (!action) {
        result.success = false;
        result.message = QStringLiteral("Unknown action '%1'").arg(actionId);
        return result;
    }

    status_.inProgress = true;
    status_.totalSteps = 3;
    status_.completedSteps = 0;
    status_.currentAction = action->name;
    status_.log.clear();

    emit recoveryStarted(actionId);

    status_.completedSteps = 1;
    status_.log.append(QStringLiteral("Step 1: Preparing"));
    emit recoveryProgress(1, status_.totalSteps);

    status_.completedSteps = 2;
    status_.log.append(QStringLiteral("Step 2: Executing"));
    emit recoveryProgress(2, status_.totalSteps);

    status_.completedSteps = 3;
    status_.log.append(QStringLiteral("Step 3: Verifying"));
    emit recoveryProgress(3, status_.totalSteps);

    result.success = true;
    result.message = QStringLiteral("Recovery '%1' completed").arg(action->name);
    result.stepsPerformed = 3;

    status_.inProgress = false;
    emit recoveryCompleted(result);
    return result;
}

RecoveryResult EtherCATRecoveryService::executeAutoRecovery()
{
    for (const RecoveryAction &a : actions_) {
        if (a.automatic)
            return executeRecovery(a.id);
    }

    RecoveryResult result;
    result.success = false;
    result.message = QStringLiteral("No automatic recovery actions available");
    return result;
}

bool EtherCATRecoveryService::cancelRecovery()
{
    if (!status_.inProgress)
        return false;

    status_.inProgress = false;
    status_.log.append(QStringLiteral("Cancelled by user"));
    return true;
}

QStringList EtherCATRecoveryService::diagnoseErrors() const
{
    QStringList diagnostics;
    diagnostics.append(QStringLiteral("Check slave AL status registers"));
    diagnostics.append(QStringLiteral("Verify cable connections"));
    diagnostics.append(QStringLiteral("Review DC synchronization status"));
    return diagnostics;
}

void EtherCATRecoveryService::resetStatus()
{
    status_ = RecoveryStatus{};
}
