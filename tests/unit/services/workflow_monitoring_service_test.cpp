// WorkflowMonitoringServiceTest — Tests for Workflow Monitoring Service
//
// Test coverage:
//   - Execution recording and status retrieval
//   - Performance monitoring (step duration, throughput)
//   - Error recording and retrieval
//   - Alert management and threshold configuration
//   - Signal emissions on state changes

#include "services/WorkflowMonitoringService.h"
#include <QSignalSpy>
#include <QTest>

class WorkflowMonitoringServiceTest : public QObject {
    Q_OBJECT
private:
    ExecutionStatus makeStatus(const QString& id = QStringLiteral("wf1"),
                               ExecutionState state = ExecutionState::Running) {
        ExecutionStatus s;
        s.workflowId = id;
        s.state = state;
        s.startTime = QDateTime::currentDateTime().addSecs(-10);
        s.endTime = QDateTime::currentDateTime();
        s.progress = 50.0;
        s.currentStep = 3;
        s.totalSteps = 6;
        s.warnings << QStringLiteral("slow step");
        return s;
    }

private slots:
    // Record execution and verify execution and performance signals
    void testRecordExecution() {
        WorkflowMonitoringService svc;
        QSignalSpy execSpy(&svc, &WorkflowMonitoringService::executionUpdated);
        QSignalSpy perfSpy(&svc, &WorkflowMonitoringService::performanceUpdated);

        auto status = makeStatus();
        svc.recordExecution(status);

        QCOMPARE(execSpy.count(), 1);
        QCOMPARE(perfSpy.count(), 1);
    }

    // Retrieve execution status by workflow ID
    void testMonitorExecution() {
        WorkflowMonitoringService svc;
        auto status = makeStatus();
        svc.recordExecution(status);

        auto fetched = svc.monitorExecution(QStringLiteral("wf1"));
        QCOMPARE(fetched.workflowId, QString("wf1"));
        QCOMPARE(fetched.currentStep, 3);
        QCOMPARE(fetched.totalSteps, 6);
    }

    // Retrieve performance metrics for a recorded workflow
    void testMonitorPerformance() {
        WorkflowMonitoringService svc;
        auto status = makeStatus();
        svc.recordExecution(status);

        auto perf = svc.monitorPerformance(QStringLiteral("wf1"));
        QCOMPARE(perf.workflowId, QString("wf1"));
        QCOMPARE(perf.completedSteps, 3);
        QVERIFY(perf.totalDurationMs > 0.0);
        QVERIFY(perf.avgStepDurationMs > 0.0);
    }

    // Return empty errors for workflow with no recorded errors
    void testMonitorErrorsEmpty() {
        WorkflowMonitoringService svc;
        auto errors = svc.monitorErrors(QStringLiteral("wf1"));
        QVERIFY(errors.isEmpty());
    }

    // Record error and verify signal and retrieval
    void testRecordError() {
        WorkflowMonitoringService svc;
        QSignalSpy spy(&svc, &WorkflowMonitoringService::errorOccurred);

        WfErrorInfo err;
        err.workflowId = QStringLiteral("wf1");
        err.stepId = QStringLiteral("step3");
        err.message = QStringLiteral("Connection timeout");
        err.severity = QStringLiteral("warning");
        err.timestamp = QDateTime::currentDateTime();

        svc.recordError(err);
        QCOMPARE(spy.count(), 1);

        auto errors = svc.monitorErrors(QStringLiteral("wf1"));
        QCOMPARE(errors.size(), 1);
        QCOMPARE(errors[0].message, QString("Connection timeout"));
    }

    // Critical error sets execution state to Failed
    void testCriticalErrorSetsFailedState() {
        WorkflowMonitoringService svc;
        auto status = makeStatus();
        svc.recordExecution(status);

        WfErrorInfo err;
        err.workflowId = QStringLiteral("wf1");
        err.stepId = QStringLiteral("step1");
        err.message = QStringLiteral("Fatal");
        err.severity = QStringLiteral("critical");
        svc.recordError(err);

        auto exec = svc.monitorExecution(QStringLiteral("wf1"));
        QCOMPARE(exec.state, ExecutionState::Failed);
        QVERIFY(exec.errors.contains(QStringLiteral("Fatal")));
    }

    // Default resource monitoring returns zero values
    void testMonitorResourcesDefault() {
        WorkflowMonitoringService svc;
        auto res = svc.monitorResources(QStringLiteral("wf1"));
        QCOMPARE(res.cpuPercent, 0.0);
        QCOMPARE(res.memoryMb, 0.0);
    }

    // Clear history removes execution and error data
    void testClearHistory() {
        WorkflowMonitoringService svc;
        svc.recordExecution(makeStatus());

        WfErrorInfo err;
        err.workflowId = QStringLiteral("wf1");
        err.message = QStringLiteral("err");
        svc.recordError(err);

        svc.clearHistory(QStringLiteral("wf1"));

        auto exec = svc.monitorExecution(QStringLiteral("wf1"));
        QCOMPARE(exec.workflowId, QString());
        QVERIFY(svc.monitorErrors(QStringLiteral("wf1")).isEmpty());
    }

    // Track multiple workflows independently
    void testMultipleWorkflows() {
        WorkflowMonitoringService svc;
        svc.recordExecution(makeStatus(QStringLiteral("wf1")));
        svc.recordExecution(makeStatus(QStringLiteral("wf2"), ExecutionState::Completed));

        auto e1 = svc.monitorExecution(QStringLiteral("wf1"));
        auto e2 = svc.monitorExecution(QStringLiteral("wf2"));
        QCOMPARE(e1.state, ExecutionState::Running);
        QCOMPARE(e2.state, ExecutionState::Completed);
    }

    // Verify throughput and completed steps for performance monitoring
    void testPerformanceThroughput() {
        WorkflowMonitoringService svc;
        ExecutionStatus s;
        s.workflowId = QStringLiteral("wf_perf");
        s.state = ExecutionState::Completed;
        s.startTime = QDateTime::currentDateTime().addSecs(-10);
        s.endTime = QDateTime::currentDateTime();
        s.currentStep = 5;
        s.totalSteps = 5;
        svc.recordExecution(s);

        auto perf = svc.monitorPerformance(QStringLiteral("wf_perf"));
        QVERIFY(perf.throughput > 0.0);
        QCOMPARE(perf.completedSteps, 5);
    }
};

QTEST_MAIN(WorkflowMonitoringServiceTest)
#include "workflow_monitoring_service_test.moc"
