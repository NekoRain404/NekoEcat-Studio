#pragma once

// DataPipelineService — manages a data processing pipeline with stages.
//
// Supports configurable filter, transform, and aggregate stages.
// Processes data through a sequential pipeline with configurable buffer.
// Uses QTimer-based throughput measurement.
//
// This service provides data processing pipeline capabilities for the
// EtherCAT network. It handles:
//   - Pipeline stage management (add, remove, configure)
//   - Data processing through sequential stages
//   - Configurable buffer size for data processing
//   - Throughput measurement and monitoring
//   - Pipeline start/stop control
//   - Statistics tracking per stage
//
// Usage:
//   ServiceContainer *container = ...;
//   DataPipelineService *pipeline = container->dataPipeline();
//   QVariantMap config;
//   config["threshold"] = 100;
//   int stageId = pipeline->addStage("Filter", config);
//   pipeline->start();
//   QByteArray result = pipeline->process(inputData);
//   double throughput = pipeline->throughput();
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. Data processing
//   is synchronous and blocks the calling thread.
//
// Performance:
//   - Pipeline processing is O(n*m) where n is data size, m is stages
//   - Throughput measurement is O(1) per update
//   - Buffer size limits memory usage

#include <QByteArray>
#include <QObject>
#include <QTimer>
#include <QVariantMap>
#include <QVector>

// Represents a single pipeline stage.
struct PipelineStage {
    QString name;        // Stage name
    QString type;        // Stage type (filter, transform, aggregate)
    QVariantMap config;  // Stage configuration
    bool active = false; // Whether stage is active
    int processed = 0;   // Number of items processed
    int errors = 0;      // Number of processing errors
};

// Pipeline performance metrics.
struct PipelineMetrics {
    double throughput = 0.0; // Throughput in bytes per second
    double latencyMs = 0.0;  // Average latency in milliseconds
};

class DataPipelineService : public QObject {
    Q_OBJECT
public:
    explicit DataPipelineService(QObject* parent = nullptr);

    // Add a new stage to the pipeline.
    // @param name    Stage name
    // @param config  Stage configuration
    // @return Stage index
    int addStage(const QString& name, const QVariantMap& config);

    // Remove a stage from the pipeline.
    // @param index  Stage index to remove
    void removeStage(int index);

    // Get the number of stages in the pipeline.
    // @return Number of stages
    int stageCount() const;

    // Process data through the pipeline.
    // @param input  Input data
    // @return Processed output data
    QByteArray process(const QByteArray& input);

    // Set the buffer size for data processing.
    // @param bytes  Buffer size in bytes
    void setBufferSize(int bytes);

    // Get the current buffer size.
    // @return Buffer size in bytes
    int bufferSize() const;

    // Get the current throughput.
    // @return Throughput in bytes per second
    double throughput() const;

    // Check if the pipeline is running.
    // @return true if pipeline is running
    bool isRunning() const;

    // Start the pipeline.
    void start();

    // Stop the pipeline.
    void stop();

    // Add a default stage with standard configuration.
    // @return Stage index
    int addDefaultStage();

    // Reset all stage statistics.
    void resetStatistics();

    // Get all stages in the pipeline.
    // @return Vector of PipelineStage structures
    QVector<PipelineStage> allStages() const;

    // Get pipeline performance metrics.
    // @return PipelineMetrics structure
    PipelineMetrics pipelineMetrics() const;

signals:
    // Emitted when a stage completes processing.
    // @param index   Stage index
    // @param result  Processed data
    void stageCompleted(int index, const QByteArray& result);

    // Emitted when the entire pipeline completes processing.
    // @param result  Final processed data
    void pipelineFinished(const QByteArray& result);

    // Emitted when data is processed.
    // @param bytes  Number of bytes processed
    void dataProcessed(int bytes);

    // Emitted when the pipeline is updated.
    void pipelineUpdated();

private:
    // Update throughput measurement.
    void updateThroughput();

    QVector<PipelineStage> stages_; // Pipeline stages
    QByteArray buffer_;             // Processing buffer
    int bufferSize_ = 65536;        // Buffer size (64KB default)
    bool running_ = false;          // Whether pipeline is running

    QTimer* throughputTimer_ = nullptr; // Timer for throughput measurement
    qint64 bytesProcessed_ = 0;         // Total bytes processed
    double throughput_ = 0.0;           // Current throughput
};
