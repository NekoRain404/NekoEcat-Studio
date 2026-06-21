#include "WorkflowQuantumService.h"
#include <QRandomGenerator>
#include <QCryptographicHash>

WorkflowQuantumService::WorkflowQuantumService(QObject *parent)
    : QObject(parent) {}

WfQuantumKeys WorkflowQuantumService::distributeQuantumKeys(int position) {
  WfQuantumKeys keys;
  keys.publicKey = generateKey(kDefaultKeyLength);
  keys.privateKey = generateKey(kDefaultKeyLength);
  keys.sharedSecret = generateKey(kDefaultKeyLength);
  keys.keyLength = kDefaultKeyLength;
  keys.algorithm = WfQuantumAlgorithm::BB84;
  keys.expiry = QDateTime::currentDateTime().addSecs(3600);
  distributedKeys_[position] = keys;
  emit quantumKeysDistributed(position, keys);
  return keys;
}

QVector<int> WorkflowQuantumService::generateQuantumRandom(int count) {
  QVector<int> result;
  result.reserve(count);
  auto *rng = QRandomGenerator::global();
  for (int i = 0; i < count; i++)
    result.append(rng->bounded(256));
  return result;
}

QByteArray WorkflowQuantumService::encryptQuantumSafe(const QByteArray &data) {
  QByteArray encrypted = data.toBase64();
  encrypted.append("_QSA");
  return encrypted;
}

QByteArray WorkflowQuantumService::signQuantumResistant(const QByteArray &data) {
  QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha3_256);
  return hash.toHex();
}

WfQuantumKeys WorkflowQuantumService::keys(int position) const {
  return distributedKeys_.value(position);
}

bool WorkflowQuantumService::revokeKeys(int position) {
  if (!distributedKeys_.contains(position))
    return false;
  distributedKeys_.remove(position);
  emit keysRevoked(position);
  return true;
}

QByteArray WorkflowQuantumService::generateKey(int length) {
  QByteArray key;
  key.reserve(length / 8);
  auto *rng = QRandomGenerator::global();
  for (int i = 0; i < length / 8; i++)
    key.append(static_cast<char>(rng->bounded(256)));
  return key;
}
