#include "StateMachineService.h"
#include "infra/EcatClient.h"

// StateMachineService.cpp — Per-slave EtherCAT state machine with transition validation
//
// Implementation notes:
//   - Validates transitions against the EtherCAT state diagram
//   - Sends validated state change requests to the ecatd daemon via EcatClient
//   - Updates local state optimistically on successful send
//   - Transition history capped at kMaxHistory per position

StateMachineService::StateMachineService(EcatClient* client, QObject* parent) : QObject(parent), client_(client) {}

bool StateMachineService::requestState(int position, int state) {
    if (!isValidState(state)) {
        emit stateTransitionFailed(position, currentStates_.value(position, 0), state, "Invalid target state");
        return false;
    }

    int from = currentStates_.value(position, 0);
    if (!validateTransition(from, state)) {
        QString reason = QString("Transition from state %1 to %2 is not allowed").arg(from).arg(state);
        emit stateTransitionFailed(position, from, state, reason);
        return false;
    }

    // Validation passed: send the state change to the daemon.
    client_->setState(position, stateToString(state));

    // Record the transition in history as successful.
    {
        StateTransition tr;
        tr.position = position;
        tr.fromState = from;
        tr.toState = state;
        tr.timestamp = QDateTime::currentDateTime();
        tr.success = true;
        auto& h = history_[position];
        h.append(tr);
        if (h.size() > kMaxHistory)
            h.removeFirst();
    }

    // Optimistically update local state tracking.
    currentStates_[position] = state;

    emit stateChangeRequested(position, state);
    emit stateChanged(position, state);
    return true;
}

int StateMachineService::currentState(int position) const {
    return currentStates_.value(position, 0);
}

bool StateMachineService::validateTransition(int from, int to) const {
    if (!isValidState(to))
        return false;
    if (from == 0 && to == 1)
        return true;
    if (!isValidState(from))
        return false;
    if (from == to)
        return true;
    switch (from) {
        case 1: // INIT
            return to == 2;
        case 2: // PRE-OP
            return to == 4 || to == 1;
        case 4: // SAFE-OP
            return to == 8 || to == 2 || to == 1;
        case 8: // OP
            return to == 4;
        default:
            return false;
    }
}

QVector<StateTransition> StateMachineService::stateHistory(int position) const {
    return history_.value(position);
}

bool StateMachineService::recoverState(int position) {
    int current = currentStates_.value(position, 0);
    if (!isValidState(current)) {
        emit stateTransitionFailed(position, current, current, "No known state to recover from");
        return false;
    }

    int target = recoveryTargetState(current);
    if (target == current) {
        // Already at the lowest state; nothing to recover.
        emit stateChanged(position, current);
        return true;
    }

    return requestState(position, target);
}

bool StateMachineService::isValidState(int state) {
    return state == 1 || state == 2 || state == 4 || state == 8;
}

QString StateMachineService::stateToString(int state) {
    switch (state) {
        case 1:
            return QStringLiteral("INIT");
        case 2:
            return QStringLiteral("PREOP");
        case 4:
            return QStringLiteral("SAFEOP");
        case 8:
            return QStringLiteral("OP");
        default:
            return QStringLiteral("INIT");
    }
}

int StateMachineService::recoveryTargetState(int current) {
    switch (current) {
        case 8:
            return 4;
        case 4:
            return 2;
        case 2:
            return 1;
        default:
            return current;
    }
}
