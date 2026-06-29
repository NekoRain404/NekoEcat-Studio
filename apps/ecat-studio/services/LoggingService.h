#pragma once

// LoggingService — centralized logging with file rotation, level filtering,
// and category-based organization. All services and plugins log through this.
//
// This service provides centralized logging for the entire NekoEcat Studio
// application. It handles:
//   - Log entry creation with severity levels and categories
//   - File rotation when log files exceed size limits
//   - Level-based filtering (Debug, Info, Warning, Error, Fatal)
//   - Category-based organization (System, Communication, UI, Plugin, Service)
//   - Log export and history queries
//   - In-memory log buffer with configurable size
//
// Usage:
//   ServiceContainer *container = ...;
//   LoggingService *logging = container->logging();
//   logging->log(LogLevel::Info, LogCategory::System, "Application started", "Main");
//   logging->setLogLevel(LogLevel::Warning);
//   QVector<LogEntry> logs = logging->getLogs(100);
//   logging->exportLogs("/path/to/logs.txt");
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. File I/O
//   operations are synchronous and block the calling thread.
//
// Performance:
//   - Log entry creation is O(1)
//   - File rotation is O(1) for size check, O(n) for rotation
//   - Log queries are O(n) where n is number of entries
//   - Memory usage is bounded by kMaxEntries (10,000 entries)

#include <QObject>
#include <QVector>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

/// @brief Log severity levels.
enum class LogLevel {
  Debug,    ///< Detailed debug information.
  Info,     ///< General information.
  Warning,  ///< Warning conditions.
  Error,    ///< Error conditions.
  Fatal     ///< Fatal errors (application termination).
};

/// @brief Log categories for classification.
enum class LogCategory {
  System,        ///< System-level events.
  Communication, ///< Network/communication events.
  UI,            ///< User interface events.
  Plugin,        ///< Plugin-related events.
  Service        ///< Service-related events.
};

/// @brief Represents a single log entry with level, category, and source.
struct LogEntry {
  int id = 0;                              ///< Unique log entry ID.
  LogLevel level = LogLevel::Info;         ///< Severity level.
  LogCategory category = LogCategory::System;  ///< Log category.
  QString message;                         ///< Log message text.
  QString source;                          ///< Source component.
  QDateTime timestamp;                     ///< When the log was created.
};

/// @brief Centralized logging with file rotation, level filtering, and category-based organization.
///
/// All services and plugins log through this service. Provides log entry
/// creation, file rotation, level-based filtering, and export capabilities.
/// Memory usage is bounded by a configurable maximum entry count.
class LoggingService : public QObject {
  Q_OBJECT
public:
  /// @brief Construct the logging service.
  /// @param parent  Parent QObject.
  explicit LoggingService(QObject *parent = nullptr);

  /// @brief Destroy the logging service and close any open log files.
  ~LoggingService();

  /// @brief Log a message with the specified level and category.
  /// @param level     Severity level (Debug, Info, Warning, Error, Fatal).
  /// @param category  Log category (System, Communication, UI, Plugin, Service).
  /// @param message   Log message text.
  /// @param source    Source component that generated the log entry (optional).
  void log(LogLevel level, LogCategory category, const QString &message,
           const QString &source = QString());

  /// @brief Set the minimum log level for filtering.
  /// @param level  Minimum level to log; entries below this level are discarded.
  void setLogLevel(LogLevel level);

  /// @brief Get the current minimum log level.
  /// @return Current minimum log level.
  LogLevel logLevel() const { return minLevel_; }

  /// @brief Set the log file path for file-based logging.
  /// @param path  Path to the log file.
  void setLogPath(const QString &path);

  /// @brief Get the current log file path.
  /// @return Current log file path (empty if no file logging is configured).
  QString logPath() const { return logPath_; }

  /// @brief Get recent log entries from the in-memory buffer.
  /// @param count  Maximum number of entries to return (default: 100).
  /// @return Vector of LogEntry structures, most recent first.
  QVector<LogEntry> getLogs(int count = 100) const;

  /// @brief Export all in-memory log entries to a file.
  /// @param filePath  Path to export logs to.
  /// @return true if export was successful.
  bool exportLogs(const QString &filePath) const;

  /// @brief Clear all log entries from the in-memory buffer.
  void clearLogs();

signals:
  /// @brief Emitted when a new log entry is added to the buffer.
  /// @param entry  The new LogEntry.
  void logEntryAdded(const LogEntry &entry);

private:
  /// @brief Rotate the log file if it exceeds the maximum file size.
  void rotateIfNeeded();

  /// @brief Write a log entry to the current log file.
  /// @param entry  Log entry to write.
  void writeToFile(const LogEntry &entry);

  /// @brief Convert a log level enum to its string representation.
  /// @param level  Log level to convert.
  /// @return String representation (e.g., "DEBUG", "INFO").
  static QString levelToString(LogLevel level);

  /// @brief Convert a log category enum to its string representation.
  /// @param category  Log category to convert.
  /// @return String representation (e.g., "System", "Communication").
  static QString categoryToString(LogCategory category);

  QVector<LogEntry> entries_;              ///< In-memory log entry buffer.
  int nextId_ = 1;                         ///< Next log entry ID to assign.
  LogLevel minLevel_ = LogLevel::Debug;    ///< Minimum log level for filtering.
  QString logPath_;                        ///< Log file path.
  QFile *logFile_ = nullptr;               ///< Log file handle.
  QTextStream *logStream_ = nullptr;       ///< Log file write stream.
  qint64 currentFileSize_ = 0;             ///< Current log file size in bytes.
  /// @brief Maximum log file size before rotation (10 MB).
  static constexpr qint64 kMaxFileSize = 10 * 1024 * 1024;

  /// @brief Maximum number of rotated log files retained.
  static constexpr int kMaxFiles = 10;

  /// @brief Maximum number of entries kept in the in-memory buffer.
  static constexpr int kMaxEntries = 10000;
};
