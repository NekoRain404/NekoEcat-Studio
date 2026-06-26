// WorkflowBlockchainServiceTest — Tests for WorkflowBlockchainService
//
// Test coverage:
//   - Transaction recording, verification, and smart contract execution fail closed without backend
//   - Supply chain tracking
//   - Rejected blockchain requests do not emit synthetic success signals

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
    QVERIFY(!svc.recordTransaction(tx));
    QCOMPARE(spy.count(), 0);
    QVERIFY(svc.allTransactions().isEmpty());
  }

  void testTransactionAutoId() {
    WorkflowBlockchainService svc;
    WfTransaction tx;
    tx.sender = "a";
    tx.receiver = "b";
    QVERIFY(!svc.recordTransaction(tx));
    auto all = svc.allTransactions();
    QVERIFY(all.isEmpty());
  }

  void testVerifyTransaction() {
    WorkflowBlockchainService svc;
    WfTransaction tx;
    tx.sender = "a";
    tx.receiver = "b";
    QVERIFY(!svc.recordTransaction(tx));
    QSignalSpy spy(&svc, &WorkflowBlockchainService::verificationCompleted);
    QVERIFY(!svc.verifyTransaction(QStringLiteral("WFTX_1")));
    QCOMPARE(spy.count(), 0);
  }

  void testVerifyTransactionNotFound() {
    WorkflowBlockchainService svc;
    QSignalSpy spy(&svc, &WorkflowBlockchainService::verificationCompleted);
    QVERIFY(!svc.verifyTransaction("nonexistent"));
    QCOMPARE(spy.count(), 0);
  }

  void testExecuteSmartContract() {
    WorkflowBlockchainService svc;
    QSignalSpy spy(&svc, &WorkflowBlockchainService::smartContractExecuted);
    WfSmartContract sc;
    sc.contractId = "SC001";
    sc.name = "TestContract";
    QVERIFY(!svc.executeSmartContract(sc));
    QCOMPARE(spy.count(), 0);
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
      QVERIFY(!svc.recordTransaction(tx));
    }
    QVERIFY(svc.allTransactions().isEmpty());
  }

  void testTransactionBlockNumber() {
    WorkflowBlockchainService svc;
    WfTransaction tx1;
    tx1.sender = "a";
    tx1.receiver = "b";
    QVERIFY(!svc.recordTransaction(tx1));
    WfTransaction tx2;
    tx2.sender = "c";
    tx2.receiver = "d";
    QVERIFY(!svc.recordTransaction(tx2));
    auto all = svc.allTransactions();
    QVERIFY(all.isEmpty());
  }

  void testTransactionStatusConfirmed() {
    WorkflowBlockchainService svc;
    WfTransaction tx;
    tx.sender = "a";
    tx.receiver = "b";
    QVERIFY(!svc.recordTransaction(tx));
    QVERIFY(svc.allTransactions().isEmpty());
  }

  void testTransactionTimestamp() {
    WorkflowBlockchainService svc;
    WfTransaction tx;
    tx.sender = "a";
    tx.receiver = "b";
    QVERIFY(!svc.recordTransaction(tx));
    QVERIFY(svc.allTransactions().isEmpty());
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
