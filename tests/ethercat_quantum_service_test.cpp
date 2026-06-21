// EtherCATQuantumServiceTest — Tests for EtherCATQuantumService
//
// Test coverage:
//   - Quantum key distribution (single, multiple, lookup, not found)
//   - Quantum random number generation (zero, normal, large)
//   - Quantum-safe encryption and signing
//   - Key revocation and overwrite
//   - Signal emission for key distribution and revocation

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATQuantumService.h"

class EtherCATQuantumServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Distribute quantum keys to a slave
  // Distribute quantum keys and verify key length, algorithm, and signal
  void testDistributeQuantumKeys() {
    EtherCATQuantumService svc;
    QSignalSpy spy(&svc, &EtherCATQuantumService::quantumKeysDistributed);
    QuantumKeys keys = svc.distributeQuantumKeys(1);
    QCOMPARE(keys.keyLength, 256);
    QCOMPARE(keys.algorithm, QuantumAlgorithm::BB84);
    QVERIFY(keys.publicKey.size() > 0);
    QVERIFY(keys.privateKey.size() > 0);
    QVERIFY(keys.sharedSecret.size() > 0);
    QVERIFY(keys.expiry.isValid());
    QCOMPARE(spy.count(), 1);
  }

  // Distribute keys to multiple slaves
  // Distribute keys to multiple slaves and verify storage
  void testDistributeMultipleKeys() {
    EtherCATQuantumService svc;
    svc.distributeQuantumKeys(1);
    svc.distributeQuantumKeys(2);
    QCOMPARE(svc.keys(1).publicKey.size(), 32);
    QCOMPARE(svc.keys(2).publicKey.size(), 32);
  }

  // Lookup keys by slave position
  // Look up distributed keys by slave ID
  void testKeysLookup() {
    EtherCATQuantumService svc;
    svc.distributeQuantumKeys(5);
    QuantumKeys keys = svc.keys(5);
    QCOMPARE(keys.keyLength, 256);
  }

  // Lookup nonexistent keys returns empty
  // Lookup nonexistent slave returns empty keys
  void testKeysNotFound() {
    EtherCATQuantumService svc;
    QuantumKeys keys = svc.keys(99);
    QVERIFY(keys.publicKey.isEmpty());
  }

  // Generate quantum random numbers in range
  // Generate random numbers within valid range
  void testGenerateQuantumRandom() {
    EtherCATQuantumService svc;
    QVector<int> random = svc.generateQuantumRandom(100);
    QCOMPARE(random.size(), 100);
    for (int val : random) {
      QVERIFY(val >= 0);
      QVERIFY(val < 256);
    }
  }

  // Generate zero random numbers
  // Generate zero random numbers returns empty vector
  void testGenerateQuantumRandomZero() {
    EtherCATQuantumService svc;
    QVector<int> random = svc.generateQuantumRandom(0);
    QCOMPARE(random.size(), 0);
  }

  // Generate large batch of random numbers
  // Generate large batch of random numbers
  void testGenerateQuantumRandomLarge() {
    EtherCATQuantumService svc;
    QVector<int> random = svc.generateQuantumRandom(10000);
    QCOMPARE(random.size(), 10000);
  }

  // Encrypt data with quantum-safe algorithm
  // Encrypt data with quantum-safe algorithm and verify suffix
  void testEncryptQuantumSafe() {
    EtherCATQuantumService svc;
    QByteArray data("Hello, Quantum World!");
    QByteArray encrypted = svc.encryptQuantumSafe(data);
    QVERIFY(encrypted != data);
    QVERIFY(encrypted.endsWith("_QSA"));
  }

  // Encrypt empty data still appends QSA suffix
  // Encrypt empty data still produces QSA suffix
  void testEncryptQuantumSafeEmpty() {
    EtherCATQuantumService svc;
    QByteArray encrypted = svc.encryptQuantumSafe(QByteArray());
    QVERIFY(encrypted.endsWith("_QSA"));
  }

  // Sign data with quantum-resistant signature
  // Sign data and verify signature length
  void testSignQuantumResistant() {
    EtherCATQuantumService svc;
    QByteArray data("Test data for signing");
    QByteArray signature = svc.signQuantumResistant(data);
    QVERIFY(signature.size() > 0);
    QCOMPARE(signature.size(), 64);
  }

  // Same data produces consistent signature
  // Same data produces same signature
  void testSignQuantumResistantConsistency() {
    EtherCATQuantumService svc;
    QByteArray data("Consistent test");
    QByteArray sig1 = svc.signQuantumResistant(data);
    QByteArray sig2 = svc.signQuantumResistant(data);
    QCOMPARE(sig1, sig2);
  }

  // Different data produces different signatures
  // Different data produces different signatures
  void testSignQuantumResistantDifferent() {
    EtherCATQuantumService svc;
    QByteArray sig1 = svc.signQuantumResistant(QByteArray("data1"));
    QByteArray sig2 = svc.signQuantumResistant(QByteArray("data2"));
    QVERIFY(sig1 != sig2);
  }

  // Revoke keys and verify cleanup
  // Revoke keys and verify signal and empty lookup
  void testRevokeKeys() {
    EtherCATQuantumService svc;
    svc.distributeQuantumKeys(1);
    QSignalSpy spy(&svc, &EtherCATQuantumService::keysRevoked);
    QVERIFY(svc.revokeKeys(1));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 1);
    QuantumKeys keys = svc.keys(1);
    QVERIFY(keys.publicKey.isEmpty());
  }

  // Revoke nonexistent keys fails
  // Revoke nonexistent keys returns false
  void testRevokeKeysNotFound() {
    EtherCATQuantumService svc;
    QVERIFY(!svc.revokeKeys(99));
  }

  // quantumKeysDistributed signal carries position
  // Verify quantumKeysDistributed signal emission
  void testQuantumKeysDistributedSignal() {
    EtherCATQuantumService svc;
    QSignalSpy spy(&svc, &EtherCATQuantumService::quantumKeysDistributed);
    QVERIFY(spy.isValid());
    svc.distributeQuantumKeys(1);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 1);
  }

  // keysRevoked signal fires on revocation
  // Verify keysRevoked signal emission
  void testKeysRevokedSignal() {
    EtherCATQuantumService svc;
    svc.distributeQuantumKeys(1);
    QSignalSpy spy(&svc, &EtherCATQuantumService::keysRevoked);
    QVERIFY(spy.isValid());
    svc.revokeKeys(1);
    QCOMPARE(spy.count(), 1);
  }

  // Re-distributing keys overwrites previous keys
  // Distributing keys twice overwrites with new keys
  void testKeyOverwrite() {
    EtherCATQuantumService svc;
    svc.distributeQuantumKeys(1);
    QByteArray firstPub = svc.keys(1).publicKey;
    svc.distributeQuantumKeys(1);
    QByteArray secondPub = svc.keys(1).publicKey;
    QVERIFY(firstPub != secondPub);
  }
};

QTEST_MAIN(EtherCATQuantumServiceTest)
#include "ethercat_quantum_service_test.moc"
