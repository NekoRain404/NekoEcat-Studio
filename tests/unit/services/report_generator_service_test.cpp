// ReportGeneratorServiceTest — Tests for Report Generator Service
//
// Test coverage:
//   - Default service state
//   - Report generation and section management
//   - Template configuration
//   - Export to text and HTML formats
//   - Signal validation
#include "services/ReportGeneratorService.h"
#include <QFile>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

class ReportGeneratorServiceTest : public QObject {
    Q_OBJECT
private slots:
    // Service initializes without errors
    void testDefaultState() { ReportGeneratorService svc; }

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
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("report.txt");
        QVERIFY(svc.exportReport(path, "text"));
        QFile file(path);
        QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
        const QString content = QString::fromUtf8(file.readAll());
        QVERIFY(content.contains("NekoEcat Diagnostic Report"));
        QVERIFY(content.contains("Section 1"));
        QVERIFY(content.contains("Content 1"));
    }

    // Export report in HTML format
    void testExportReportHtml() {
        ReportGeneratorService svc;
        svc.generateReport("Test");
        svc.addSection("Section 1", "Content 1");
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("report.html");
        QVERIFY(svc.exportReport(path, "html"));
        QFile file(path);
        QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
        const QString content = QString::fromUtf8(file.readAll());
        QVERIFY(content.contains("<html>"));
        QVERIFY(content.contains("Section 1"));
    }

    // Invalid export arguments fail explicitly.
    void testExportReportInvalidArguments() {
        ReportGeneratorService svc;
        svc.addSection("Section 1", "Content 1");
        QSignalSpy failedSpy(&svc, &ReportGeneratorService::reportFailed);
        QVERIFY(!svc.exportReport(QString(), "text"));
        QVERIFY(!svc.exportReport("/tmp/report.pdf", "pdf"));
        QCOMPARE(failedSpy.count(), 2);
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
