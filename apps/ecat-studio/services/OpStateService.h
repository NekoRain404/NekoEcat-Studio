#pragma once

// OpStateService — OP state transition management for EtherCAT slaves.
//
// This service manages the critical process of transitioning EtherCAT slaves
// into Operational (OP) state. It handles:
//   - OP state transition requests with pre-condition checks
//   - Current state monitoring per slave
//   - Error handling and diagnostics during state transitions
//   - Automatic recovery from error states
//   - Transition history tracking
//
// The EtherCAT state machine requires:
//   INIT (1) → PREOP (2) → SAFEOP (4) → OP (8)
//
// Usage:
//   ServiceContainer *container = ...;
//   OpStateService *opState = container->opState();
//   opState->requestOpState(0);
//   OpStateStatus status = opState->checkOpState(0);
//   if (!status.error.isEmpty()) {
//     opState->recoverFromError(0);
//   }
//
// Thread safety:
//   All methods must be called from the main (GUI) thread.

#include <QDateTime>
#include <QHash>
#include <QObject>
#include <QString>
#include <QVector>

struct OpStateErrorInfo {
    int errorCode = 0;
    QString message;
    QString source;
    QDateTime timestamp;
    bool recoverable = false;
};

struct OpStateStatus {
    int position = -1;
    int currentState = 0;
    int targetState = 8;
    QString error;
    QDateTime timestamp;
    QString details;
};

struct OpStateTransition {
    int position = -1;
    int fromState = 0;
    int toState = 0;
    QDateTime timestamp;
    bool success = false;
    QString error;
};

class OpStateService : public QObject {
    Q_OBJECT
public:
    explicit OpStateService(QObject* parent = nullptr);

    bool requestOpState(int position);
    OpStateStatus checkOpState(int position) const;
    bool handleOpStateError(int position, const OpStateErrorInfo& error);
    bool recoverFromError(int position);

    int currentState(int position) const;
    QVector<OpStateErrorInfo> errorHistory(int position) const;
    QVector<OpStateTransition> transitionHistory(int position) const;

signals:
    void opStateChanged(int position, bool success);
    void opStateError(int position, const OpStateErrorInfo& error);

private:
    bool isValidTransition(int from, int to) const;
    static constexpr int kOpState = 8;
    static constexpr int kMaxHistory = 500;

    QHash<int, int> currentStates_;
    QHash<int, QVector<OpStateErrorInfo>> errors_;
    QHash<int, QVector<OpStateTransition>> transitions_;
};
