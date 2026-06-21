// ReportGeneratorServiceTest — Tests for Report Generator Service
//
// Test coverage:
//   - Default service state
//   - Report generation and section management
//   - Template configuration
//   - Export to text and HTML formats
//   - Signal validation
#include <QTest>
#include <QSignalSpy>
#include "services/ReportGeneratorService.h"

class ReportGeneratorServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Service initializes without errors
  void testDefaultState() {
    ReportGeneratorService svc;
  }

  // Generate a named report
  void testGenerateReport() {
    ReportGeneratorService svc;
    svc.generateReport("Test Report");
  }

  // Add multiple sections to a report
  void testAddSection() {
    ReportGeneratorService svc;
    svc.generateReport("Test");
    svc.addSection("Introduction", "This is the intro.");
    svc.addSection("Methods", "These are the methods.");
  }

  // Set report template
  void testSetTemplate() {
    ReportGeneratorService svc;
    svc.setTemplate("default");
  }

  // Export report in text format
  void testExportReportText() {
    ReportGeneratorService svc;
    svc.generateReport("Test");
    svc.addSection("Section 1", "Content 1");
    svc.exportReport("/tmp/report.txt", "text");
  }

  // Export report in HTML format
  void testExportReportHtml() {
    ReportGeneratorService svc;
    svc.generateReport("Test");
    svc.addSection("Section 1", "Content 1");
    svc.exportReport("/tmp/report.html", "html");
  }

  // Verify report generation and failure signals are valid
  void testSignals() {
    ReportGeneratorService svc;
    QSignalSpy generatedSpy(&svc, &ReportGeneratorService::reportGenerated);
    QSignalSpy failedSpy(&svc, &ReportGeneratorService::reportFailed);
    QVERIFY(generatedSpy.isValid());
    QVERIFY(failedSpy.isValid());
  }
};

QTEST_MAIN(ReportGeneratorServiceTest)
#include "report_generator_service_test.moc"
