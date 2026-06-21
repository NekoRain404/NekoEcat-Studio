#pragma once

// SafetyController — implements safety boundaries for EtherCAT operations.
// Validates state transitions, SDO write permissions, and Free Run safety.
// Prevents dangerous operations without confirmation.
//
// This service provides safety validation for critical EtherCAT operations.
// It handles:
//   - State transition validation (INIT → PREOP → SAFEOP → OP)
//   - SDO write permission checks
//   - Free Run start condition validation
//   - SDO write during Free Run safety checks
//   - Safety violation reporting
//
// Usage:
//   ServiceContainer *container = ...;
//   SafetyController *safety = container->safety();
//   auto result = safety->validateStateTransition(1, 8);  // INIT → OP
//   if (!result.allowed) {
//     // Show confirmation dialog with result.reason
//   }
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The controller
//   is stateless and does not perform any I/O operations.
//
// Performance:
//   - Validation operations are O(1)
//   - No I/O or network operations
//   - No caching or state management

#include <QObject>
#include <QString>

// Result of a safety validation check.
struct ValidationResult {
  bool allowed = false;   // Whether the operation is allowed
  QString reason;         // Human-readable reason (empty if allowed)
};

class SafetyController : public QObject {
  Q_OBJECT
public:
  explicit SafetyController(QObject *parent = nullptr);

  // Validate a slave state transition.
  // @param from  Current state (1=INIT, 2=PREOP, 4=SAFEOP, 8=OP)
  // @param to    Target state (1=INIT, 2=PREOP, 4=SAFEOP, 8=OP)
  // @return ValidationResult with allowed flag and reason
  ValidationResult validateStateTransition(int from, int to);

  // Validate an SDO write operation.
  // @param position  Slave position on the bus
  // @param index     SDO index in hex format
  // @param value     Value to write
  // @return ValidationResult with allowed flag and reason
  ValidationResult validateSdoWrite(int position, const QString &index,
                                    const QString &value);

  // Validate Free Run start conditions.
  // @param opStateActive  Whether OP state is active
  // @return ValidationResult with allowed flag and reason
  ValidationResult validateFreeRunStart(bool opStateActive);

  // Validate SDO write during Free Run.
  // @param freeRunActive  Whether Free Run is currently active
  // @return ValidationResult with allowed flag and reason
  ValidationResult validateSdoWriteDuringFreeRun(bool freeRunActive);

signals:
  // Emitted when a safety violation is detected.
  // @param operation  The operation that was attempted
  // @param reason     Human-readable reason for the violation
  void safetyViolation(const QString &operation, const QString &reason);

private:
  // EtherCAT state constants
  static constexpr int kStateInit = 1;     // INIT state
  static constexpr int kStatePreOp = 2;    // PREOP state
  static constexpr int kStateSafeOp = 4;   // SAFEOP state
  static constexpr int kStateOp = 8;       // OP state
};
