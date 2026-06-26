#include "WorkflowQuantumService.h"

WorkflowQuantumService::WorkflowQuantumService(QObject *parent)
    : QObject(parent) {}

WfQuantumKeys WorkflowQuantumService::distributeQuantumKeys(int position) {
  Q_UNUSED(position);
  WfQuantumKeys keys;
  return keys;
}

QVector<int> WorkflowQuantumService::generateQuantumRandom(int count) {
  Q_UNUSED(count);
  return {};
}

QByteArray WorkflowQuantumService::encryptQuantumSafe(const QByteArray &data) {
  Q_UNUSED(data);
  return {};
}

QByteArray WorkflowQuantumService::signQuantumResistant(const QByteArray &data) {
  Q_UNUSED(data);
  return {};
}

WfQuantumKeys WorkflowQuantumService::keys(int position) const {
  return distributedKeys_.value(position);
}

bool WorkflowQuantumService::revokeKeys(int position) {
  Q_UNUSED(position);
  return false;
}

QByteArray WorkflowQuantumService::generateKey(int length) {
  Q_UNUSED(length);
  return {};
}
