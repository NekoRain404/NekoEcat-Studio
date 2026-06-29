// WorkflowAnalyticsServiceTest — Tests for Workflow Analytics Service
//
// Test coverage:
//   - Execution recording and analysis (success rate, duration stats)
//   - Performance analysis (step duration, throughput)
//   - Error recording and analysis (error types, most common)
//   - Resource usage recording and analysis (CPU, memory, network)
//   - Empty workflow edge case
//   - Signal emissions

#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowAnalyticsService.h"

class WorkflowAnalyticsServiceTest : public QObject {
  Q_OBJECT
private slots:
  void initTestCase();
  void cleanupTestCase();
  // Record multiple executions and verify success rate and duration stats
  void recordAndAnalyzeExecution();
  // Record executions and verify performance metrics (throughput, p95)
  void recordAndAnalyzePerformance();
  // Record errors and verify error type grouping and most common error
  void recordAndAnalyzeErrors();
  // Record resource usage and verify CPU, memory, network averages
  void recordAndAnalyzeResources();
  // Analyze a workflow with no recorded data
  void emptyWorkflowAnalysis();
  // Verify signal emissions on recording events
  void signalEmissions();

private:
  WorkflowAnalyticsService *svc_ = nullptr;
};

void WorkflowAnalyticsServiceTest::initTestCase() {
  svc_ = new WorkflowAnalyticsService(this);
}

void WorkflowAnalyticsServiceTest::cleanupTestCase() {
  delete svc_;
  svc_ = nullptr;
}

void WorkflowAnalyticsServiceTest::recordAndAnalyzeExecution() {
  svc_->recordExecution("wf_exec", true, 100.0);
  svc_->recordExecution("wf_exec", true, 200.0);
  svc_->recordExecution("wf_exec", false, 300.0);

  auto analysis = svc_->analyzeExecution("wf_exec");
  QCOMPARE(analysis.workflowId, QString("wf_exec"));
  QCOMPARE(analysis.totalExecutions, 3);
  QCOMPARE(analysis.successfulExecutions, 2);
  QCOMPARE(analysis.failedExecutions, 1);
  QVERIFY(analysis.successRate > 60.0 && analysis.successRate < 70.0);
  QVERIFY(analysis.averageDurationMs > 190.0 && analysis.averageDurationMs < 210.0);
  QCOMPARE(analysis.minDurationMs, 100.0);
  QCOMPARE(analysis.maxDurationMs, 300.0);
}

void WorkflowAnalyticsServiceTest::recordAndAnalyzePerformance() {
  svc_->recordExecution("wf_perf", true, 50.0);
  svc_->recordExecution("wf_perf", true, 100.0);
  svc_->recordExecution("wf_perf", true, 150.0);

  auto analysis = svc_->analyzePerformance("wf_perf");
  QCOMPARE(analysis.workflowId, QString("wf_perf"));
  QVERIFY(analysis.averageStepDurationMs > 90.0 && analysis.averageStepDurationMs < 110.0);
  QVERIFY(analysis.p95StepDurationMs >= 150.0);
  QVERIFY(analysis.throughputPerSecond > 0.0);
}

void WorkflowAnalyticsServiceTest::recordAndAnalyzeErrors() {
  svc_->recordError("wf_err", "Timeout", "Connection timed out");
  svc_->recordError("wf_err", "Timeout", "Read timed out");
  svc_->recordError("wf_err", "Auth", "Authentication failed");

  auto analysis = svc_->analyzeErrors("wf_err");
  QCOMPARE(analysis.workflowId, QString("wf_err"));
  QCOMPARE(analysis.totalErrors, 3);
  QCOMPARE(analysis.uniqueErrorTypes, 2);
  QCOMPARE(analysis.mostCommonError, QString("Timeout"));
}

void WorkflowAnalyticsServiceTest::recordAndAnalyzeResources() {
  svc_->recordResourceUsage("wf_res", 50.0, 256.0, 100.0);
  svc_->recordResourceUsage("wf_res", 70.0, 512.0, 200.0);
  svc_->recordResourceUsage("wf_res", 90.0, 1024.0, 150.0);

  auto analysis = svc_->analyzeResources("wf_res");
  QCOMPARE(analysis.workflowId, QString("wf_res"));
  QVERIFY(analysis.averageCpuPercent > 65.0 && analysis.averageCpuPercent < 75.0);
  QCOMPARE(analysis.peakCpuPercent, 90.0);
  QVERIFY(analysis.averageMemoryMb > 590.0 && analysis.averageMemoryMb < 600.0);
  QCOMPARE(analysis.peakMemoryMb, 1024.0);
}

void WorkflowAnalyticsServiceTest::emptyWorkflowAnalysis() {
  auto execAnalysis = svc_->analyzeExecution("nonexistent");
  QCOMPARE(execAnalysis.totalExecutions, 0);

  auto perfAnalysis = svc_->analyzePerformance("nonexistent");
  QCOMPARE(perfAnalysis.averageStepDurationMs, 0.0);

  auto errAnalysis = svc_->analyzeErrors("nonexistent");
  QCOMPARE(errAnalysis.totalErrors, 0);

  auto resAnalysis = svc_->analyzeResources("nonexistent");
  QCOMPARE(resAnalysis.averageCpuPercent, 0.0);
}

void WorkflowAnalyticsServiceTest::signalEmissions() {
  QSignalSpy spy(svc_, &WorkflowAnalyticsService::analysisCompleted);

  svc_->recordExecution("wf_sig", true, 100.0);
  svc_->analyzeExecution("wf_sig");
  QCOMPARE(spy.count(), 1);

  svc_->analyzePerformance("wf_sig");
  QCOMPARE(spy.count(), 2);

  svc_->analyzeErrors("wf_sig");
  QCOMPARE(spy.count(), 3);

  svc_->analyzeResources("wf_sig");
  QCOMPARE(spy.count(), 4);
}

QTEST_MAIN(WorkflowAnalyticsServiceTest)
#include "workflow_analytics_service_test.moc"
