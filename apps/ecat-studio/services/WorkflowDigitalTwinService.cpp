#include "WorkflowDigitalTwinService.h"

WorkflowDigitalTwinService::WorkflowDigitalTwinService(QObject *parent)
    : QObject(parent) {}

WfDigitalTwin WorkflowDigitalTwinService::createDigitalTwin(int position) {
  Q_UNUSED(position);
  return {};
}

bool WorkflowDigitalTwinService::syncWithPhysical(int position) {
  Q_UNUSED(position);
  return false;
}

WfTwinSimulationResult WorkflowDigitalTwinService::simulateScenario(const WfTwinScenario &scenario) {
  WfTwinSimulationResult result;
  result.scenarioName = scenario.name;
  result.status = WfTwinSimulationStatus::Failed;
  return result;
}

WfTwinPrediction WorkflowDigitalTwinService::predictBehavior(const QVector<WfTwinDataPoint> &data) {
  Q_UNUSED(data);
  return {};
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
  Q_UNUSED(position);
  return false;
}
