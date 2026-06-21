#pragma once

// EtherCATTestingService — automated testing for EtherCAT configurations,
// integration, performance, and stress scenarios.
//
// Runs test suites and emits results via signal.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QVector>
#include <QString>
#include <QDateTime>

struct TestFailure {
    QString testName;
    QString message;
    QString file;
    int line = 0;
    int severity = 0;
};

struct TestResults {
    int total = 0;
    int passed = 0;
    int failed = 0;
    int skipped = 0;
    qint64 durationMs = 0;
    qint64 startTimeMs = 0;
    qint64 endTimeMs = 0;
    QVector<TestFailure> failures;
    QString suiteName;
};

class EtherCATTestingService : public QObject {
    Q_OBJECT
public:
    explicit EtherCATTestingService(QObject *parent = nullptr);

    TestResults runUnitTests();
    TestResults runIntegrationTests();
    TestResults runPerformanceTests();
    TestResults runStressTests();

signals:
    void testCompleted(const TestResults &results);
    void testStarted(const QString &suiteName);

private:
    TestResults runTestSuite(const QString &name, int testCount);
};
