#include "EtherCATQuantumService.h"

EtherCATQuantumService::EtherCATQuantumService(QObject *parent)
    : QObject(parent) {}

QuantumKeys EtherCATQuantumService::distributeQuantumKeys(int position) {
  Q_UNUSED(position);
  QuantumKeys keys;
  return keys;
}

QVector<int> EtherCATQuantumService::generateQuantumRandom(int count) {
  Q_UNUSED(count);
  return {};
}

QByteArray EtherCATQuantumService::encryptQuantumSafe(const QByteArray &data) {
  Q_UNUSED(data);
  return {};
}

QByteArray EtherCATQuantumService::signQuantumResistant(const QByteArray &data) {
  Q_UNUSED(data);
  return {};
}

QuantumKeys EtherCATQuantumService::keys(int position) const {
  return distributedKeys_.value(position);
}

bool EtherCATQuantumService::revokeKeys(int position) {
  Q_UNUSED(position);
  return false;
}

QByteArray EtherCATQuantumService::generateKey(int length) {
  Q_UNUSED(length);
  return {};
}
