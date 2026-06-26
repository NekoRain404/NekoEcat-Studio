#pragma once
// WorkflowDigitalTwinService -- request facade for backend-backed workflow
// digital twins, physical sync, scenario simulation, and behavior prediction.
// No twin/model/simulation backend is wired yet, so requests fail closed
// instead of creating synthetic twins, sync state, simulations, or forecasts.

#include <QObject>
#include <QVector>
#include <QDateTime>
#include <QHash>

enum class WfTwinSyncStatus { 
  Never,
  Synced,
  Stale,
  Error
};

enum class WfTwinSimulationStatus { 
  Idle,
  Running,
  Completed,
  Failed
};

struct WfDigitalTwin {
  int position = 0;
  QString model;
  QString state;
  QDateTime lastSync;
  WfTwinSyncStatus syncStatus = WfTwinSyncStatus::Never;
  QVector<QString> simulationResults;
};

struct WfTwinScenario {
  QString name;
  QString description;
  QVariantMap parameters;
  int durationMs = 1000;
};

struct WfTwinSimulationResult {
  QString scenarioName;
  WfTwinSimulationStatus status = WfTwinSimulationStatus::Idle;
  QDateTime startTime;
  QDateTime endTime;
  QVector<QString> outputs;
  bool success = false;
};

struct WfTwinDataPoint {
  QDateTime timestamp;
  double value = 0.0;
};

struct WfTwinPrediction {
  QVector<WfTwinDataPoint> forecast;
  double confidence = 0.0;
  QString model;
};

class WorkflowDigitalTwinService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowDigitalTwinService(QObject *parent = nullptr);

  WfDigitalTwin createDigitalTwin(int position);
  bool syncWithPhysical(int position);
  WfTwinSimulationResult simulateScenario(const WfTwinScenario &scenario);
  WfTwinPrediction predictBehavior(const QVector<WfTwinDataPoint> &data);
  WfDigitalTwin twin(int position) const;
  QVector<WfDigitalTwin> allTwins() const;
  bool removeTwin(int position);

signals:
  void digitalTwinCreated(int position);
  void syncCompleted(int position, bool success);
  void simulationFinished(const WfTwinSimulationResult &result);

private:
  QHash<int, WfDigitalTwin> twins_;
  QVector<WfTwinSimulationResult> simulationHistory_;
  static constexpr int kMaxSimHistory = 50;
};
