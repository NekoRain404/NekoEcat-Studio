// ProjectTrackingServiceTest — Tests for ProjectTrackingService
//
// Test coverage:
//   - Progress tracking with task counts and status
//   - Time logging and time report generation
//   - Cost logging and cost report generation
//   - Quality metric tracking and overall score
//   - Task completion updates with signal verification
//   - Nonexistent project error handling
//   - Duplicate project rejection
//   - Project count and listing
//   - Completed project status detection
//   - Over-budget cost handling

#include <QTest>
#include <QSignalSpy>
#include "services/ProjectTrackingService.h"

class ProjectTrackingServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Test progress tracking with task counts and status
  void testTrackProgress() {
    ProjectTrackingService svc;

    ProjectTrackData data;
    data.projectId = 1;
    data.completedTasks = 3;
    data.totalTasks = 10;
    data.budgetedHours = 100;
    data.budgetedCost = 5000;
    svc.addProject(data);

    ProgressStatus status = svc.trackProgress(1);
    QCOMPARE(status.projectId, 1);
    QCOMPARE(status.completedTasks, 3);
    QCOMPARE(status.totalTasks, 10);
    QCOMPARE(status.progress, 30);
    QVERIFY(status.percentage > 29.0 && status.percentage < 31.0);
    QCOMPARE(status.status, ProjectTrackStatus::Behind);
  }

  void testRejectInvalidProjectTrackData() {
    ProjectTrackingService svc;

    ProjectTrackData data;
    data.projectId = 0;
    data.totalTasks = 10;
    QVERIFY(!svc.addProject(data));

    data.projectId = 1;
    data.totalTasks = -1;
    QVERIFY(!svc.addProject(data));

    data.totalTasks = 10;
    data.completedTasks = -1;
    QVERIFY(!svc.addProject(data));

    data.completedTasks = 11;
    QVERIFY(!svc.addProject(data));

    data.completedTasks = 0;
    data.budgetedHours = -1.0;
    QVERIFY(!svc.addProject(data));

    data.budgetedHours = 0.0;
    data.budgetedCost = -1.0;
    QVERIFY(!svc.addProject(data));

    QCOMPARE(svc.projectCount(), 0);
  }

  // Log time entry and verify time report totals
  // Test time tracking with log entries
  void testTrackTime() {
    ProjectTrackingService svc;

    ProjectTrackData data;
    data.projectId = 1;
    data.budgetedHours = 100;
    svc.addProject(data);

    TimeEntry entry;
    entry.taskId = 1;
    entry.description = "Development";
    entry.hours = 8.0;
    entry.assignee = "alice";
    svc.logTime(1, entry);

    TimeReport report = svc.trackTime(1);
    QCOMPARE(report.projectId, 1);
    QCOMPARE(report.totalHours, 8.0);
    QCOMPARE(report.budgetedHours, 100.0);
    QCOMPARE(report.remainingHours, 92.0);
    QCOMPARE(report.entries.size(), 1);
    QVERIFY(report.generatedAt.isValid());
  }

  void testRejectInvalidTimeEntries() {
    ProjectTrackingService svc;

    ProjectTrackData data;
    data.projectId = 1;
    svc.addProject(data);

    TimeEntry entry;
    entry.taskId = 1;
    entry.hours = -1.0;
    QVERIFY(!svc.logTime(1, entry));

    TimeReport report = svc.trackTime(1);
    QCOMPARE(report.entries.size(), 0);
    QCOMPARE(report.totalHours, 0.0);
  }

  // Log cost entry and verify cost report totals
  // Test cost tracking with log entries
  void testTrackCost() {
    ProjectTrackingService svc;

    ProjectTrackData data;
    data.projectId = 1;
    data.budgetedCost = 5000;
    svc.addProject(data);

    CostEntry entry;
    entry.category = "Hardware";
    entry.amount = 1200.0;
    entry.description = "Cables";
    svc.logCost(1, entry);

    CostReport report = svc.trackCost(1);
    QCOMPARE(report.projectId, 1);
    QCOMPARE(report.totalCost, 1200.0);
    QCOMPARE(report.budgetedCost, 5000.0);
    QCOMPARE(report.remainingBudget, 3800.0);
    QCOMPARE(report.entries.size(), 1);
  }

  void testRejectInvalidCostEntries() {
    ProjectTrackingService svc;

    ProjectTrackData data;
    data.projectId = 1;
    svc.addProject(data);

    CostEntry entry;
    entry.category = "Hardware";
    entry.amount = -1.0;
    QVERIFY(!svc.logCost(1, entry));

    CostReport report = svc.trackCost(1);
    QCOMPARE(report.entries.size(), 0);
    QCOMPARE(report.totalCost, 0.0);
  }

  // Update quality metrics and verify report metrics and overall score
  // Test quality metric tracking and overall score
  void testTrackQuality() {
    ProjectTrackingService svc;

    ProjectTrackData data;
    data.projectId = 1;
    svc.addProject(data);

    QualityMetric m1;
    m1.name = "Code Coverage";
    m1.value = 85.0;
    m1.target = 80.0;
    svc.updateQualityMetric(1, m1);

    QualityMetric m2;
    m2.name = "Bug Rate";
    m2.value = 2.0;
    m2.target = 5.0;
    svc.updateQualityMetric(1, m2);

    QualityReport report = svc.trackQuality(1);
    QCOMPARE(report.projectId, 1);
    QCOMPARE(report.metrics.size(), 2);
    QVERIFY(report.overallScore > 0.0);
    QVERIFY(report.generatedAt.isValid());
  }

  // Update task completion and verify signal and progress values
  // Test task completion update with signal
  void testUpdateTaskCompletion() {
    ProjectTrackingService svc;

    ProjectTrackData data;
    data.projectId = 1;
    data.totalTasks = 10;
    data.completedTasks = 0;
    svc.addProject(data);

    QSignalSpy spy(&svc, &ProjectTrackingService::progressUpdated);
    svc.updateTaskCompletion(1, 5);
    QCOMPARE(spy.count(), 1);

    ProgressStatus status = svc.trackProgress(1);
    QCOMPARE(status.completedTasks, 5);
    QCOMPARE(status.progress, 50);
  }

  void testRejectInvalidTaskCompletionUpdates() {
    ProjectTrackingService svc;

    ProjectTrackData data;
    data.projectId = 1;
    data.totalTasks = 10;
    data.completedTasks = 2;
    svc.addProject(data);

    QSignalSpy spy(&svc, &ProjectTrackingService::progressUpdated);
    QVERIFY(!svc.updateTaskCompletion(1, -1));
    QVERIFY(!svc.updateTaskCompletion(1, 11));

    ProgressStatus status = svc.trackProgress(1);
    QCOMPARE(status.completedTasks, 2);
    QCOMPARE(spy.count(), 0);
  }

  // Verify logging time to nonexistent project fails
  // Verify logging time to nonexistent project fails
  void testLogTimeToNonexistent() {
    ProjectTrackingService svc;
    TimeEntry entry;
    entry.hours = 5.0;
    QVERIFY(!svc.logTime(999, entry));
  }

  // Verify logging cost to nonexistent project fails
  // Verify logging cost to nonexistent project fails
  void testLogCostToNonexistent() {
    ProjectTrackingService svc;
    CostEntry entry;
    entry.amount = 100.0;
    QVERIFY(!svc.logCost(999, entry));
  }

  void testRejectInvalidQualityMetrics() {
    ProjectTrackingService svc;
    ProjectTrackData data;
    data.projectId = 1;
    svc.addProject(data);

    QualityMetric metric;
    metric.name = "   ";
    metric.value = 1.0;
    metric.target = 1.0;
    QVERIFY(!svc.updateQualityMetric(1, metric));

    metric.name = "Coverage";
    metric.value = -1.0;
    QVERIFY(!svc.updateQualityMetric(1, metric));

    metric.value = 1.0;
    metric.target = 0.0;
    QVERIFY(!svc.updateQualityMetric(1, metric));

    QualityReport report = svc.trackQuality(1);
    QCOMPARE(report.metrics.size(), 0);
    QCOMPARE(report.overallScore, 0.0);
  }

  // Verify duplicate project ID is rejected
  // Verify duplicate project is rejected
  void testAddProjectDuplicate() {
    ProjectTrackingService svc;
    ProjectTrackData data;
    data.projectId = 1;
    data.totalTasks = 5;
    svc.addProject(data);
    QVERIFY(!svc.addProject(data));
  }

  // Verify projectCount increments correctly with multiple projects
  // Test project count tracking
  void testProjectCount() {
    ProjectTrackingService svc;
    QCOMPARE(svc.projectCount(), 0);

    ProjectTrackData d1;
    d1.projectId = 1;
    svc.addProject(d1);

    ProjectTrackData d2;
    d2.projectId = 2;
    svc.addProject(d2);

    QCOMPARE(svc.projectCount(), 2);
  }

  // Verify completed project shows Completed status and 100% percentage
  // Verify completed project status and 100% percentage
  void testCompletedProjectStatus() {
    ProjectTrackingService svc;
    ProjectTrackData data;
    data.projectId = 1;
    data.totalTasks = 10;
    data.completedTasks = 10;
    svc.addProject(data);

    ProgressStatus status = svc.trackProgress(1);
    QCOMPARE(status.status, ProjectTrackStatus::Completed);
    QCOMPARE(status.percentage, 100.0);
  }

  // Verify over-budget cost report shows zero remaining budget
  // Test over-budget cost clamps remaining to zero
  void testOverBudgetCost() {
    ProjectTrackingService svc;
    ProjectTrackData data;
    data.projectId = 1;
    data.budgetedCost = 1000;
    svc.addProject(data);

    CostEntry entry;
    entry.amount = 1500;
    svc.logCost(1, entry);

    CostReport report = svc.trackCost(1);
    QCOMPARE(report.remainingBudget, 0.0);
  }
};

QTEST_MAIN(ProjectTrackingServiceTest)
#include "project_tracking_service_test.moc"
