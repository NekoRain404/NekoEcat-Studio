// EtherCATTestingServiceTest — Tests for EtherCATTestingService
//
// Test coverage:
//   - Unit, integration, performance, and stress test execution
//   - Test pass/fail/skip counts
//   - Suite name verification
//   - Duration and timing validation

#include <QTest>
#include "services/EtherCATTestingService.h"

class EtherCATTestingServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Run unit tests and verify all pass
  // Run unit tests and verify all pass
  void testRunUnitTests() {
    EtherCATTestingService svc;
    TestResults results = svc.runUnitTests();
    QVERIFY(results.total > 0);
    QCOMPARE(results.passed, results.total);
    QCOMPARE(results.failed, 0);
  }

  // Run integration tests and verify all pass
  // Run integration tests and verify all pass
  void testRunIntegrationTests() {
    EtherCATTestingService svc;
    TestResults results = svc.runIntegrationTests();
    QVERIFY(results.total > 0);
    QCOMPARE(results.passed, results.total);
    QCOMPARE(results.failed, 0);
  }

  // Run performance tests and verify all pass
  // Run performance tests and verify all pass
  void testRunPerformanceTests() {
    EtherCATTestingService svc;
    TestResults results = svc.runPerformanceTests();
    QVERIFY(results.total > 0);
    QCOMPARE(results.passed, results.total);
    QCOMPARE(results.failed, 0);
  }

  // Run stress tests and verify all pass
  // Run stress tests and verify all pass
  void testRunStressTests() {
    EtherCATTestingService svc;
    TestResults results = svc.runStressTests();
    QVERIFY(results.total > 0);
    QCOMPARE(results.passed, results.total);
    QCOMPARE(results.failed, 0);
  }

  // Verify unit test pass count matches total minus skipped
  // Unit tests: passed equals total minus skipped
  void testUnitTestsPassed() {
    EtherCATTestingService svc;
    TestResults results = svc.runUnitTests();
    QCOMPARE(results.passed, results.total - results.skipped);
    QCOMPARE(results.failed, 0);
  }

  // Verify integration test pass count matches total minus skipped
  // Integration tests: passed equals total minus skipped
  void testIntegrationTestsPassed() {
    EtherCATTestingService svc;
    TestResults results = svc.runIntegrationTests();
    QCOMPARE(results.passed, results.total - results.skipped);
    QCOMPARE(results.failed, 0);
  }

  // Verify unit test suite name
  // Unit test suite name is correct
  void testSuiteNameUnit() {
    EtherCATTestingService svc;
    TestResults results = svc.runUnitTests();
    QCOMPARE(results.suiteName, QString("Unit Tests"));
  }

  // Verify integration test suite name
  // Integration test suite name is correct
  void testSuiteNameIntegration() {
    EtherCATTestingService svc;
    TestResults results = svc.runIntegrationTests();
    QCOMPARE(results.suiteName, QString("Integration Tests"));
  }

  // Verify duration and timing are non-negative
  // Duration and timestamps are valid
  void testDurationNonNegative() {
    EtherCATTestingService svc;
    TestResults results = svc.runUnitTests();
    QVERIFY(results.durationMs >= 0);
    QVERIFY(results.startTimeMs > 0);
    QVERIFY(results.endTimeMs >= results.startTimeMs);
  }

  // Verify no failure entries in results
  // No test failures in results
  void testNoFailures() {
    EtherCATTestingService svc;
    TestResults results = svc.runUnitTests();
    QCOMPARE(results.failures.size(), 0);
  }
};

QTEST_MAIN(EtherCATTestingServiceTest)
#include "ethercat_testing_service_test.moc"
