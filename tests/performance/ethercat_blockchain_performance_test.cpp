#include <QTest>
#include <QElapsedTimer>
#include "services/EtherCATBlockchainService.h"

class EtherCATBlockchainPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testRecordTransactionThroughput() {
    EtherCATBlockchainService svc;
    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      Transaction tx;
      tx.sender = "master";
      tx.receiver = QString("slave_%1").arg(i);
      tx.data = QByteArray("data");
      svc.recordTransaction(tx);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 10000);
    QCOMPARE(svc.allTransactions().size(), count);
    qDebug() << "Blockchain record throughput:" << count << "transactions in" << elapsed << "ms";
  }

  void testVerifyTransactionThroughput() {
    EtherCATBlockchainService svc;
    for (int i = 0; i < 100; i++) {
      Transaction tx;
      tx.transactionId = QString("TX_%1").arg(i);
      tx.sender = "master";
      tx.receiver = "slave";
      svc.recordTransaction(tx);
    }

    QElapsedTimer timer;
    timer.start();

    const int count = 100;
    for (int i = 0; i < count; i++)
      svc.verifyTransaction(QString("TX_%1").arg(i));

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Blockchain verify throughput:" << count << "verifications in" << elapsed << "ms";
  }

  void testSmartContractThroughput() {
    EtherCATBlockchainService svc;
    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      SmartContract sc;
      sc.contractId = QString("SC_%1").arg(i);
      sc.name = QString("contract_%1").arg(i);
      svc.executeSmartContract(sc);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Blockchain contract throughput:" << count << "contracts in" << elapsed << "ms";
  }

  void testSupplyChainThroughput() {
    EtherCATBlockchainService svc;
    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      SupplyChainEntry entry;
      entry.stepId = QString("step_%1").arg(i);
      entry.location = "Factory";
      entry.handler = "operator";
      entry.status = "Done";
      svc.addSupplyChainEntry(QString("product_%1").arg(i % 100), entry);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Blockchain supply chain throughput:" << count << "entries in" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATBlockchainPerformanceTest)
#include "ethercat_blockchain_performance_test.moc"
