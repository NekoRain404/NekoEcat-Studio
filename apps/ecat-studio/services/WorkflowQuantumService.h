#pragma once
// WorkflowQuantumService -- request facade for quantum key distribution,
// quantum random number generation, quantum-safe encryption, and
// quantum-resistant signatures. No quantum/cryptographic backend is wired yet,
// so requests fail closed instead of generating synthetic keys, random data,
// encryption output, or signatures.

#include <QObject>
#include <QVector>
#include <QDateTime>
#include <QHash>
#include <QByteArray>

enum class WfQuantumAlgorithm { BB84, E91, B92, Lattice, HashBased };

struct WfQuantumKeys {
  QByteArray publicKey;
  QByteArray privateKey;
  QByteArray sharedSecret;
  int keyLength = 256;
  WfQuantumAlgorithm algorithm = WfQuantumAlgorithm::BB84;
  QDateTime expiry;
};

class WorkflowQuantumService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowQuantumService(QObject *parent = nullptr);

  WfQuantumKeys distributeQuantumKeys(int position);
  QVector<int> generateQuantumRandom(int count);
  QByteArray encryptQuantumSafe(const QByteArray &data);
  QByteArray signQuantumResistant(const QByteArray &data);
  WfQuantumKeys keys(int position) const;
  bool revokeKeys(int position);

signals:
  void quantumKeysDistributed(int position, const WfQuantumKeys &keys);
  void keysRevoked(int position);

private:
  QByteArray generateKey(int length);
  QHash<int, WfQuantumKeys> distributedKeys_;
  static constexpr int kDefaultKeyLength = 256;
};
