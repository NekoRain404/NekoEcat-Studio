// WorkflowQuantumServiceTest — Tests for WorkflowQuantumService
//
// Test coverage:
//   - Quantum key distribution
//   - Quantum random number generation
//   - Quantum-safe encryption
//   - Quantum-resistant signatures
//   - Key revocation and signal emission

#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowQuantumService.h"

class WorkflowQuantumServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testDistributeQuantumKeys() {
    WorkflowQuantumService svc;
    QSignalSpy spy(&svc, &WorkflowQuantumService::quantumKeysDistributed);
    WfQuantumKeys keys = svc.distributeQuantumKeys(1);
    QCOMPARE(keys.keyLength, 256);
    QCOMPARE(keys.algorithm, WfQuantumAlgorithm::BB84);
    QVERIFY(!keys.publicKey.isEmpty());
    QVERIFY(!keys.privateKey.isEmpty());
    QVERIFY(!keys.sharedSecret.isEmpty());
    QVERIFY(keys.expiry.isValid());
    QCOMPARE(spy.count(), 1);
  }

  void testKeysLookup() {
    WorkflowQuantumService svc;
    svc.distributeQuantumKeys(5);
    WfQuantumKeys keys = svc.keys(5);
    QCOMPARE(keys.keyLength, 256);
    QVERIFY(!keys.publicKey.isEmpty());
  }

  void testKeysNotFound() {
    WorkflowQuantumService svc;
    WfQuantumKeys keys = svc.keys(99);
    QVERIFY(keys.publicKey.isEmpty());
  }

  void testGenerateQuantumRandom() {
    WorkflowQuantumService svc;
    QVector<int> random = svc.generateQuantumRandom(10);
    QCOMPARE(random.size(), 10);
    for (int val : random) {
      QVERIFY(val >= 0 && val < 256);
    }
  }

  void testGenerateQuantumRandomZero() {
    WorkflowQuantumService svc;
    QVector<int> random = svc.generateQuantumRandom(0);
    QCOMPARE(random.size(), 0);
  }

  void testEncryptQuantumSafe() {
    WorkflowQuantumService svc;
    QByteArray data = "Hello, Quantum World!";
    QByteArray encrypted = svc.encryptQuantumSafe(data);
    QVERIFY(encrypted != data);
    QVERIFY(encrypted.endsWith("_QSA"));
  }

  void testEncryptQuantumSafeEmpty() {
    WorkflowQuantumService svc;
    QByteArray encrypted = svc.encryptQuantumSafe({});
    QVERIFY(encrypted.endsWith("_QSA"));
  }

  void testSignQuantumResistant() {
    WorkflowQuantumService svc;
    QByteArray data = "Sign me";
    QByteArray signature = svc.signQuantumResistant(data);
    QVERIFY(!signature.isEmpty());
    QCOMPARE(signature.size(), 64);
  }

  void testSignQuantumResistantDeterministic() {
    WorkflowQuantumService svc;
    QByteArray data = "Deterministic";
    QByteArray sig1 = svc.signQuantumResistant(data);
    QByteArray sig2 = svc.signQuantumResistant(data);
    QCOMPARE(sig1, sig2);
  }

  void testRevokeKeys() {
    WorkflowQuantumService svc;
    svc.distributeQuantumKeys(1);
    QSignalSpy spy(&svc, &WorkflowQuantumService::keysRevoked);
    QVERIFY(svc.revokeKeys(1));
    QCOMPARE(spy.count(), 1);
    WfQuantumKeys keys = svc.keys(1);
    QVERIFY(keys.publicKey.isEmpty());
  }

  void testRevokeKeysNotFound() {
    WorkflowQuantumService svc;
    QVERIFY(!svc.revokeKeys(99));
  }

  void testQuantumKeysDistributedSignal() {
    WorkflowQuantumService svc;
    QSignalSpy spy(&svc, &WorkflowQuantumService::quantumKeysDistributed);
    QVERIFY(spy.isValid());
    svc.distributeQuantumKeys(1);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 1);
  }

  void testMultipleKeyDistribution() {
    WorkflowQuantumService svc;
    svc.distributeQuantumKeys(1);
    svc.distributeQuantumKeys(2);
    svc.distributeQuantumKeys(3);
    WfQuantumKeys k1 = svc.keys(1);
    WfQuantumKeys k2 = svc.keys(2);
    WfQuantumKeys k3 = svc.keys(3);
    QVERIFY(!k1.publicKey.isEmpty());
    QVERIFY(!k2.publicKey.isEmpty());
    QVERIFY(!k3.publicKey.isEmpty());
  }

  void testKeysExpiry() {
    WorkflowQuantumService svc;
    WfQuantumKeys keys = svc.distributeQuantumKeys(1);
    QVERIFY(keys.expiry > QDateTime::currentDateTime());
  }
};

QTEST_MAIN(WorkflowQuantumServiceTest)
#include "workflow_quantum_service_test.moc"
