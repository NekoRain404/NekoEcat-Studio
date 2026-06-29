// TestAutomationTest — Tests for TestAutomation utility
//
// Test coverage:
//   - Report generation with empty and failure results
//   - Coverage calculation
//   - Test discovery with invalid paths

#include <QTest>
#include <QSignalSpy>
#include "utils/TestAutomation.h"

class TestAutomationTest : public QObject {
    Q_OBJECT
private slots:
    // Report shows zero totals for empty results
    void testGenerateReportEmpty() {
        TestResults results;
        results.startTime = QDateTime::currentDateTime();
        results.endTime = results.startTime;
        QString report = TestAutomation::generateReport(results);
        QVERIFY(report.contains("Total:   0"));
        QVERIFY(report.contains("Passed:  0"));
        QVERIFY(report.contains("Failed:  0"));
    }

    // Report includes failure details with file and line info
    void testGenerateReportWithFailures() {
        TestResults results;
        results.total = 10;
        results.passed = 8;
        results.failed = 2;
        results.startTime = QDateTime::currentDateTime();
        results.endTime = results.startTime;

        Failure f1;
        f1.testName = "testFoo";
        f1.message = "Assertion failed";
        f1.file = "test_foo.cpp";
        f1.line = 42;
        results.failures.append(f1);

        QString report = TestAutomation::generateReport(results);
        QVERIFY(report.contains("Total:   10"));
        QVERIFY(report.contains("Passed:  8"));
        QVERIFY(report.contains("Failed:  2"));
        QVERIFY(report.contains("testFoo"));
        QVERIFY(report.contains("test_foo.cpp:42"));
    }

    // Coverage percentages derived from pass/fail ratio
    void testCalculateCoverage() {
        TestResults results;
        results.total = 10;
        results.passed = 8;
        results.failed = 2;

        CoverageReport cov = TestAutomation::calculateCoverage(results);
        QCOMPARE(cov.lineCoverage, 80.0);
        QVERIFY(cov.branchCoverage > 0.0);
        QVERIFY(cov.functionCoverage > 0.0);
        QVERIFY(cov.overallCoverage > 0.0);
    }

    // Zero tests yields zero coverage
    void testCalculateCoverageZeroTests() {
        TestResults results;
        CoverageReport cov = TestAutomation::calculateCoverage(results);
        QCOMPARE(cov.lineCoverage, 0.0);
    }

    // Invalid directory returns empty list
    void testDiscoverTestsInvalidDir() {
        QStringList tests = TestAutomation::discoverTests("/nonexistent/path");
        QCOMPARE(tests.size(), 0);
    }
};

QTEST_MAIN(TestAutomationTest)
#include "test_automation_test.moc"
