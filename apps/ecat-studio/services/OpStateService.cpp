#include "OpStateService.h"

OpStateService::OpStateService(QObject *parent)
    : QObject(parent) {}

bool OpStateService::requestOpState(int position) {
  int current = currentStates_.value(position, 0);
  int target = kOpState;

  if (current == target) {
    return true;
  }

  static const int kStateChain[] = {1, 2, 4, 8};
  int startIdx = 0;
  for (int i = 0; i < 4; ++i) {
    if (kStateChain[i] == current) {
      startIdx = i + 1;
      break;
    }
  }

  for (int i = startIdx; i < 4; ++i) {
    int from = currentStates_.value(position, 0);
    int to = kStateChain[i];

    currentStates_[position] = to;

    OpStateTransition tr;
    tr.position = position;
    tr.fromState = from;
    tr.toState = to;
    tr.timestamp = QDateTime::currentDateTime();
    tr.success = true;
    auto &h = transitions_[position];
    h.append(tr);
    if (h.size() > kMaxHistory) h.removeFirst();
  }

  emit opStateChanged(position, true);
  return true;
}

OpStateStatus OpStateService::checkOpState(int position) const {
  OpStateStatus status;
  status.position = position;
  status.currentState = currentStates_.value(position, 0);
  status.targetState = kOpState;
  status.timestamp = QDateTime::currentDateTime();

  if (status.currentState == kOpState) {
    status.details = QStringLiteral("Slave is in OP state");
  } else {
    status.details = QString("Slave in state %1, target is OP (8)")
                         .arg(status.currentState);
  }

  auto errIt = errors_.constFind(position);
  if (errIt != errors_.constEnd() && !errIt->isEmpty()) {
    status.error = errIt->last().message;
  }

  return status;
}

bool OpStateService::handleOpStateError(int position, const OpStateErrorInfo &error) {
  auto &errList = errors_[position];
  errList.append(error);
  if (errList.size() > kMaxHistory) errList.removeFirst();

  emit opStateError(position, error);
  return error.recoverable;
}

bool OpStateService::recoverFromError(int position) {
  int current = currentStates_.value(position, 0);

  if (current == 0 || current == 1) {
    return false;
  }

  int recoveryTarget = 1;
  if (current == 8) recoveryTarget = 4;
  else if (current == 4) recoveryTarget = 2;
  else if (current == 2) recoveryTarget = 1;

  currentStates_[position] = recoveryTarget;

  OpStateTransition tr;
  tr.position = position;
  tr.fromState = current;
  tr.toState = recoveryTarget;
  tr.timestamp = QDateTime::currentDateTime();
  tr.success = true;
  auto &h = transitions_[position];
  h.append(tr);
  if (h.size() > kMaxHistory) h.removeFirst();

  emit opStateChanged(position, false);
  return true;
}

int OpStateService::currentState(int position) const {
  return currentStates_.value(position, 0);
}

QVector<OpStateErrorInfo> OpStateService::errorHistory(int position) const {
  return errors_.value(position);
}

QVector<OpStateTransition> OpStateService::transitionHistory(int position) const {
  return transitions_.value(position);
}

bool OpStateService::isValidTransition(int from, int to) const {
  if (from == 0 && to == 1) return true;
  if (from == 1 && to == 2) return true;
  if (from == 2 && to == 4) return true;
  if (from == 4 && to == 8) return true;
  if (from == to) return true;
  return false;
}
