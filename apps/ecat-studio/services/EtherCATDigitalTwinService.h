#pragma once
// EtherCATDigitalTwinService -- request facade for backend-backed EtherCAT
// device digital twins, physical sync, scenario simulation, and prediction.
// No twin/model/simulation backend is wired yet, so requests fail closed
// instead of creating synthetic twins, sync state, simulations, or forecasts.
//
// Thread safety:
//   All methods must be called from the main (GUI) thread.

#include <QObject>
#include <QVector>
#include <QDateTime>
#include <QHash>

// Synchronization state between digital twin and physical device.
enum class TwinSyncStatus { 
  Never,    // Never synced with physical device
  Synced,   // Currently in sync
  Stale,    // Sync data is outdated
  Error     // Sync failed
};

// Simulation execution state.
enum class TwinSimulationStatus { 
  Idle,      // No simulation running
  Running,   // Simulation in progress
  Completed, // Simulation finished successfully
  Failed     // Simulation encountered an error
};

// Virtual representation of a physical EtherCAT slave device.
struct DigitalTwin {
  int position = 0;                              // Slave bus position
  QString model;                                 // Device model identifier
  QString state;                                 // Current operational state
  QDateTime lastSync;                            // Last sync timestamp
  TwinSyncStatus syncStatus = TwinSyncStatus::Never; // Sync status
  QVector<QString> simulationResults;            // Historical simulation outputs
};

// Defines a simulation scenario with parameters and duration.
struct TwinScenario {
  QString name;                    // Scenario identifier
  QString description;             // Human-readable description
  QVariantMap parameters;          // Scenario-specific parameters
  int durationMs = 1000;           // Simulation duration in milliseconds
};

// Result of a completed simulation run.
struct TwinSimulationResult {
  QString scenarioName;                            // Name of executed scenario
  TwinSimulationStatus status = TwinSimulationStatus::Idle; // Execution status
  QDateTime startTime;                             // Simulation start time
  QDateTime endTime;                               // Simulation end time
  QVector<QString> outputs;                        // Simulation output messages
  bool success = false;                            // Whether simulation passed
};

// Single data point for time-series prediction.
struct TwinDataPoint {
  QDateTime timestamp;  // Measurement timestamp
  double value = 0.0;   // Measured value
};

// Predicted future behavior from historical data.
struct TwinPrediction {
  QVector<TwinDataPoint> forecast;  // Predicted data points
  double confidence = 0.0;          // Prediction confidence (0.0–1.0)
  QString model;                    // Prediction model used
};

class EtherCATDigitalTwinService : public QObject {
  Q_OBJECT
public:
  explicit EtherCATDigitalTwinService(QObject *parent = nullptr);

  // Create a digital twin for the slave at the given bus position.
  // @param position  Slave position on the bus (0-based)
  // @return The created DigitalTwin instance
  // Emits digitalTwinCreated() on success.
  DigitalTwin createDigitalTwin(int position);

  // Synchronize the digital twin state with the physical device.
  // @param position  Slave position on the bus (0-based)
  // @return true if sync succeeded, false otherwise
  // Emits syncCompleted() with the result.
  bool syncWithPhysical(int position);

  // Run a simulation scenario on the digital twin.
  // @param scenario  Scenario definition with parameters and duration
  // @return Simulation result with outputs and status
  // Emits simulationFinished() when complete.
  TwinSimulationResult simulateScenario(const TwinScenario &scenario);

  // Predict future behavior from historical time-series data.
  // @param data  Historical data points for prediction
  // @return Prediction with forecast and confidence level
  TwinPrediction predictBehavior(const QVector<TwinDataPoint> &data);

  // Get the digital twin for a specific slave position.
  // @param position  Slave position on the bus (0-based)
  // @return The DigitalTwin, or default-constructed if not found
  DigitalTwin twin(int position) const;

  // Get all registered digital twins.
  // @return Vector of all DigitalTwin instances
  QVector<DigitalTwin> allTwins() const;

  // Remove a digital twin for the given slave position.
  // @param position  Slave position on the bus (0-based)
  // @return true if removed, false if not found
  bool removeTwin(int position);

signals:
  // Emitted when a new digital twin is created.
  void digitalTwinCreated(int position);
  // Emitted when sync with physical device completes.
  void syncCompleted(int position, bool success);
  // Emitted when a simulation scenario finishes.
  void simulationFinished(const TwinSimulationResult &result);

private:
  QHash<int, DigitalTwin> twins_;                       // Twins indexed by bus position
  QVector<TwinSimulationResult> simulationHistory_;      // Bounded simulation history
  static constexpr int kMaxSimHistory = 50;              // Max simulation results retained
};
