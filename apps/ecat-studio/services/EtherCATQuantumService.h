#pragma once
// EtherCATQuantumService -- request facade for quantum key distribution,
// quantum random number generation, quantum-safe encryption, and
// quantum-resistant signatures. No quantum/cryptographic backend is wired yet,
// so requests fail closed instead of generating synthetic keys, random data,
// encryption output, or signatures.

#include <QObject>
#include <QVector>
#include <QDateTime>
#include <QHash>
#include <QByteArray>

enum class QuantumAlgorithm { BB84, E91, B92, Lattice, HashBased };

struct QuantumKeys {
  QByteArray publicKey;
  QByteArray privateKey;
  QByteArray sharedSecret;
  int keyLength = 256;
  QuantumAlgorithm algorithm = QuantumAlgorithm::BB84;
  QDateTime expiry;
};

class EtherCATQuantumService : public QObject {
  Q_OBJECT
public:
  explicit EtherCATQuantumService(QObject *parent = nullptr);

  QuantumKeys distributeQuantumKeys(int position);
  QVector<int> generateQuantumRandom(int count);
  QByteArray encryptQuantumSafe(const QByteArray &data);
  QByteArray signQuantumResistant(const QByteArray &data);
  QuantumKeys keys(int position) const;
  bool revokeKeys(int position);

signals:
  void quantumKeysDistributed(int position, const QuantumKeys &keys);
  void keysRevoked(int position);

private:
  QByteArray generateKey(int length);
  QHash<int, QuantumKeys> distributedKeys_;
  static constexpr int kDefaultKeyLength = 256;
};
