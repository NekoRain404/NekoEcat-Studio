#pragma once

// EtherCATRecoveryService — recovery action catalogue and offline facade.
//
// This service currently provides recovery planning helpers:
//   - Error diagnosis with actionable suggestions
//   - Manual recovery action catalogue
//   - Automatic recovery selection, rejected until a live backend is available
//   - Recovery status readback/reset helpers
//
// Usage:
//   ServiceContainer *container = ...;
//   EtherCATRecoveryService *recovery = container->recovery();
//   QStringList errors = recovery->diagnoseErrors();
//   QVector<RecoveryAction> actions = recovery->availableActions();
//   // These return failure until connected to a real backend:
//   RecoveryResult result = recovery->executeRecovery("reset_slaves");
//   RecoveryResult autoResult = recovery->executeAutoRecovery();
//
// Thread safety:
//   All methods must be called from the main (GUI) thread.
//
// Performance:
//   - Error diagnosis is O(n) where n is number of monitored components
//   - Unsupported recovery execution is rejected in O(n) over actions
//   - Status queries are O(1)

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

// Defines a recovery action that can be executed.
struct RecoveryAction {
  QString id;               // Unique action identifier (e.g., "reset-master")
  QString name;             // Human-readable action name
  QString description;      // Detailed description of what the action does
  int priority = 0;         // Execution priority (higher = preferred)
  bool automatic = false;   // Whether action can be auto-selected
};

// Result of a recovery action execution.
struct RecoveryResult {
  bool success = false;      // Whether recovery succeeded
  QString message;           // Human-readable result message
  QString actionId;          // ID of the action that was executed
  int stepsPerformed = 0;    // Number of recovery steps completed
};

// Current recovery operation status.
struct RecoveryStatus {
  bool inProgress = false;    // Whether a recovery is currently running
  int totalSteps = 0;         // Total steps in current recovery
  int completedSteps = 0;     // Steps completed so far
  QString currentAction;      // ID of action currently being executed
  QStringList log;            // Recovery operation log entries
};

class EtherCATRecoveryService : public QObject {
  Q_OBJECT
public:
  explicit EtherCATRecoveryService(QObject *parent = nullptr);

  // Get all available recovery actions.
  // @return Vector of RecoveryAction definitions
  QVector<RecoveryAction> availableActions() const { return actions_; }

  // Get the current recovery operation status.
  // @return Current RecoveryStatus
  RecoveryStatus status() const { return status_; }

  // Execute a specific recovery action by ID.
  // @param actionId  ID of the recovery action to execute
  // @return Result of the recovery attempt; currently failed without backend
  RecoveryResult executeRecovery(const QString &actionId);

  // Automatically select and execute the best recovery action.
  // Selects the highest-priority automatic action.
  // @return Result of the recovery attempt; currently failed without backend
  RecoveryResult executeAutoRecovery();

  // Cancel the currently running recovery operation.
  // @return true if cancellation was successful
  bool cancelRecovery();

  // Diagnose current system errors and return suggestions.
  // @return List of error descriptions with recovery suggestions
  QStringList diagnoseErrors() const;

  // Reset recovery status to idle state.
  void resetStatus();

signals:
  // Emitted when a recovery action begins execution.
  void recoveryStarted(const QString &actionId);
  // Emitted on each recovery step completion.
  void recoveryProgress(int completed, int total);
  // Emitted when recovery completes (success or failure).
  void recoveryCompleted(const RecoveryResult &result);
  // Emitted for each diagnosed error with a recovery suggestion.
  void errorDiagnosed(const QString &error, const QString &suggestion);

private:
  QVector<RecoveryAction> actions_;   // Registered recovery actions
  RecoveryStatus status_;             // Current recovery operation status
};
