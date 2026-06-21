#include <QTest>
#include <QSignalSpy>
#include <QElapsedTimer>
#include <QTemporaryFile>
#include "services/DiagnosticReportService.h"
#include "services/EventBus.h"
#include "services/TopologyService.h"
#include "services/DcSyncService.h"
#include "services/PerformanceMonitorService.h"
#include "services/WatchdogService.h"
#include "infra/EcatClient.h"

class DiagnosticReportPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testGenerateReportThroughput() {
    EcatClient client;
    EventBus bus;
    TopologyService topology(&client);
    DcSyncService dcSync(&client);
    PerformanceMonitorService perfMon(&bus, &client);
    WatchdogService watchdog(&bus, &client);

    DiagnosticReportService svc(&bus, &client, &topology, &dcSync, &perfMon,
                                &watchdog);

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      svc.generateReport();
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "GenerateReport throughput:" << count << "in" << elapsed
             << "ms";
  }

  void testExportReportThroughput() {
    EcatClient client;
    EventBus bus;
    TopologyService topology(&client);
    DcSyncService dcSync(&client);
    PerformanceMonitorService perfMon(&bus, &client);
    WatchdogService watchdog(&bus, &client);

    DiagnosticReportService svc(&bus, &client, &topology, &dcSync, &perfMon,
                                &watchdog);

    QTemporaryFile tmp;
    QVERIFY(tmp.open());
    QString path = tmp.fileName();
    tmp.close();

    QElapsedTimer timer;
    timer.start();

    const int count = 100;
    for (int i = 0; i < count; i++) {
      svc.exportReport(path);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "ExportReport throughput:" << count << "in" << elapsed << "ms";
  }

  void testExportCsvThroughput() {
    EcatClient client;
    EventBus bus;
    TopologyService topology(&client);
    DcSyncService dcSync(&client);
    PerformanceMonitorService perfMon(&bus, &client);
    WatchdogService watchdog(&bus, &client);

    DiagnosticReportService svc(&bus, &client, &topology, &dcSync, &perfMon,
                                &watchdog);

    QTemporaryFile tmp;
    QVERIFY(tmp.open());
    QString path = tmp.fileName();
    tmp.close();

    QElapsedTimer timer;
    timer.start();

    const int count = 100;
    for (int i = 0; i < count; i++) {
      svc.exportReportCsv(path);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "ExportCsv throughput:" << count << "in" << elapsed << "ms";
  }

  void testMemoryStability() {
    for (int round = 0; round < 50; round++) {
      EcatClient client;
      EventBus bus;
      TopologyService topology(&client);
      DcSyncService dcSync(&client);
      PerformanceMonitorService perfMon(&bus, &client);
      WatchdogService watchdog(&bus, &client);

      DiagnosticReportService svc(&bus, &client, &topology, &dcSync, &perfMon,
                                  &watchdog);
      svc.generateReport();
    }
    qDebug() << "Memory stability: 50 report generation cycles completed";
  }
};

QTEST_MAIN(DiagnosticReportPerformanceTest)
#include "diagnostic_report_performance_test.moc"
