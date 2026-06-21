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

// Log severity levels.
enum class LogLevel { 
  Debug,    // Detailed debug information
  Info,     // General information
  Warning,  // Warning conditions
  Error,    // Error conditions
  Fatal     // Fatal errors (application termination)
};

// Log categories for classification.
enum class LogCategory { 
  System,        // System-level events
  Communication, // Network/communication events
  UI,            // User interface events
  Plugin,        // Plugin-related events
  Service        // Service-related events
};

// Represents a single log entry.
struct LogEntry {
  int id = 0;                              // Unique log entry ID
  LogLevel level = LogLevel::Info;         // Severity level
  LogCategory category = LogCategory::System;  // Category
  QString message;                         // Log message
  QString source;                          // Source component
  QDateTime timestamp;                     // When the log was created
};

class LoggingService : public QObject {
  Q_OBJECT
public:
  explicit LoggingService(QObject *parent = nullptr);
  ~LoggingService();

  // Log a message with specified level and category.
  // @param level     Severity level
  // @param category  Log category
  // @param message   Log message
  // @param source    Source component (optional)
  void log(LogLevel level, LogCategory category, const QString &message,
           const QString &source = QString());

  // Set the minimum log level for filtering.
  // @param level  Minimum level to log
  void setLogLevel(LogLevel level);

  // Get the current minimum log level.
  // @return Current minimum log level
  LogLevel logLevel() const { return minLevel_; }

  // Set the log file path for file logging.
  // @param path  Path to the log file
  void setLogPath(const QString &path);

  // Get the current log file path.
  // @return Current log file path
  QString logPath() const { return logPath_; }

  // Get recent log entries.
  // @param count  Maximum number of entries to return (default: 100)
  // @return Vector of LogEntry structures, most recent first
  QVector<LogEntry> getLogs(int count = 100) const;

  // Export logs to a file.
  // @param filePath  Path to export logs to
  // @return true if export was successful
  bool exportLogs(const QString &filePath) const;

  // Clear all log entries from memory.
  void clearLogs();

signals:
  // Emitted when a new log entry is added.
  // @param entry  The new LogEntry
  void logEntryAdded(const LogEntry &entry);

private:
  // Rotate log file if it exceeds size limit.
  void rotateIfNeeded();

  // Write a log entry to the log file.
  void writeToFile(const LogEntry &entry);

  // Convert log level to string representation.
  static QString levelToString(LogLevel level);

  // Convert log category to string representation.
  static QString categoryToString(LogCategory category);

  QVector<LogEntry> entries_;              // In-memory log entries
  int nextId_ = 1;                         // Next log entry ID
  LogLevel minLevel_ = LogLevel::Debug;    // Minimum log level
  QString logPath_;                        // Log file path
  QFile *logFile_ = nullptr;               // Log file handle
  QTextStream *logStream_ = nullptr;       // Log file stream
  qint64 currentFileSize_ = 0;             // Current log file size
  static constexpr qint64 kMaxFileSize = 10 * 1024 * 1024; // 10MB per file
  static constexpr int kMaxFiles = 10;     // Maximum log files
  static constexpr int kMaxEntries = 10000; // Maximum in-memory entries
};
