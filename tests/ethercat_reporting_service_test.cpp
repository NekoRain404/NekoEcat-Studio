// EtherCATReportingServiceTest — Tests for EtherCATReportingService
//
// Test coverage:
//   - Report generation (system, performance, error, compliance)
//   - Report export (text, HTML, empty report)
//   - Report metadata (timestamp, sections with content)
//   - Signal emission on report generation

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATReportingService.h"

class EtherCATReportingServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Generate system report and verify title, summary, and sections
  // Generate system report with sections and timestamp
  void testGenerateSystemReport() {
    EtherCATReportingService svc(nullptr, nullptr);
    auto report = svc.generateSystemReport();
    QCOMPARE(report.title, QStringLiteral("System Report"));
    QVERIFY(!report.summary.isEmpty());
    QVERIFY(report.sections.size() >= 2);
    QVERIFY(report.timestamp.isValid());
  }

  // Generate performance report with recommendations
  // Generate performance report with recommendations
  void testGeneratePerformanceReport() {
    EtherCATReportingService svc(nullptr, nullptr);
    auto report = svc.generatePerformanceReport();
    QCOMPARE(report.title, QStringLiteral("Performance Report"));
    QVERIFY(report.sections.size() >= 2);
    QVERIFY(!report.recommendations.isEmpty());
  }

  // Generate error report with sections
  // Generate error report
  void testGenerateErrorReport() {
    EtherCATReportingService svc(nullptr, nullptr);
    auto report = svc.generateErrorReport();
    QCOMPARE(report.title, QStringLiteral("Error Report"));
    QVERIFY(report.sections.size() >= 2);
  }

  // Generate compliance report with recommendations
  // Generate compliance report with recommendations
  void testGenerateComplianceReport() {
    EtherCATReportingService svc(nullptr, nullptr);
    auto report = svc.generateComplianceReport();
    QCOMPARE(report.title, QStringLiteral("Compliance Report"));
    QVERIFY(report.sections.size() >= 2);
    QVERIFY(!report.recommendations.isEmpty());
  }

  // Export report in text format
  // Export report as text
  void testExportReportText() {
    EtherCATReportingService svc(nullptr, nullptr);
    auto report = svc.generateSystemReport();
    bool ok = svc.exportReport(report, QStringLiteral("text"));
    QVERIFY(ok);
  }

  // Export report in HTML format
  // Export report as HTML
  void testExportReportHtml() {
    EtherCATReportingService svc(nullptr, nullptr);
    auto report = svc.generateSystemReport();
    bool ok = svc.exportReport(report, QStringLiteral("html"));
    QVERIFY(ok);
  }

  // Export empty report fails
  // Export empty report fails
  void testExportEmptyReport() {
    EtherCATReportingService svc(nullptr, nullptr);
    Report empty;
    bool ok = svc.exportReport(empty, QStringLiteral("text"));
    QVERIFY(!ok);
  }

  // Verify reportGenerated signal emission
  // reportGenerated signal fires on generation
  void testReportGeneratedSignal() {
    EtherCATReportingService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATReportingService::reportGenerated);
    svc.generateSystemReport();
    QCOMPARE(spy.count(), 1);
  }

  // Report timestamp is valid and not in the future
  // Report timestamp is not in the future
  void testReportTimestamp() {
    EtherCATReportingService svc(nullptr, nullptr);
    auto report = svc.generateSystemReport();
    QVERIFY(report.timestamp <= QDateTime::currentDateTime());
  }

  // All report sections have non-empty title and content
  // All sections have non-empty title and content
  void testReportSectionsHaveContent() {
    EtherCATReportingService svc(nullptr, nullptr);
    auto report = svc.generatePerformanceReport();
    for (const auto &section : report.sections) {
      QVERIFY(!section.title.isEmpty());
      QVERIFY(!section.content.isEmpty());
    }
  }
};

QTEST_MAIN(EtherCATReportingServiceTest)
#include "ethercat_reporting_service_test.moc"
