// WorkflowBlockchainServiceTest — Tests for WorkflowBlockchainService
//
// Test coverage:
//   - Transaction recording and verification
//   - Smart contract execution
//   - Supply chain tracking
//   - Signal emission for blockchain events

#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowBlockchainService.h"

class WorkflowBlockchainServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testRecordTransaction() {
    WorkflowBlockchainService svc;
    QSignalSpy spy(&svc, &WorkflowBlockchainService::transactionRecorded);
    WfTransaction tx;
    tx.sender = "node1";
    tx.receiver = "node2";
    tx.data = QByteArray("test data");
    QVERIFY(svc.recordTransaction(tx));
    QCOMPARE(spy.count(), 1);
  }

  void testTransactionAutoId() {
    WorkflowBlockchainService svc;
    WfTransaction tx;
    tx.sender = "a";
    tx.receiver = "b";
    svc.recordTransaction(tx);
    auto all = svc.allTransactions();
    QCOMPARE(all.size(), 1);
    QVERIFY(!all.first().transactionId.isEmpty());
  }

  void testVerifyTransaction() {
    WorkflowBlockchainService svc;
    WfTransaction tx;
    tx.sender = "a";
    tx.receiver = "b";
    svc.recordTransaction(tx);
    auto recorded = svc.allTransactions().first();
    QSignalSpy spy(&svc, &WorkflowBlockchainService::verificationCompleted);
    QVERIFY(svc.verifyTransaction(recorded.transactionId));
    QCOMPARE(spy.count(), 1);
    QVERIFY(spy.at(0).at(1).toBool());
  }

  void testVerifyTransactionNotFound() {
    WorkflowBlockchainService svc;
    QSignalSpy spy(&svc, &WorkflowBlockchainService::verificationCompleted);
    QVERIFY(!svc.verifyTransaction("nonexistent"));
    QCOMPARE(spy.count(), 1);
    QVERIFY(!spy.at(0).at(1).toBool());
  }

  void testExecuteSmartContract() {
    WorkflowBlockchainService svc;
    QSignalSpy spy(&svc, &WorkflowBlockchainService::smartContractExecuted);
    WfSmartContract sc;
    sc.contractId = "SC001";
    sc.name = "TestContract";
    QVERIFY(svc.executeSmartContract(sc));
    QCOMPARE(spy.count(), 1);
  }

  void testTrackSupplyChain() {
    WorkflowBlockchainService svc;
    WfSupplyChainEntry entry;
    entry.stepId = "step1";
    entry.location = "Factory";
    entry.handler = "handler1";
    entry.status = "completed";
    svc.addSupplyChainEntry("PROD001", entry);
    WfSupplyChain chain = svc.trackSupplyChain("PROD001");
    QCOMPARE(chain.productId, QString("PROD001"));
    QCOMPARE(chain.entries.size(), 1);
  }

  void testTrackSupplyChainNotFound() {
    WorkflowBlockchainService svc;
    WfSupplyChain chain = svc.trackSupplyChain("nonexistent");
    QVERIFY(chain.productId.isEmpty());
    QCOMPARE(chain.entries.size(), 0);
  }

  void testMultipleTransactions() {
    WorkflowBlockchainService svc;
    for (int i = 0; i < 5; i++) {
      WfTransaction tx;
      tx.sender = QString("sender_%1").arg(i);
      tx.receiver = QString("receiver_%1").arg(i);
      svc.recordTransaction(tx);
    }
    QCOMPARE(svc.allTransactions().size(), 5);
  }

  void testTransactionBlockNumber() {
    WorkflowBlockchainService svc;
    WfTransaction tx1;
    tx1.sender = "a";
    tx1.receiver = "b";
    svc.recordTransaction(tx1);
    WfTransaction tx2;
    tx2.sender = "c";
    tx2.receiver = "d";
    svc.recordTransaction(tx2);
    auto all = svc.allTransactions();
    QCOMPARE(all.size(), 2);
    QVERIFY(all[0].blockNumber != all[1].blockNumber);
    QVERIFY(all[0].blockNumber > 0);
    QVERIFY(all[1].blockNumber > 0);
  }

  void testTransactionStatusConfirmed() {
    WorkflowBlockchainService svc;
    WfTransaction tx;
    tx.sender = "a";
    tx.receiver = "b";
    svc.recordTransaction(tx);
    auto recorded = svc.allTransactions().first();
    QCOMPARE(recorded.status, WfTransactionStatus::Confirmed);
  }

  void testTransactionTimestamp() {
    WorkflowBlockchainService svc;
    WfTransaction tx;
    tx.sender = "a";
    tx.receiver = "b";
    svc.recordTransaction(tx);
    auto recorded = svc.allTransactions().first();
    QVERIFY(recorded.timestamp.isValid());
  }

  void testMultipleSupplyChainEntries() {
    WorkflowBlockchainService svc;
    for (int i = 0; i < 3; i++) {
      WfSupplyChainEntry entry;
      entry.stepId = QString("step_%1").arg(i);
      entry.location = QString("location_%1").arg(i);
      svc.addSupplyChainEntry("PROD001", entry);
    }
    WfSupplyChain chain = svc.trackSupplyChain("PROD001");
    QCOMPARE(chain.entries.size(), 3);
  }
};

QTEST_MAIN(WorkflowBlockchainServiceTest)
#include "workflow_blockchain_service_test.moc"
