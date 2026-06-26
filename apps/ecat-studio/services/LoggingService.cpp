#include "LoggingService.h"
#include <QDir>
#include <QFileInfo>

// LoggingService.cpp — Categorized, leveled logging with file rotation and export
//
// Implementation notes:
//   - Five log levels (Debug–Fatal) and five categories (System, Comm, UI, Plugin, Service)
//   - In-memory ring buffer capped at kMaxEntries; file rotation at kMaxFileSize
//   - Rotated files numbered .1 through .(kMaxFiles-1); oldest is deleted

LoggingService::LoggingService(QObject *parent) : QObject(parent) {}

LoggingService::~LoggingService() {
  delete logStream_;
  delete logFile_;
}

void LoggingService::log(LogLevel level, LogCategory category,
                         const QString &message, const QString &source) {
  if (level < minLevel_) {
    return;
  }

  LogEntry entry;
  entry.id = nextId_++;
  entry.level = level;
  entry.category = category;
  entry.message = message;
  entry.source = source;
  entry.timestamp = QDateTime::currentDateTime();

  entries_.append(entry);
  if (entries_.size() > kMaxEntries) {
    entries_.removeFirst();
  }

  writeToFile(entry);
  emit logEntryAdded(entry);
}

void LoggingService::setLogLevel(LogLevel level) {
  minLevel_ = level;
}

void LoggingService::setLogPath(const QString &path) {
  logPath_ = path;
  delete logStream_;
  logStream_ = nullptr;
  delete logFile_;
  logFile_ = nullptr;
  currentFileSize_ = 0;

  if (!path.isEmpty()) {
    QDir dir = QFileInfo(path).absoluteDir();
    if (!dir.exists()) {
      dir.mkpath(".");
    }
    logFile_ = new QFile(path);
    if (logFile_->open(QIODevice::WriteOnly | QIODevice::Append)) {
      logStream_ = new QTextStream(logFile_);
      currentFileSize_ = logFile_->size();
    }
  }
}

QVector<LogEntry> LoggingService::getLogs(int count) const {
  if (count >= entries_.size()) {
    return entries_;
  }
  return entries_.mid(entries_.size() - count);
}

bool LoggingService::exportLogs(const QString &filePath) const {
  if (filePath.isEmpty()) {
    return false;
  }

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return false;
  }
  QTextStream out(&file);
  for (const auto &entry : entries_) {
    out << entry.timestamp.toString(Qt::ISODate) << " ["
        << levelToString(entry.level) << "] ["
        << categoryToString(entry.category) << "] "
        << (entry.source.isEmpty() ? "" : entry.source + ": ")
        << entry.message << "\n";
  }
  return out.status() == QTextStream::Ok && file.flush();
}

void LoggingService::clearLogs() {
  entries_.clear();
}

void LoggingService::rotateIfNeeded() {
  if (currentFileSize_ < kMaxFileSize || !logFile_) {
    return;
  }

  delete logStream_;
  logStream_ = nullptr;
  logFile_->close();
  delete logFile_;
  logFile_ = nullptr;

  for (int i = kMaxFiles - 1; i > 0; --i) {
    QString oldName = logPath_ + "." + QString::number(i);
    QString newName = logPath_ + "." + QString::number(i + 1);
    if (QFileInfo::exists(oldName)) {
      if (i == kMaxFiles - 1) {
        QFile::remove(oldName);
      } else {
        QFile::rename(oldName, newName);
      }
    }
  }
  if (QFileInfo::exists(logPath_)) {
    QFile::rename(logPath_, logPath_ + ".1");
  }

  logFile_ = new QFile(logPath_);
  if (logFile_->open(QIODevice::WriteOnly | QIODevice::Append)) {
    logStream_ = new QTextStream(logFile_);
    currentFileSize_ = 0;
  }
}

void LoggingService::writeToFile(const LogEntry &entry) {
  if (!logStream_) {
    return;
  }

  rotateIfNeeded();

  QString line = entry.timestamp.toString(Qt::ISODate) + " [" +
                 levelToString(entry.level) + "] [" +
                 categoryToString(entry.category) + "] " +
                 (entry.source.isEmpty() ? "" : entry.source + ": ") +
                 entry.message + "\n";

  *logStream_ << line;
  logStream_->flush();
  currentFileSize_ += line.toUtf8().size();
}

QString LoggingService::levelToString(LogLevel level) {
  switch (level) {
  case LogLevel::Debug:   return "DEBUG";
  case LogLevel::Info:    return "INFO";
  case LogLevel::Warning: return "WARN";
  case LogLevel::Error:   return "ERROR";
  case LogLevel::Fatal:   return "FATAL";
  }
  return "INFO";
}

QString LoggingService::categoryToString(LogCategory category) {
  switch (category) {
  case LogCategory::System:        return "SYSTEM";
  case LogCategory::Communication: return "COMM";
  case LogCategory::UI:            return "UI";
  case LogCategory::Plugin:        return "PLUGIN";
  case LogCategory::Service:       return "SERVICE";
  }
  return "SYSTEM";
}
