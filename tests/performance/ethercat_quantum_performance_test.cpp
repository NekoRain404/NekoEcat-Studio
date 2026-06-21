#include <QTest>
#include <QElapsedTimer>
#include "services/EtherCATQuantumService.h"

class EtherCATQuantumPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testKeyDistributionThroughput() {
    EtherCATQuantumService svc;
    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++)
      svc.distributeQuantumKeys(i);

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 10000);
    qDebug() << "Quantum key distribution throughput:" << count << "keys in" << elapsed << "ms";
  }

  void testRandomGenerationThroughput() {
    EtherCATQuantumService svc;
    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++)
      svc.generateQuantumRandom(100);

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Quantum random generation throughput:" << count << "batches in" << elapsed << "ms";
  }

  void testEncryptionThroughput() {
    EtherCATQuantumService svc;
    QByteArray plaintext("Performance test data payload");

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++)
      svc.encryptQuantumSafe(plaintext);

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Quantum encryption throughput:" << count << "encryptions in" << elapsed << "ms";
  }

  void testSigningThroughput() {
    EtherCATQuantumService svc;
    QByteArray data("Signing performance test data");

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++)
      svc.signQuantumResistant(data);

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Quantum signing throughput:" << count << "signatures in" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATQuantumPerformanceTest)
#include "ethercat_quantum_performance_test.moc"
