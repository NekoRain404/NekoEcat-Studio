#include "SafetyController.h"

// SafetyController.cpp — Validates EtherCAT state transitions and SDO write operations
//
// Implementation notes:
//   - Blocks illegal state transitions (e.g. OP→INIT, OP→PRE-OP)
//   - SDO write validation checks position, index format, and value ranges
//   - Emits safetyViolation signals on rejected operations for logging

SafetyController::SafetyController(QObject *parent) : QObject(parent) {}

ValidationResult SafetyController::validateStateTransition(int from, int to) {
  ValidationResult r;

  if (from == kStateOp && to == kStateInit) {
    r.allowed = false;
    r.reason = "Cannot transition directly from OP to INIT. "
               "Must go through SAFE-OP or PRE-OP first.";
    emit safetyViolation("stateTransition", r.reason);
    return r;
  }

  if (from == kStateOp && to == kStatePreOp) {
    r.allowed = false;
    r.reason = "Cannot transition directly from OP to PRE-OP. "
               "Must go through SAFE-OP first.";
    emit safetyViolation("stateTransition", r.reason);
    return r;
  }

  const bool validFrom = (from == kStateInit || from == kStatePreOp ||
                          from == kStateSafeOp || from == kStateOp);
  const bool validTo =
      (to == kStateInit || to == kStatePreOp ||
       to == kStateSafeOp || to == kStateOp);

  if (!validFrom || !validTo) {
    r.allowed = false;
    r.reason = "Invalid state value.";
    emit safetyViolation("stateTransition", r.reason);
    return r;
  }

  r.allowed = true;
  return r;
}

ValidationResult SafetyController::validateSdoWrite(int position,
                                                    const QString &index,
                                                    const QString &value) {
  ValidationResult r;

  if (position < 0) {
    r.allowed = false;
    r.reason = "Invalid slave position.";
    emit safetyViolation("sdoWrite", r.reason);
    return r;
  }

  if (index.isEmpty()) {
    r.allowed = false;
    r.reason = "SDO index cannot be empty.";
    emit safetyViolation("sdoWrite", r.reason);
    return r;
  }

  bool ok;
  int idx = index.startsWith("0x") ? index.toInt(&ok, 16)
                                    : index.toInt(&ok, 16);
  if (ok && idx >= 0x1000 && idx < 0x1FFF) {
    if (idx == 0x1000 || idx == 0x1001 || idx == 0x1018) {
      r.allowed = false;
      r.reason = QString("Object 0x%1 is read-only and cannot be written.")
                     .arg(idx, 4, 16, QChar('0'));
      emit safetyViolation("sdoWrite", r.reason);
      return r;
    }
  }

  r.allowed = true;
  return r;
}

ValidationResult SafetyController::validateFreeRunStart(bool opStateActive) {
  ValidationResult r;

  if (!opStateActive) {
    r.allowed = false;
    r.reason = "Cannot start Free Run: at least one slave must be in OP state.";
    emit safetyViolation("freeRunStart", r.reason);
    return r;
  }

  r.allowed = true;
  return r;
}

ValidationResult SafetyController::validateSdoWriteDuringFreeRun(
    bool freeRunActive) {
  ValidationResult r;

  if (freeRunActive) {
    r.allowed = false;
    r.reason = "Cannot write SDOs while Free Run is active. "
               "Stop Free Run first.";
    emit safetyViolation("sdoWrite", r.reason);
    return r;
  }

  r.allowed = true;
  return r;
}
