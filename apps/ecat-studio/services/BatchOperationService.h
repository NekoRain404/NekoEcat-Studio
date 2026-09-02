#pragma once

// BatchOperationService — executes batch SDO/state/topology operations with
// progress tracking and cancellation support.
//
// This service provides batch operation capabilities for efficient bulk
// operations on the EtherCAT network. It handles:
//   - Batch SDO read operations
//   - Batch SDO write operations
//   - Batch state change operations
//   - Batch topology scan operations
//   - Progress tracking per batch
//   - Cancellation support
//   - Error aggregation
//
// Usage:
//   ServiceContainer *container = ...;
//   BatchOperationService *batch = container->batch();
//   BatchOperation op;
//   op.type = BatchType::ReadSDO;
//   op.reads << BatchReadSDOItem{0, "0x6000", "0x01"};
//   op.reads << BatchReadSDOItem{0, "0x6000", "0x02"};
//   BatchResult result = batch->executeBatch(op);
//   if (result.success) {
//     // Process results
//   }
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. Batch operations
//   execute synchronously and block the calling thread.
//
// Performance:
//   - Batch operations are O(n) where n is number of items
//   - Progress tracking adds minimal overhead
//   - Cancellation checks are O(1) per item

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVector>

class EcatClient;
class SdoService;
class TopologyService;

/// @brief Types of batch operations.
enum class BatchType {
    ReadSDO,     ///< Batch SDO read operations.
    WriteSDO,    ///< Batch SDO write operations.
    SetState,    ///< Batch state change operations.
    ScanTopology ///< Batch topology scan operations.
};

/// @brief Item for batch SDO read operations.
struct BatchReadSDOItem {
    int position = 0; ///< Slave position on the bus.
    QString index;    ///< SDO index in hex format.
    QString subIndex; ///< SDO subindex in hex format.
};

/// @brief Item for batch SDO write operations.
struct BatchWriteSDOItem {
    int position = 0; ///< Slave position on the bus.
    QString index;    ///< SDO index in hex format.
    QString subIndex; ///< SDO subindex in hex format.
    QString value;    ///< Value to write.
    QString type;     ///< Data type (e.g., "UINT8", "INT16").
};

/// @brief Item for batch state change operations.
struct BatchSetStateItem {
    int position = 0; ///< Slave position on the bus.
    QString state;    ///< Target state (e.g., "OP", "PREOP").
};

/// @brief Represents a batch operation to execute.
struct BatchOperation {
    BatchType type;                          ///< Type of batch operation.
    QVector<BatchReadSDOItem> reads;         ///< SDO read items.
    QVector<BatchWriteSDOItem> writes;       ///< SDO write items.
    QVector<BatchSetStateItem> stateChanges; ///< State change items.
    int scanCount = 1;                       ///< Number of topology scans to perform.
};

/// @brief Result of a batch operation.
struct BatchResult {
    bool success = false;   ///< Whether the batch completed successfully.
    int completedItems = 0; ///< Number of items completed.
    int totalItems = 0;     ///< Total number of items in the batch.
    QVariantList results;   ///< Individual item results.
    QString error;          ///< Error message if the batch failed.
};

/// @brief Executes batch SDO/state/topology operations with progress tracking.
///
/// Provides efficient bulk operation execution on the EtherCAT network with
/// per-item progress tracking, cancellation support, and error aggregation.
class BatchOperationService : public QObject {
    Q_OBJECT
public:
    /// @brief Construct the batch operation service.
    /// @param client    TCP client to the ecatd daemon.
    /// @param sdo       SDO service for read/write operations.
    /// @param topology  Topology service for scan operations.
    /// @param parent    Parent QObject.
    explicit BatchOperationService(EcatClient* client, SdoService* sdo, TopologyService* topology,
                                   QObject* parent = nullptr);

    /// @brief Execute a batch operation synchronously.
    /// @param op  Batch operation to execute.
    /// @return BatchResult with success status, per-item results, and error info.
    BatchResult executeBatch(const BatchOperation& op);

    /// @brief Cancel the currently executing batch operation.
    /// @note Sets a flag checked between items; does not interrupt in-flight I/O.
    void cancelBatch();

    /// @brief Get the progress of the current batch operation.
    /// @return Progress percentage (0-100).
    int progress() const;

signals:
    /// @brief Emitted when a batch operation starts executing.
    /// @param op  The batch operation being executed.
    void batchStarted(const BatchOperation& op);

    /// @brief Emitted when batch progress is updated.
    /// @param progress  Progress percentage (0-100).
    void batchProgress(int progress);

    /// @brief Emitted when a batch operation completes successfully.
    /// @param result  Batch result with success status and per-item results.
    void batchCompleted(const BatchResult& result);

    /// @brief Emitted when a batch operation fails.
    /// @param error  Human-readable error message.
    void batchFailed(const QString& error);

private:
    /// @brief Execute a batch SDO read operation.
    /// @param op  Batch operation containing SDO read items.
    /// @return BatchResult with per-item read results.
    BatchResult executeReadSDO(const BatchOperation& op);

    /// @brief Execute a batch SDO write operation.
    /// @param op  Batch operation containing SDO write items.
    /// @return BatchResult with per-item write results.
    BatchResult executeWriteSDO(const BatchOperation& op);

    /// @brief Execute a batch state change operation.
    /// @param op  Batch operation containing state change items.
    /// @return BatchResult with per-item state change results.
    BatchResult executeSetState(const BatchOperation& op);

    /// @brief Execute a batch topology scan operation.
    /// @param op  Batch operation containing scan parameters.
    /// @return BatchResult with scan results.
    BatchResult executeScanTopology(const BatchOperation& op);

    EcatClient* client_;        ///< TCP client to ecatd daemon.
    SdoService* sdo_;           ///< SDO service for read/write operations.
    TopologyService* topology_; ///< Topology service for scan operations.
    int progress_ = 0;          ///< Current batch progress (0-100).
    bool cancelled_ = false;    ///< Whether batch cancellation was requested.
};
