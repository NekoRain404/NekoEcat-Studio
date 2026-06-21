#pragma once

// ErrorHandlingService — detects, classifies, recovers from, and reports
// EtherCAT errors. Maintains error history and emits signals for real-time
// error monitoring.
//
// This service provides comprehensive error handling for the EtherCAT
// network. It handles:
//   - Error detection and reporting
//   - Error classification (Transient, Persistent, Fatal)
//   - Error recovery with configurable actions
//   - Error history tracking
//   - Real-time error notifications
//
// Usage:
//   ErrorHandlingService errorHandler;
//   QVector<EcatErrorInfo> errors = errorHandler.detectErrors();
//   EcatErrorClass errorClass = errorHandler.classifyError(error);
//   errorHandler.recoverFromError(error);
//   QVector<EcatErrorInfo> history = errorHandler.errorHistory();
//   int errorId = errorHandler.reportError(0, 0x1234, "Communication lost",
//     EcatErrorSeverity::Error, EcatErrorCategory::Communication, "Retry connection");
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. Error
//   operations are synchronous and block the calling thread.
//
// Performance:
//   - Error detection is O(n) where n is number of slaves
//   - Error classification is O(1)
//   - Error recovery is O(1)
//   - Error history is bounded by kMaxHistory (1000 entries)

#include <QObject>
#include <QVector>
#include <QDateTime>

// Error severity levels.
enum class EcatErrorSeverity { 
  Info,      // Informational
  Warning,   // Warning condition
  Error,     // Error condition
  Critical   // Critical failure
};

// Error categories.
enum class EcatErrorCategory { 
  Communication,  // Communication errors
  Device,         // Device errors
  Network,        // Network errors
  Configuration,  // Configuration errors
  Protocol        // Protocol errors
};

// Error classification.
enum class EcatErrorClass { 
  Transient,   // Temporary error (may resolve)
  Persistent,  // Persistent error (requires intervention)
  Fatal,       // Fatal error (requires restart)
  Unknown      // Unknown error class
};

// Error information structure.
struct EcatErrorInfo {
  int id = 0;                                          // Error ID
  QDateTime timestamp;                                 // Error timestamp
  int position = -1;                                   // Slave position
  int errorCode = 0;                                   // Error code
  QString errorMessage;                                // Human-readable error message
  EcatErrorSeverity severity = EcatErrorSeverity::Info;  // Error severity
  EcatErrorCategory category = EcatErrorCategory::Communication;  // Error category
  QString recoveryAction;                              // Suggested recovery action
};

class ErrorHandlingService : public QObject {
  Q_OBJECT
public:
  explicit ErrorHandlingService(QObject *parent = nullptr);

  // Detect errors on the EtherCAT network.
  // @return Vector of detected EcatErrorInfo structures
  QVector<EcatErrorInfo> detectErrors();

  // Classify an error.
  // @param error  EcatErrorInfo structure to classify
  // @return EcatErrorClass enumeration
  EcatErrorClass classifyError(const EcatErrorInfo &error) const;

  // Attempt to recover from an error.
  // @param error  EcatErrorInfo structure to recover from
  // @return true if recovery was successful
  bool recoverFromError(const EcatErrorInfo &error);

  // Get error history.
  // @return Vector of EcatErrorInfo structures
  QVector<EcatErrorInfo> errorHistory() const;

  // Report an error.
  // @param position       Slave position
  // @param errorCode      Error code
  // @param message        Error message
  // @param severity       Error severity
  // @param category       Error category
  // @param recoveryAction Suggested recovery action
  // @return Error ID
  int reportError(int position, int errorCode, const QString &message,
                  EcatErrorSeverity severity, EcatErrorCategory category,
                  const QString &recoveryAction = QString());

signals:
  // Emitted when an error is detected.
  // @param error  EcatErrorInfo structure
  void errorDetected(const EcatErrorInfo &error);

  // Emitted when an error is recovered.
  // @param error  EcatErrorInfo structure
  void errorRecovered(const EcatErrorInfo &error);

private:
  QVector<EcatErrorInfo> errors_;  // Error history
  int nextId_ = 1;                 // Next error ID
  static constexpr int kMaxHistory = 1000;  // Maximum history entries
};
