#pragma once

// AsyncOperationManager — manages asynchronous EtherCAT operations with
// priority queuing, progress tracking, timeout handling, and cancellation.
//
// This service provides a priority-queued async execution framework for
// long-running EtherCAT operations. It handles:
//   - Priority-based operation queuing (Low, Normal, High, Critical)
//   - Concurrent operation execution with configurable limit
//   - Progress tracking per operation
//   - Timeout handling with automatic cancellation
//   - Operation cancellation (single or all)
//   - Result aggregation and error reporting
//
// Usage:
//   AsyncOperationManager manager(4);  // Max 4 concurrent operations
//   QString opId = manager.execute("SDO Read", [](std::atomic<bool> &cancel) {
//     // Long-running operation
//     return QJsonObject{{"result", "value"}};
//   }, OperationPriority::High, 30000);
//   // Monitor progress
//   connect(&manager, &AsyncOperationManager::operationProgress, ...);
//
// Thread safety:
//   The manager is thread-safe. Operations execute in separate threads
//   and are marshaled back to the main thread for signal emission.
//
// Performance:
//   - Queue operations are O(log n) for priority insertion
//   - Operation lookup is O(1) by ID
//   - Concurrent execution is bounded by maxConcurrent parameter

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QMutex>
#include <QMap>
#include <QQueue>
#include <QUuid>
#include <functional>
#include <atomic>

// Priority levels for async operations.
enum class OperationPriority {
  Low = 0,       // Background tasks, non-urgent
  Normal = 1,    // Standard operations (default)
  High = 2,      // Important operations, expedited
  Critical = 3   // Safety-critical operations, highest priority
};

// States for async operations.
enum class OperationState {
  Pending,    // Waiting in queue
  Running,    // Currently executing
  Completed,  // Successfully completed
  Failed,     // Failed with error
  Cancelled   // Cancelled by user or timeout
};

// Result of an async operation.
struct OperationResult {
  bool success = false;   // Whether the operation succeeded
  QJsonObject data;       // Result data (on success)
  QString error;          // Error message (on failure)
};

// Function signature for async operations.
// @param cancelled  Atomic flag to check for cancellation
// @return JSON result data
using OperationFunc = std::function<QJsonObject(std::atomic<bool> &cancelled)>;

// Represents a single async operation.
struct Operation {
  QString id;                                    // Unique operation ID
  QString name;                                  // Human-readable name
  OperationPriority priority = OperationPriority::Normal;  // Priority level
  OperationState state = OperationState::Pending;  // Current state
  int progress = 0;                              // Progress percentage (0-100)
  int timeoutMs = 30000;                         // Timeout in milliseconds
  OperationFunc func;                            // Operation function
  OperationResult result;                        // Operation result
  std::atomic<bool> cancelled{false};            // Cancellation flag
};

class AsyncOperationManager : public QObject {
  Q_OBJECT
public:
  static constexpr int kMaxConcurrent = 4;  // Default maximum concurrent operations

  explicit AsyncOperationManager(int maxConcurrent = kMaxConcurrent,
                                 QObject *parent = nullptr);
  ~AsyncOperationManager() override;

  // Execute an async operation.
  // @param name      Human-readable operation name
  // @param func      Operation function to execute
  // @param priority  Priority level (default: Normal)
  // @param timeoutMs Timeout in milliseconds (default: 30000ms)
  // @return Operation ID for tracking
  QString execute(const QString &name, OperationFunc func,
                  OperationPriority priority = OperationPriority::Normal,
                  int timeoutMs = 30000);

  // Cancel a specific operation by ID.
  // @param operationId  Operation ID returned by execute()
  // @return true if the operation was found and cancelled
  bool cancel(const QString &operationId);

  // Check if an operation is currently running.
  // @param operationId  Operation ID to check
  // @return true if the operation is running
  bool isRunning(const QString &operationId) const;

  // Get the progress of a specific operation.
  // @param operationId  Operation ID to check
  // @return Progress percentage (0-100)
  int progress(const QString &operationId) const;

  // Get the result of a completed operation.
  // @param operationId  Operation ID to get result for
  // @return OperationResult structure
  OperationResult result(const QString &operationId) const;

  // Cancel all pending and running operations.
  void cancelAll();

signals:
  // Emitted when an operation completes successfully.
  // @param operationId  Operation ID
  // @param result       Operation result
  void operationCompleted(const QString &operationId, const OperationResult &result);

  // Emitted when an operation fails.
  // @param operationId  Operation ID
  // @param error        Error message
  void operationFailed(const QString &operationId, const QString &error);

  // Emitted when operation progress is updated.
  // @param operationId  Operation ID
  // @param progress     Progress percentage (0-100)
  void operationProgress(const QString &operationId, int progress);

  // Emitted when an operation starts executing.
  // @param operationId  Operation ID
  void operationStarted(const QString &operationId);

private:
  // Process the operation queue and start pending operations.
  void processQueue();

  // Handle operation completion (success or failure).
  void onOperationFinished(const QString &operationId, const OperationResult &result);

  mutable QMutex mutex_;                    // Thread-safe mutex
  int maxConcurrent_;                       // Maximum concurrent operations
  int runningCount_ = 0;                    // Currently running operations

  QMap<QString, Operation *> operations_;   // All operations by ID
  QQueue<QString> queue_;                   // Pending operation queue (FIFO; processQueue picks the highest priority entry)

  // Retention bound for finished operations.  Finished operations are kept so
  // callers can still query result()/progress() shortly after completion, but
  // they are evicted oldest-first so the map cannot grow without bound.
  static constexpr int kMaxRetainedOperations = 64;
  QList<QString> finishedOrder_;            // Finish order, oldest-first, for eviction
};
