#include "EtherCATDigitalTwinService.h"

EtherCATDigitalTwinService::EtherCATDigitalTwinService(QObject *parent)
    : QObject(parent) {}

DigitalTwin EtherCATDigitalTwinService::createDigitalTwin(int position) {
  Q_UNUSED(position);
  return {};
}

bool EtherCATDigitalTwinService::syncWithPhysical(int position) {
  Q_UNUSED(position);
  return false;
}

TwinSimulationResult EtherCATDigitalTwinService::simulateScenario(const TwinScenario &scenario) {
  TwinSimulationResult result;
  result.scenarioName = scenario.name;
  result.status = TwinSimulationStatus::Failed;
  return result;
}

TwinPrediction EtherCATDigitalTwinService::predictBehavior(const QVector<TwinDataPoint> &data) {
  Q_UNUSED(data);
  return {};
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
  Q_UNUSED(position);
  return false;
}
