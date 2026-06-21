#pragma once

// BatchProcessor — handles batch operations for multiple SDO/state requests.
// Provides progress tracking, cancellation, and error aggregation.
//
// This service provides batch processing capabilities for multiple
// EtherCAT operations. It handles:
//   - Batch execution of multiple items
//   - Progress tracking per batch and per item
//   - Cancellation support with atomic flag checking
//   - Error aggregation and reporting
//   - Asynchronous execution in separate threads
//   - Item-level completion tracking
//
// Usage:
//   BatchProcessor processor;
//   QVector<BatchItem> items;
//   BatchItem item;
//   item.id = "sdo_1";
//   item.type = "read";
//   item.params = QJsonObject{{"position", 0}, {"index", "0x6000"}};
//   items << item;
//   QString batchId = processor.startBatch("SDO Read Batch", items,
//     [](const QJsonObject &params, std::atomic<bool> &cancelled) {
//       // Process item
//       return QJsonObject{{"result", "value"}};
//     });
//   // Monitor progress
//   connect(&processor, &BatchProcessor::batchProgress, ...);
//   processor.cancelBatch(batchId);
//
// Thread safety:
//   The processor is thread-safe. Batch execution happens in separate
//   threads and is marshaled back to the main thread for signal emission.
//
// Performance:
//   - Batch execution is O(n) where n is number of items
//   - Progress tracking is O(1) per item
//   - Cancellation checks are O(1) per item
//   - Error aggregation is O(1) per item

#include <QObject>
#include <QJsonObject>
#include <QVector>
#include <QMutex>
#include <atomic>
#include <functional>

// Represents a single item in a batch operation.
struct BatchItem {
  QString id;               // Unique item identifier
  QString type;             // Item type (e.g., "read", "write", "state")
  QJsonObject params;       // Item parameters
  bool success = false;     // Whether item completed successfully
  QJsonObject result;       // Item result (on success)
  QString error;            // Error message (on failure)
};

// Batch progress information.
struct BatchProgress {
  int total = 0;      // Total number of items
  int completed = 0;   // Number of completed items
  int failed = 0;      // Number of failed items
  int percent = 0;     // Progress percentage (0-100)
};

// Function signature for batch item processing.
// @param params     Item parameters
// @param cancelled  Atomic flag to check for cancellation
// @return JSON result data
using BatchItemFunc = std::function<QJsonObject(const QJsonObject &params, std::atomic<bool> &cancelled)>;

class BatchProcessor : public QObject {
  Q_OBJECT
public:
  explicit BatchProcessor(QObject *parent = nullptr);

  // Start a new batch operation.
  // @param name   Batch name for identification
  // @param items  Vector of BatchItem structures
  // @param func   Function to process each item
  // @return Batch ID for tracking
  QString startBatch(const QString &name, QVector<BatchItem> items, BatchItemFunc func);

  // Cancel a batch operation.
  // @param batchId  Batch ID to cancel
  // @return true if batch was found and cancelled
  bool cancelBatch(const QString &batchId);

  // Get the progress of a batch operation.
  // @param batchId  Batch ID to check
  // @return BatchProgress structure
  BatchProgress progress(const QString &batchId) const;

  // Get the results of a completed batch operation.
  // @param batchId  Batch ID to get results for
  // @return Vector of BatchItem structures with results
  QVector<BatchItem> results(const QString &batchId) const;

signals:
  // Emitted when a batch operation starts.
  // @param batchId  Batch ID
  void batchStarted(const QString &batchId);

  // Emitted when batch progress is updated.
  // @param batchId   Batch ID
  // @param progress  BatchProgress structure
  void batchProgress(const QString &batchId, const BatchProgress &progress);

  // Emitted when a batch operation completes successfully.
  // @param batchId  Batch ID
  // @param results  Vector of completed BatchItem structures
  void batchCompleted(const QString &batchId, const QVector<BatchItem> &results);

  // Emitted when a batch operation fails.
  // @param batchId  Batch ID
  // @param error    Human-readable error message
  void batchFailed(const QString &batchId, const QString &error);

  // Emitted when a single item completes.
  // @param batchId  Batch ID
  // @param item     Completed BatchItem structure
  void itemCompleted(const QString &batchId, const BatchItem &item);

private:
  // Internal batch state structure.
  struct BatchState {
    QString name;                        // Batch name
    QVector<BatchItem> items;            // Items to process
    BatchItemFunc func;                  // Processing function
    std::atomic<bool> cancelled{false};  // Cancellation flag
    std::atomic<int> completed{0};       // Completed items count
    std::atomic<int> failed{0};          // Failed items count
  };

  mutable QMutex mutex_;                    // Thread-safe mutex
  QHash<QString, BatchState *> batches_;    // Active batches by ID
};
