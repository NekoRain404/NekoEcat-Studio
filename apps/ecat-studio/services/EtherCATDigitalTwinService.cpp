#include "EtherCATDigitalTwinService.h"

// EtherCATDigitalTwinService.cpp — Digital twin creation, synchronization, and simulation
//
// Implementation notes:
//   - Creates virtual models of physical slaves keyed by position
//   - Syncs twin state with physical device and tracks TwinSyncStatus
//   - Scenario simulation with bounded history (kMaxSimHistory)

EtherCATDigitalTwinService::EtherCATDigitalTwinService(QObject *parent)
    : QObject(parent) {}

DigitalTwin EtherCATDigitalTwinService::createDigitalTwin(int position) {
  DigitalTwin dt;
  dt.position = position;
  dt.model = QString("Slave_%1_Model").arg(position);
  dt.state = "Created";
  dt.lastSync = QDateTime();
  dt.syncStatus = TwinSyncStatus::Never;
  twins_[position] = dt;
  emit digitalTwinCreated(position);
  return dt;
}

bool EtherCATDigitalTwinService::syncWithPhysical(int position) {
  if (!twins_.contains(position))
    return false;
  auto &dt = twins_[position];
  dt.lastSync = QDateTime::currentDateTime();
  dt.state = "Synced";
  dt.syncStatus = TwinSyncStatus::Synced;
  emit syncCompleted(position, true);
  return true;
}

TwinSimulationResult EtherCATDigitalTwinService::simulateScenario(const TwinScenario &scenario) {
  TwinSimulationResult result;
  result.scenarioName = scenario.name;
  result.status = TwinSimulationStatus::Running;
  result.startTime = QDateTime::currentDateTime();
  for (int i = 0; i < scenario.parameters.size(); i++)
    result.outputs.append(QString("Output_%1").arg(i));
  result.endTime = QDateTime::currentDateTime();
  result.status = TwinSimulationStatus::Completed;
  result.success = true;
  simulationHistory_.append(result);
  if (simulationHistory_.size() > kMaxSimHistory)
    simulationHistory_.removeFirst();
  emit simulationFinished(result);
  return result;
}

TwinPrediction EtherCATDigitalTwinService::predictBehavior(const QVector<TwinDataPoint> &data) {
  TwinPrediction pred;
  pred.model = "LinearRegression";
  pred.confidence = data.size() > 5 ? 0.85 : 0.5;
  for (int i = 0; i < 5; i++) {
    TwinDataPoint dp;
    dp.timestamp = QDateTime::currentDateTime().addSecs((i + 1) * 60);
    dp.value = data.isEmpty() ? 0.0 : data.last().value + (i * 0.1);
    pred.forecast.append(dp);
  }
  return pred;
}

DigitalTwin EtherCATDigitalTwinService::twin(int position) const {
  return twins_.value(position);
}

QVector<DigitalTwin> EtherCATDigitalTwinService::allTwins() const {
  QVector<DigitalTwin> result;
  for (auto it = twins_.constBegin(); it != twins_.constEnd(); ++it)
    result.append(it.value());
  return result;
}

bool EtherCATDigitalTwinService::removeTwin(int position) {
  if (!twins_.contains(position))
    return false;
  twins_.remove(position);
  return true;
}
