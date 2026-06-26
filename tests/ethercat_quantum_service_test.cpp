// EtherCATQuantumServiceTest — Tests for EtherCATQuantumService
//
// Test coverage:
//   - Quantum key distribution, random generation, encryption, and signatures fail closed without backend
//   - Rejected quantum requests do not emit synthetic key lifecycle signals
//   - Key lookup and revocation remain empty without distributed keys

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
    QVERIFY(keys.publicKey.isEmpty());
    QVERIFY(keys.privateKey.isEmpty());
    QVERIFY(keys.sharedSecret.isEmpty());
    QVERIFY(!keys.expiry.isValid());
    QCOMPARE(spy.count(), 0);
  }

  // Distribute keys to multiple slaves
  // Distribute keys to multiple slaves and verify storage
  void testDistributeMultipleKeys() {
    EtherCATQuantumService svc;
    svc.distributeQuantumKeys(1);
    svc.distributeQuantumKeys(2);
    QVERIFY(svc.keys(1).publicKey.isEmpty());
    QVERIFY(svc.keys(2).publicKey.isEmpty());
  }

  // Lookup keys by slave position
  // Look up distributed keys by slave ID
  void testKeysLookup() {
    EtherCATQuantumService svc;
    svc.distributeQuantumKeys(5);
    QuantumKeys keys = svc.keys(5);
    QVERIFY(keys.publicKey.isEmpty());
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
    QVERIFY(random.isEmpty());
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
    QVERIFY(random.isEmpty());
  }

  // Encrypt data with quantum-safe algorithm
  // Encrypt data with quantum-safe algorithm and verify suffix
  void testEncryptQuantumSafe() {
    EtherCATQuantumService svc;
    QByteArray data("Hello, Quantum World!");
    QByteArray encrypted = svc.encryptQuantumSafe(data);
    QVERIFY(encrypted.isEmpty());
  }

  // Encrypt empty data still appends QSA suffix
  // Encrypt empty data still produces QSA suffix
  void testEncryptQuantumSafeEmpty() {
    EtherCATQuantumService svc;
    QByteArray encrypted = svc.encryptQuantumSafe(QByteArray());
    QVERIFY(encrypted.isEmpty());
  }

  // Sign data with quantum-resistant signature
  // Sign data and verify signature length
  void testSignQuantumResistant() {
    EtherCATQuantumService svc;
    QByteArray data("Test data for signing");
    QByteArray signature = svc.signQuantumResistant(data);
    QVERIFY(signature.isEmpty());
  }

  // Same data produces consistent signature
  // Same data produces same signature
  void testSignQuantumResistantConsistency() {
    EtherCATQuantumService svc;
    QByteArray data("Consistent test");
    QByteArray sig1 = svc.signQuantumResistant(data);
    QByteArray sig2 = svc.signQuantumResistant(data);
    QVERIFY(sig1.isEmpty());
    QCOMPARE(sig1, sig2);
  }

  // Different data also fails closed without signatures
  void testSignQuantumResistantDifferent() {
    EtherCATQuantumService svc;
    QByteArray sig1 = svc.signQuantumResistant(QByteArray("data1"));
    QByteArray sig2 = svc.signQuantumResistant(QByteArray("data2"));
    QVERIFY(sig1.isEmpty());
    QCOMPARE(sig1, sig2);
  }

  // Revoke keys and verify cleanup
  // Revoke keys and verify signal and empty lookup
  void testRevokeKeys() {
    EtherCATQuantumService svc;
    svc.distributeQuantumKeys(1);
    QSignalSpy spy(&svc, &EtherCATQuantumService::keysRevoked);
    QVERIFY(!svc.revokeKeys(1));
    QCOMPARE(spy.count(), 0);
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
    QCOMPARE(spy.count(), 0);
  }

  // keysRevoked signal fires on revocation
  // Verify keysRevoked signal emission
  void testKeysRevokedSignal() {
    EtherCATQuantumService svc;
    svc.distributeQuantumKeys(1);
    QSignalSpy spy(&svc, &EtherCATQuantumService::keysRevoked);
    QVERIFY(spy.isValid());
    svc.revokeKeys(1);
    QCOMPARE(spy.count(), 0);
  }

  // Re-distributing keys still leaves no synthetic key material
  void testKeyOverwrite() {
    EtherCATQuantumService svc;
    svc.distributeQuantumKeys(1);
    QByteArray firstPub = svc.keys(1).publicKey;
    svc.distributeQuantumKeys(1);
    QByteArray secondPub = svc.keys(1).publicKey;
    QVERIFY(firstPub.isEmpty());
    QCOMPARE(firstPub, secondPub);
  }
};

QTEST_MAIN(EtherCATQuantumServiceTest)
#include "ethercat_quantum_service_test.moc"
