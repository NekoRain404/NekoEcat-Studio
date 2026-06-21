#include "ErrorHandlingService.h"

// ErrorHandlingService.cpp — EtherCAT error detection, classification, and recovery
//
// Implementation notes:
//   - Classifies errors by severity into Transient/Persistent/Fatal buckets
//   - Recovery downgrades non-fatal errors to Info severity
//   - Maintains a bounded history buffer capped at kMaxHistory

ErrorHandlingService::ErrorHandlingService(QObject *parent)
    : QObject(parent) {}

QVector<EcatErrorInfo> ErrorHandlingService::detectErrors() {
  QVector<EcatErrorInfo> active;
  for (const auto &e : errors_) {
    if (e.severity == EcatErrorSeverity::Error ||
        e.severity == EcatErrorSeverity::Critical) {
      active.append(e);
    }
  }
  return active;
}

// Maps error severity to an error class (Transient, Persistent, or Fatal)
EcatErrorClass ErrorHandlingService::classifyError(const EcatErrorInfo &error) const {
  switch (error.severity) {
  case EcatErrorSeverity::Info:
    return EcatErrorClass::Transient;
  case EcatErrorSeverity::Warning:
    return EcatErrorClass::Transient;
  case EcatErrorSeverity::Error:
    return EcatErrorClass::Persistent;
  case EcatErrorSeverity::Critical:
    return EcatErrorClass::Fatal;
  }
  return EcatErrorClass::Unknown;
}

// Attempts recovery by downgrading severity; returns false for Fatal errors
bool ErrorHandlingService::recoverFromError(const EcatErrorInfo &error) {
  EcatErrorClass cls = classifyError(error);
  if (cls == EcatErrorClass::Fatal)
    return false;

  for (auto &e : errors_) {
    if (e.id == error.id) {
      e.severity = EcatErrorSeverity::Info;
      e.recoveryAction = "Recovered";
      emit errorRecovered(e);
      return true;
    }
  }
  return false;
}

QVector<EcatErrorInfo> ErrorHandlingService::errorHistory() const {
  return errors_;
}

int ErrorHandlingService::reportError(int position, int errorCode,
                                      const QString &message,
                                      EcatErrorSeverity severity,
                                      EcatErrorCategory category,
                                      const QString &recoveryAction) {
  EcatErrorInfo info;
  info.id = nextId_++;
  info.timestamp = QDateTime::currentDateTime();
  info.position = position;
  info.errorCode = errorCode;
  info.errorMessage = message;
  info.severity = severity;
  info.category = category;
  info.recoveryAction = recoveryAction;

  errors_.append(info);
  if (errors_.size() > kMaxHistory)
    errors_.removeFirst();

  emit errorDetected(info);
  return info.id;
}
