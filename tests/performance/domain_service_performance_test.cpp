#include <QTest>
#include <QSignalSpy>
#include <QElapsedTimer>
#include "services/DomainService.h"

class DomainServicePerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testDomainCreationThroughput() {
    DomainService svc;
    QElapsedTimer timer;
    timer.start();

    const int count = 10000;
    for (int i = 0; i < count; i++) {
      svc.createDomain();
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 500);
    qDebug() << "Domain creation throughput:" << count << "domains in" << elapsed << "ms";
  }

  void testPdoEntryRegistrationThroughput() {
    DomainService svc;

    QVector<int> domainIds;
    for (int d = 0; d < 100; d++) {
      domainIds.append(svc.createDomain());
    }

    QElapsedTimer timer;
    timer.start();

    const int entriesPerDomain = 1000;
    for (int d = 0; d < domainIds.size(); d++) {
      for (int e = 0; e < entriesPerDomain; e++) {
        svc.registerPdoEntry(domainIds[d], e % 100, 0x6000 + e, 0);
      }
    }

    qint64 elapsed = timer.elapsed();
    int total = domainIds.size() * entriesPerDomain;
    QVERIFY(elapsed < 1000);
    qDebug() << "PDO entry registration throughput:" << total << "entries in" << elapsed << "ms";
  }

  void testDomainProcessingThroughput() {
    DomainService svc;

    QVector<int> domainIds;
    for (int i = 0; i < 10000; i++) {
      int d = svc.createDomain();
      svc.registerPdoEntry(d, 0, 0x6000, 0);
      domainIds.append(d);
    }

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < domainIds.size(); i++) {
      svc.processDomain(domainIds[i]);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "Domain processing throughput:" << domainIds.size() << "domains in" << elapsed << "ms";
  }

  void testDomainDataQueryLatency() {
    DomainService svc;
    int d = svc.createDomain();
    svc.registerPdoEntry(d, 0, 0x6000, 0);
    svc.processDomain(d);

    QElapsedTimer timer;
    timer.start();

    const int count = 100000;
    volatile int sink = 0;
    for (int i = 0; i < count; i++) {
      QByteArray data = svc.domainData(d);
      sink = data.size();
    }

    qint64 elapsed = timer.elapsed();
    Q_UNUSED(sink);
    QVERIFY(elapsed < 500);
    qDebug() << "Domain data query latency:" << count << "domainData() in" << elapsed << "ms";
  }

  void testDomainsListThroughput() {
    DomainService svc;

    for (int i = 0; i < 1000; i++) {
      svc.createDomain();
    }

    QElapsedTimer timer;
    timer.start();

    const int count = 10000;
    volatile int sink = 0;
    for (int i = 0; i < count; i++) {
      QVector<int> list = svc.domains();
      sink = list.size();
    }

    qint64 elapsed = timer.elapsed();
    Q_UNUSED(sink);
    QVERIFY(elapsed < 1000);
    qDebug() << "Domains list throughput:" << count << "domains() with 1000 entries in" << elapsed << "ms";
  }
};

QTEST_MAIN(DomainServicePerformanceTest)
#include "domain_service_performance_test.moc"
