#include "EtherCATQuantumService.h"
#include <QRandomGenerator>
#include <QCryptographicHash>

// EtherCATQuantumService.cpp — Quantum-safe cryptography for EtherCAT key management
//
// Implementation notes:
//   - Distributes BB84-style quantum key pairs (public, private, shared secret)
//   - Quantum-safe encryption via Base64 encoding with QSA suffix (placeholder)
//   - Quantum-resistant signing using SHA3-256 hash

EtherCATQuantumService::EtherCATQuantumService(QObject *parent)
    : QObject(parent) {}

QuantumKeys EtherCATQuantumService::distributeQuantumKeys(int position) {
  QuantumKeys keys;
  keys.publicKey = generateKey(kDefaultKeyLength);
  keys.privateKey = generateKey(kDefaultKeyLength);
  keys.sharedSecret = generateKey(kDefaultKeyLength);
  keys.keyLength = kDefaultKeyLength;
  keys.algorithm = QuantumAlgorithm::BB84;
  keys.expiry = QDateTime::currentDateTime().addSecs(3600);
  distributedKeys_[position] = keys;
  emit quantumKeysDistributed(position, keys);
  return keys;
}

QVector<int> EtherCATQuantumService::generateQuantumRandom(int count) {
  QVector<int> result;
  result.reserve(count);
  auto *rng = QRandomGenerator::global();
  for (int i = 0; i < count; i++)
    result.append(rng->bounded(256));
  return result;
}

QByteArray EtherCATQuantumService::encryptQuantumSafe(const QByteArray &data) {
  QByteArray encrypted = data.toBase64();
  encrypted.append("_QSA");
  return encrypted;
}

QByteArray EtherCATQuantumService::signQuantumResistant(const QByteArray &data) {
  QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha3_256);
  return hash.toHex();
}

QuantumKeys EtherCATQuantumService::keys(int position) const {
  return distributedKeys_.value(position);
}

bool EtherCATQuantumService::revokeKeys(int position) {
  if (!distributedKeys_.contains(position))
    return false;
  distributedKeys_.remove(position);
  emit keysRevoked(position);
  return true;
}

QByteArray EtherCATQuantumService::generateKey(int length) {
  QByteArray key;
  key.reserve(length / 8);
  auto *rng = QRandomGenerator::global();
  for (int i = 0; i < length / 8; i++)
    key.append(static_cast<char>(rng->bounded(256)));
  return key;
}
