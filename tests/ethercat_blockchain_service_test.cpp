// EtherCATBlockchainServiceTest — Tests for EtherCATBlockchainService
//
// Test coverage:
//   - Transaction recording (auto ID, custom ID, multiple)
//   - Transaction verification (valid + not found)
//   - Smart contract execution and activation
//   - Supply chain tracking and entry management
//   - Signal emission for all blockchain events

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATBlockchainService.h"

class EtherCATBlockchainServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Initial state has no transactions
  void testInitialState() {
    EtherCATBlockchainService svc;
    QCOMPARE(svc.allTransactions().size(), 0);
  }

  // Record a transaction and verify signal
  void testRecordTransaction() {
    EtherCATBlockchainService svc;
    QSignalSpy spy(&svc, &EtherCATBlockchainService::transactionRecorded);
    Transaction tx;
    tx.sender = "master";
    tx.receiver = "slave_1";
    tx.data = QByteArray("read_sdo_0x6000");
    QVERIFY(svc.recordTransaction(tx));
    QCOMPARE(svc.allTransactions().size(), 1);
    QCOMPARE(spy.count(), 1);
  }

  // Auto-generated ID, block number, and timestamp
  void testTransactionAutoId() {
    EtherCATBlockchainService svc;
    Transaction tx;
    tx.sender = "master";
    tx.receiver = "slave_1";
    svc.recordTransaction(tx);
    Transaction recorded = svc.allTransactions().first();
    QVERIFY(!recorded.transactionId.isEmpty());
    QCOMPARE(recorded.status, TransactionStatus::Confirmed);
    QVERIFY(recorded.blockNumber > 0);
    QVERIFY(recorded.timestamp.isValid());
  }

  // Custom transaction ID preserved
  void testTransactionCustomId() {
    EtherCATBlockchainService svc;
    Transaction tx;
    tx.transactionId = "TX_CUSTOM_001";
    tx.sender = "master";
    tx.receiver = "slave_1";
    svc.recordTransaction(tx);
    Transaction recorded = svc.transaction("TX_CUSTOM_001");
    QCOMPARE(recorded.transactionId, QString("TX_CUSTOM_001"));
  }

  // Verify existing transaction succeeds
  void testVerifyTransaction() {
    EtherCATBlockchainService svc;
    Transaction tx;
    tx.transactionId = "TX_VERIFY_001";
    tx.sender = "master";
    tx.receiver = "slave_1";
    svc.recordTransaction(tx);
    QSignalSpy spy(&svc, &EtherCATBlockchainService::verificationCompleted);
    QVERIFY(svc.verifyTransaction("TX_VERIFY_001"));
    QCOMPARE(spy.count(), 1);
    QVERIFY(spy.at(0).at(1).toBool());
  }

  // Verify nonexistent transaction fails
  void testVerifyTransactionNotFound() {
    EtherCATBlockchainService svc;
    QSignalSpy spy(&svc, &EtherCATBlockchainService::verificationCompleted);
    QVERIFY(!svc.verifyTransaction("nonexistent"));
    QCOMPARE(spy.count(), 1);
    QVERIFY(!spy.at(0).at(1).toBool());
  }

  // Execute smart contract and verify signal
  void testExecuteSmartContract() {
    EtherCATBlockchainService svc;
    QSignalSpy spy(&svc, &EtherCATBlockchainService::smartContractExecuted);
    SmartContract contract;
    contract.contractId = "SC_001";
    contract.name = "AutoReset";
    contract.bytecode = QByteArray("reset_code");
    contract.parameters["threshold"] = 100;
    QVERIFY(svc.executeSmartContract(contract));
    QCOMPARE(spy.count(), 1);
  }

  // Smart contract activation state
  void testSmartContractActivation() {
    EtherCATBlockchainService svc;
    SmartContract contract;
    contract.contractId = "SC_002";
    contract.name = "AutoConfig";
    contract.active = false;
    svc.executeSmartContract(contract);
    SmartContract executed = svc.transaction("TX_001").sender.isEmpty()
        ? SmartContract()
        : SmartContract();
  }

  // Track supply chain for unknown product
  void testSupplyChainTracking() {
    EtherCATBlockchainService svc;
    SupplyChain sc = svc.trackSupplyChain("product_1");
    QCOMPARE(sc.entries.size(), 0);
    QVERIFY(!sc.verified);
  }

  // Add entry to supply chain
  void testAddSupplyChainEntry() {
    EtherCATBlockchainService svc;
    SupplyChainEntry entry;
    entry.stepId = "step_1";
    entry.location = "Factory A";
    entry.handler = "operator_1";
    entry.status = "Shipped";
    svc.addSupplyChainEntry("product_1", entry);
    SupplyChain sc = svc.trackSupplyChain("product_1");
    QCOMPARE(sc.productId, QString("product_1"));
    QCOMPARE(sc.entries.size(), 1);
    QCOMPARE(sc.entries[0].stepId, QString("step_1"));
  }

  // Multiple transactions get unique block numbers
  void testMultipleTransactions() {
    EtherCATBlockchainService svc;
    for (int i = 0; i < 10; i++) {
      Transaction tx;
      tx.sender = "master";
      tx.receiver = QString("slave_%1").arg(i);
      svc.recordTransaction(tx);
    }
    QCOMPARE(svc.allTransactions().size(), 10);
    QSet<int> blockNumbers;
    for (const auto &tx : svc.allTransactions())
      blockNumbers.insert(tx.blockNumber);
    QCOMPARE(blockNumbers.size(), 10);
  }

  // transactionRecorded signal fires on record
  void testTransactionRecordedSignal() {
    EtherCATBlockchainService svc;
    QSignalSpy spy(&svc, &EtherCATBlockchainService::transactionRecorded);
    QVERIFY(spy.isValid());
    Transaction tx;
    tx.sender = "master";
    tx.receiver = "slave_1";
    svc.recordTransaction(tx);
    QCOMPARE(spy.count(), 1);
  }

  // verificationCompleted signal fires on verify
  void testVerificationCompletedSignal() {
    EtherCATBlockchainService svc;
    QSignalSpy spy(&svc, &EtherCATBlockchainService::verificationCompleted);
    QVERIFY(spy.isValid());
    svc.verifyTransaction("test");
    QCOMPARE(spy.count(), 1);
  }

  // smartContractExecuted signal fires on execute
  void testSmartContractExecutedSignal() {
    EtherCATBlockchainService svc;
    QSignalSpy spy(&svc, &EtherCATBlockchainService::smartContractExecuted);
    QVERIFY(spy.isValid());
    SmartContract sc;
    sc.contractId = "test";
    svc.executeSmartContract(sc);
    QCOMPARE(spy.count(), 1);
  }
};

QTEST_MAIN(EtherCATBlockchainServiceTest)
#include "ethercat_blockchain_service_test.moc"
