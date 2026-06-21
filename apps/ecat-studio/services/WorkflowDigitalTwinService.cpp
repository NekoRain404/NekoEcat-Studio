#include "WorkflowDigitalTwinService.h"

WorkflowDigitalTwinService::WorkflowDigitalTwinService(QObject *parent)
    : QObject(parent) {}

WfDigitalTwin WorkflowDigitalTwinService::createDigitalTwin(int position) {
  WfDigitalTwin dt;
  dt.position = position;
  dt.model = QString("Workflow_%1_Model").arg(position);
  dt.state = "Created";
  dt.lastSync = QDateTime();
  dt.syncStatus = WfTwinSyncStatus::Never;
  twins_[position] = dt;
  emit digitalTwinCreated(position);
  return dt;
}

bool WorkflowDigitalTwinService::syncWithPhysical(int position) {
  if (!twins_.contains(position))
    return false;
  auto &dt = twins_[position];
  dt.lastSync = QDateTime::currentDateTime();
  dt.state = "Synced";
  dt.syncStatus = WfTwinSyncStatus::Synced;
  emit syncCompleted(position, true);
  return true;
}

WfTwinSimulationResult WorkflowDigitalTwinService::simulateScenario(const WfTwinScenario &scenario) {
  WfTwinSimulationResult result;
  result.scenarioName = scenario.name;
  result.status = WfTwinSimulationStatus::Running;
  result.startTime = QDateTime::currentDateTime();
  for (int i = 0; i < scenario.parameters.size(); i++)
    result.outputs.append(QString("Output_%1").arg(i));
  result.endTime = QDateTime::currentDateTime();
  result.status = WfTwinSimulationStatus::Completed;
  result.success = true;
  simulationHistory_.append(result);
  if (simulationHistory_.size() > kMaxSimHistory)
    simulationHistory_.removeFirst();
  emit simulationFinished(result);
  return result;
}

WfTwinPrediction WorkflowDigitalTwinService::predictBehavior(const QVector<WfTwinDataPoint> &data) {
  WfTwinPrediction pred;
  pred.model = "LinearRegression";
  pred.confidence = data.size() > 5 ? 0.85 : 0.5;
  for (int i = 0; i < 5; i++) {
    WfTwinDataPoint dp;
    dp.timestamp = QDateTime::currentDateTime().addSecs((i + 1) * 60);
    dp.value = data.isEmpty() ? 0.0 : data.last().value + (i * 0.1);
    pred.forecast.append(dp);
  }
  return pred;
}

WfDigitalTwin WorkflowDigitalTwinService::twin(int position) const {
  return twins_.value(position);
}

QVector<WfDigitalTwin> WorkflowDigitalTwinService::allTwins() const {
  QVector<WfDigitalTwin> result;
  for (auto it = twins_.constBegin(); it != twins_.constEnd(); ++it)
    result.append(it.value());
  return result;
}

bool WorkflowDigitalTwinService::removeTwin(int position) {
  if (!twins_.contains(position))
    return false;
  twins_.remove(position);
  return true;
}
