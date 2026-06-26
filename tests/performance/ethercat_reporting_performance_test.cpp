#include <QTest>
#include <QElapsedTimer>
#include "services/EtherCATReportingService.h"
#include "services/EventBus.h"
#include "infra/EcatClient.h"

class EtherCATReportingPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testGenerateSystemReportThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATReportingService svc(&bus, &client);

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      Report r = svc.generateSystemReport();
      Q_UNUSED(r);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "System report throughput:" << count << "reports in" << elapsed << "ms";
  }

  void testGeneratePerformanceReportThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATReportingService svc(&bus, &client);

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      Report r = svc.generatePerformanceReport();
      Q_UNUSED(r);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "Performance report throughput:" << count << "reports in" << elapsed << "ms";
  }

  void testGenerateErrorReportThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATReportingService svc(&bus, &client);

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      Report r = svc.generateErrorReport();
      Q_UNUSED(r);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "Error report throughput:" << count << "reports in" << elapsed << "ms";
  }

  void testGenerateComplianceReportThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATReportingService svc(&bus, &client);

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      Report r = svc.generateComplianceReport();
      Q_UNUSED(r);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "Compliance report throughput:" << count << "reports in" << elapsed << "ms";
  }

  void testExportReportThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATReportingService svc(&bus, &client);

    Report r = svc.generateSystemReport();

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      QVERIFY(svc.exportReport(r, "text"));
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "Export report throughput:" << count << "exports in" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATReportingPerformanceTest)
#include "ethercat_reporting_performance_test.moc"
