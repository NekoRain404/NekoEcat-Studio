#include "EtherCATTestingService.h"
#include <QDateTime>

// EtherCATTestingService.cpp — Test suite execution for unit, integration, performance, and stress tests
//
// Implementation notes:
//   - All test types delegate to runTestSuite with configurable test count
//   - Simulates 100% pass rate with elapsed time tracking
//   - Emits testStarted and testCompleted signals around suite execution

EtherCATTestingService::EtherCATTestingService(QObject *parent)
    : QObject(parent)
{
}

TestResults EtherCATTestingService::runUnitTests()
{
    return runTestSuite(QStringLiteral("Unit Tests"), 10);
}

TestResults EtherCATTestingService::runIntegrationTests()
{
    return runTestSuite(QStringLiteral("Integration Tests"), 8);
}

TestResults EtherCATTestingService::runPerformanceTests()
{
    return runTestSuite(QStringLiteral("Performance Tests"), 6);
}

TestResults EtherCATTestingService::runStressTests()
{
    return runTestSuite(QStringLiteral("Stress Tests"), 5);
}

TestResults EtherCATTestingService::runTestSuite(const QString &name, int testCount)
{
    emit testStarted(name);

    TestResults results;
    results.suiteName = name;
    results.startTimeMs = QDateTime::currentMSecsSinceEpoch();

    results.total = testCount;
    results.passed = testCount;
    results.failed = 0;
    results.skipped = 0;

    results.endTimeMs = QDateTime::currentMSecsSinceEpoch();
    results.durationMs = results.endTimeMs - results.startTimeMs;

    emit testCompleted(results);
    return results;
}
