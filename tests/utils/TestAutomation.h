#pragma once

#include <QDateTime>
#include <QString>
#include <QVector>

struct Failure {
    QString testName;
    QString message;
    QString file;
    int line = 0;
};

struct TestResults {
    int total = 0;
    int passed = 0;
    int failed = 0;
    int skipped = 0;
    qint64 duration = 0;
    QDateTime startTime;
    QDateTime endTime;
    QVector<Failure> failures;
};

struct CoverageReport {
    double lineCoverage = 0.0;
    double branchCoverage = 0.0;
    double functionCoverage = 0.0;
    double overallCoverage = 0.0;
    QVector<int> uncoveredLines;
};

class TestAutomation {
public:
    static QStringList discoverTests(const QString &directory);
    static TestResults executeTests(const QStringList &tests, int parallel = 1);
    static QString generateReport(const TestResults &results);
    static CoverageReport calculateCoverage(const TestResults &results);
};
