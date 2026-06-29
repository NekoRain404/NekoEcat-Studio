// DiagnosticReportServiceTest — Tests for DiagnosticReportService
//
// Test coverage:
//   - Full report generation with all sections
//   - Topology section content
//   - Report generated signal emission
//   - Export report to file
//   - Export report to CSV

// DiagnosticReportServiceTest — Tests for DiagnosticReportService
//
// Test coverage:
//   - Report generation with all sections
//   - Topology section content
//   - Report generated signal emission
//   - Export report to file (Markdown and CSV)

#include <QTest>
#include <QSignalSpy>
#include <QJsonObject>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include "services/DiagnosticReportService.h"
#include "services/EventBus.h"
#include "services/TopologyService.h"
#include "services/DcSyncService.h"
#include "services/PerformanceMonitorService.h"
#include "services/WatchdogService.h"
#include "infra/EcatClient.h"

class DiagnosticReportServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Verify report contains all expected sections
  void testGenerateReport() {
    EcatClient client;
    EventBus bus;
    TopologyService topology(&client);
    DcSyncService dcSync(&client);
    PerformanceMonitorService perfMon(&bus, &client);
    WatchdogService watchdog(&bus, &client);

    DiagnosticReportService svc(&bus, &client, &topology, &dcSync, &perfMon,
                                &watchdog);

    QString report = svc.generateReport();
    QVERIFY(report.contains("# EtherCAT Diagnostic Report"));
    QVERIFY(report.contains("Bus Topology"));
    QVERIFY(report.contains("Slave Status Overview"));
    QVERIFY(report.contains("Performance Metrics"));
    QVERIFY(report.contains("DC Sync Status"));
    QVERIFY(report.contains("Watchdog Status"));
  }

  // Verify report topology and slave status sections
  // Verify report contains topology and slave status sections
  void testReportContainsTopology() {
    EcatClient client;
    EventBus bus;
    TopologyService topology(&client);
    DcSyncService dcSync(&client);
    PerformanceMonitorService perfMon(&bus, &client);
    WatchdogService watchdog(&bus, &client);

    DiagnosticReportService svc(&bus, &client, &topology, &dcSync, &perfMon,
                                &watchdog);
    QString report = svc.generateReport();
    QVERIFY(report.contains("## Bus Topology"));
    QVERIFY(report.contains("## Slave Status Overview"));
    QVERIFY(report.contains("No slaves discovered"));
  }

  // Verify reportGenerated signal is emitted
  // Verify reportGenerated signal is emitted on report generation
  void testReportGeneratedSignal() {
    EcatClient client;
    EventBus bus;
    TopologyService topology(&client);
    DcSyncService dcSync(&client);
    PerformanceMonitorService perfMon(&bus, &client);
    WatchdogService watchdog(&bus, &client);

    DiagnosticReportService svc(&bus, &client, &topology, &dcSync, &perfMon,
                                &watchdog);
    QSignalSpy spy(&svc, &DiagnosticReportService::reportGenerated);
    QVERIFY(spy.isValid());

    svc.generateReport();
    QCOMPARE(spy.count(), 1);
  }

  // Verify export writes report to file
  // Verify export writes Markdown report to file
  void testExportReport() {
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

    QVERIFY(svc.exportReport(path));

    QFile f(path);
    QVERIFY(f.open(QIODevice::ReadOnly | QIODevice::Text));
    QString content = f.readAll();
    QVERIFY(content.contains("# EtherCAT Diagnostic Report"));
  }

  // Verify CSV export contains header row
  // Verify export writes CSV report with headers
  void testExportCsv() {
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

    QVERIFY(svc.exportReportCsv(path));

    QFile f(path);
    QVERIFY(f.open(QIODevice::ReadOnly | QIODevice::Text));
    QString content = f.readAll();
    QVERIFY(content.contains("Section,Metric,Value"));
  }

  void testExportRejectsInvalidPaths() {
    EcatClient client;
    EventBus bus;
    TopologyService topology(&client);
    DcSyncService dcSync(&client);
    PerformanceMonitorService perfMon(&bus, &client);
    WatchdogService watchdog(&bus, &client);

    DiagnosticReportService svc(&bus, &client, &topology, &dcSync, &perfMon,
                                &watchdog);

    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    QVERIFY(!svc.exportReport(QString()));
    QVERIFY(!svc.exportReportCsv(QString()));
    QVERIFY(!svc.exportReport(dir.path()));
    QVERIFY(!svc.exportReportCsv(dir.path()));
  }
};

QTEST_MAIN(DiagnosticReportServiceTest)
#include "diagnostic_report_service_test.moc"
