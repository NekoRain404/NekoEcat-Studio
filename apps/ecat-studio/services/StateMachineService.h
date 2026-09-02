#pragma once

// StateMachineService — manages EtherCAT device state machine transitions
// with validation, history tracking, and automatic recovery capabilities.
//
// This service provides EtherCAT device state machine management. It handles:
//   - State transition requests with validation
//   - Current state tracking per slave
//   - State transition history
//   - Automatic state recovery
//   - Transition validation (INIT → PREOP → SAFEOP → OP)
//
// Usage:
//   StateMachineService stateMachine(client);
//   stateMachine.requestState(0, 8);  // Request OP state for slave 0
//   int currentState = stateMachine.currentState(0);
//   bool valid = stateMachine.validateTransition(1, 8);  // INIT → OP
//   QVector<StateTransition> history = stateMachine.stateHistory(0);
//   stateMachine.recoverState(0);  // Recover from error state
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. State
//   transitions are synchronous and block the calling thread.
//
// Performance:
//   - State transition is O(1)
//   - State query is O(1)
//   - Transition validation is O(1)
//   - History retrieval is O(n) where n is history size

#include <QDateTime>
#include <QHash>
#include <QObject>
#include <QVector>

class EcatClient;

// Represents a state transition event.
struct StateTransition {
    int position = -1;    // Slave position
    int fromState = 0;    // Source state
    int toState = 0;      // Target state
    QDateTime timestamp;  // When transition occurred
    bool success = false; // Whether transition succeeded
    QString reason;       // Reason for failure (if failed)
};

class StateMachineService : public QObject {
    Q_OBJECT
public:
    explicit StateMachineService(EcatClient* client, QObject* parent = nullptr);

    // Request a state transition for a slave.
    // @param position  Slave position
    // @param state     Target state (1=INIT, 2=PREOP, 4=SAFEOP, 8=OP)
    // @return true if transition was initiated successfully
    bool requestState(int position, int state);

    // Get the current state of a slave.
    // @param position  Slave position
    // @return Current state (1=INIT, 2=PREOP, 4=SAFEOP, 8=OP)
    int currentState(int position) const;

    // Validate a state transition.
    // @param from  Source state
    // @param to    Target state
    // @return true if transition is valid
    bool validateTransition(int from, int to) const;

    // Get state transition history for a slave.
    // @param position  Slave position
    // @return Vector of StateTransition structures
    QVector<StateTransition> stateHistory(int position) const;

    // Recover a slave to a safe state.
    // @param position  Slave position
    // @return true if recovery was initiated successfully
    bool recoverState(int position);

signals:
    // Emitted when a slave state changes.
    // @param position  Slave position
    // @param state     New state
    void stateChanged(int position, int state);

    // Emitted when a state transition is requested via the daemon client.
    // @param position  Slave position
    // @param state     Requested target state
    void stateChangeRequested(int position, int state);

    // Emitted when a state transition fails.
    // @param position  Slave position
    // @param from      Source state
    // @param to        Target state
    // @param reason    Failure reason
    void stateTransitionFailed(int position, int from, int to, const QString& reason);

private:
    // Check if a state value is valid.
    static bool isValidState(int state);

    // Convert internal integer state to daemon state string.
    static QString stateToString(int state);

    // Get the recovery target state for a given state.
    static int recoveryTargetState(int currentState);

    EcatClient* client_ = nullptr;
    QHash<int, int> currentStates_;                // Current state per slave
    QHash<int, QVector<StateTransition>> history_; // Transition history per slave
    static constexpr int kMaxHistory = 500;        // Maximum history entries
};
