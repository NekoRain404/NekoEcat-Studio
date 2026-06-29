// WorkflowReportingServiceTest — Tests for Workflow Reporting Service
//
// Test coverage:
//   - Execution report generation
//   - Performance report generation
//   - Error report generation
//   - Resource report generation
//   - Report ID format validation
//   - Section and table content validation
//   - Execution reports do not synthesize successful completion

#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowReportingService.h"

class WorkflowReportingServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Generate execution report and verify title, summary, sections
  void testGenerateExecutionReport() {
    WorkflowReportingService svc;
    QSignalSpy spy(&svc, &WorkflowReportingService::reportGenerated);
    auto report = svc.generateExecutionReport("wf-001");
    QCOMPARE(report.title, "Execution Report");
    QVERIFY(!report.summary.isEmpty());
    QVERIFY(!report.sections.isEmpty());
    QVERIFY(!report.recommendations.isEmpty());
    QVERIFY(report.timestamp.isValid());
    QCOMPARE(spy.count(), 1);
  }

  // Generate performance report with charts and tables
  void testGeneratePerformanceReport() {
    WorkflowReportingService svc;
    QSignalSpy spy(&svc, &WorkflowReportingService::reportGenerated);
    auto report = svc.generatePerformanceReport("wf-001");
    QCOMPARE(report.title, "Performance Report");
    QVERIFY(!report.charts.isEmpty());
    QVERIFY(!report.tables.isEmpty());
    QCOMPARE(spy.count(), 1);
  }

  // Generate error report with error tables
  void testGenerateErrorReport() {
    WorkflowReportingService svc;
    QSignalSpy spy(&svc, &WorkflowReportingService::reportGenerated);
    auto report = svc.generateErrorReport("wf-001");
    QCOMPARE(report.title, "Error Report");
    QVERIFY(!report.tables.isEmpty());
    QCOMPARE(spy.count(), 1);
  }

  // Generate resource report with charts and tables
  void testGenerateResourceReport() {
    WorkflowReportingService svc;
    QSignalSpy spy(&svc, &WorkflowReportingService::reportGenerated);
    auto report = svc.generateResourceReport("wf-001");
    QCOMPARE(report.title, "Resource Report");
    QVERIFY(!report.charts.isEmpty());
    QVERIFY(!report.tables.isEmpty());
    QCOMPARE(spy.count(), 1);
  }

  // Verify report ID contains workflow ID and report type
  void testReportIdFormat() {
    WorkflowReportingService svc;
    auto report = svc.generateExecutionReport("wf-123");
    QVERIFY(report.id.contains("wf-123"));
    QVERIFY(report.id.contains("execution_report"));
  }

  // Verify all report sections have non-empty title and content
  void testReportSectionsHaveContent() {
    WorkflowReportingService svc;
    auto report = svc.generatePerformanceReport("wf-001");
    for (const auto &section : report.sections) {
      QVERIFY(!section.title.isEmpty());
      QVERIFY(!section.content.isEmpty());
    }
  }

  // Verify all report tables have headers and rows
  void testReportTablesHaveHeaders() {
    WorkflowReportingService svc;
    auto report = svc.generateResourceReport("wf-001");
    for (const auto &table : report.tables) {
      QVERIFY(!table.headers.isEmpty());
      QVERIFY(!table.rows.isEmpty());
    }
  }

  void executionReportDoesNotSynthesizeCompletion() {
    WorkflowReportingService svc;
    auto report = svc.generateExecutionReport("wf-001");
    QVERIFY2(!report.summary.contains("completed successfully", Qt::CaseInsensitive),
             "Execution reports must not claim successful completion without backend evidence.");
    for (const auto &section : report.sections) {
      QVERIFY2(!section.content.contains("completed successfully", Qt::CaseInsensitive),
               "Execution sections must not synthesize successful completion.");
      QVERIFY2(section.data.value("status").toString() != "completed",
               "Execution report status must not be hard-coded to completed.");
    }
    for (const auto &table : report.tables) {
      for (const auto &row : table.rows) {
        QVERIFY2(!row.contains("Completed"),
                 "Execution report tables must not synthesize Completed step rows.");
      }
    }
  }
};

QTEST_MAIN(WorkflowReportingServiceTest)
#include "workflow_reporting_service_test.moc"
