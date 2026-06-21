// ProjectReportingServiceTest — Tests for ProjectReportingService
//
// Test coverage:
//   - Status, progress, performance, and financial report generation
//   - Report recommendations for over-budget and low-performance projects
//   - Nonexistent project handling
//   - Duplicate project rejection
//   - Report count and listing
// ProjectReportingServiceTest — Tests for ProjectReportingService
//
// Test coverage:
//   - Status report generation with sections and timestamps
//   - Progress report with pie chart
//   - Performance report with bar chart
//   - Financial report with cost tables
//   - Report recommendations (budget, performance, progress)
//   - Nonexistent project handling
//   - Duplicate project rejection
//   - Report count and listing

#include <QTest>
#include <QSignalSpy>
#include "services/ProjectReportingService.h"

class ProjectReportingServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Create service instance
  void init() {
    svc_.reset(new ProjectReportingService);
  }

  // Generate status report and verify title, summary, sections, timestamp
  // Test status report generation
  void testGenerateStatusReport() {
    QSignalSpy spy(svc_.data(), &ProjectReportingService::reportGenerated);

    ReportProjectData data;
    data.projectId = 1;
    data.projectName = "Test Project";
    data.completedTasks = 7;
    data.totalTasks = 10;
    data.qualityScore = 95.0;
    data.totalCost = 4000;
    data.budgetedCost = 5000;
    svc_->addProject(data);

    ProjectDocReport r = svc_->generateStatusReport(1);
    QCOMPARE(spy.count(), 1);
    QVERIFY(!r.title.isEmpty());
    QVERIFY(!r.summary.isEmpty());
    QCOMPARE(r.sections.size(), 1);
    QVERIFY(r.timestamp.isValid());
  }

  // Generate progress report and verify summary, sections, charts
  // Test progress report with pie chart
  void testGenerateProgressReport() {
    QSignalSpy spy(svc_.data(), &ProjectReportingService::reportGenerated);

    ReportProjectData data;
    data.projectId = 1;
    data.projectName = "Test Project";
    data.completedTasks = 5;
    data.totalTasks = 10;
    data.totalHours = 40;
    data.budgetedHours = 100;
    svc_->addProject(data);

    ProjectDocReport r = svc_->generateProgressReport(1);
    QCOMPARE(spy.count(), 1);
    QVERIFY(!r.summary.isEmpty());
    QCOMPARE(r.sections.size(), 2);
    QCOMPARE(r.charts.size(), 1);
    QCOMPARE(r.charts[0].type, QString("pie"));
  }

  // Generate performance report and verify summary, sections, bar chart
  // Test performance report with bar chart
  void testGeneratePerformanceReport() {
    QSignalSpy spy(svc_.data(), &ProjectReportingService::reportGenerated);

    ReportProjectData data;
    data.projectId = 1;
    data.projectName = "Test Project";
    data.completedTasks = 8;
    data.totalTasks = 10;
    data.totalHours = 50;
    data.budgetedHours = 80;
    data.qualityScore = 92.0;
    svc_->addProject(data);

    ProjectDocReport r = svc_->generatePerformanceReport(1);
    QCOMPARE(spy.count(), 1);
    QVERIFY(!r.summary.isEmpty());
    QCOMPARE(r.sections.size(), 1);
    QCOMPARE(r.charts.size(), 1);
    QCOMPARE(r.charts[0].type, QString("bar"));
  }

  // Generate financial report and verify summary, tables, headers, rows
  // Test financial report with cost table
  void testGenerateFinancialReport() {
    QSignalSpy spy(svc_.data(), &ProjectReportingService::reportGenerated);

    ReportProjectData data;
    data.projectId = 1;
    data.projectName = "Test Project";
    data.totalCost = 3500;
    data.budgetedCost = 5000;
    svc_->addProject(data);

    ProjectDocReport r = svc_->generateFinancialReport(1);
    QCOMPARE(spy.count(), 1);
    QVERIFY(!r.summary.isEmpty());
    QCOMPARE(r.sections.size(), 1);
    QCOMPARE(r.tables.size(), 1);
    QCOMPARE(r.tables[0].headers.size(), 2);
    QCOMPARE(r.tables[0].rows.size(), 3);
  }

  // Verify over-budget project generates financial recommendations
  // Test recommendations for over-budget project
  void testReportRecommendations() {
    ReportProjectData data;
    data.projectId = 1;
    data.projectName = "Over Budget Project";
    data.totalCost = 5500;
    data.budgetedCost = 5000;
    data.completedTasks = 2;
    data.totalTasks = 10;
    svc_->addProject(data);

    ProjectDocReport r = svc_->generateFinancialReport(1);
    QVERIFY(!r.recommendations.isEmpty());
  }

  // Verify nonexistent project returns empty report
  // Verify nonexistent project returns empty report
  void testNonexistentProject() {
    ProjectDocReport r = svc_->generateStatusReport(999);
    QVERIFY(r.title.isEmpty());
  }

  // Verify duplicate project ID is rejected
  // Verify duplicate project is rejected
  void testDuplicateProject() {
    ReportProjectData data;
    data.projectId = 1;
    data.projectName = "P1";
    svc_->addProject(data);
    QVERIFY(!svc_->addProject(data));
  }

  // Verify reportCount increments after generating reports
  // Test report count tracking
  void testReportCount() {
    QCOMPARE(svc_->reportCount(), 0);

    ReportProjectData data;
    data.projectId = 1;
    data.projectName = "P1";
    svc_->addProject(data);

    svc_->generateStatusReport(1);
    QCOMPARE(svc_->reportCount(), 1);

    svc_->generateProgressReport(1);
    QCOMPARE(svc_->reportCount(), 2);
  }

  // Verify allReports returns all generated reports
  // Test listing all reports
  void testAllReports() {
    ReportProjectData data;
    data.projectId = 1;
    data.projectName = "P1";
    svc_->addProject(data);

    svc_->generateStatusReport(1);
    svc_->generatePerformanceReport(1);

    QCOMPARE(svc_->allReports().size(), 2);
  }

  // Verify low-performance project generates multiple recommendations
  // Test performance recommendations for low-scoring project
  void testPerformanceRecommendations() {
    ReportProjectData data;
    data.projectId = 1;
    data.projectName = "Low Perf Project";
    data.completedTasks = 3;
    data.totalTasks = 10;
    data.totalHours = 50;
    data.budgetedHours = 80;
    data.qualityScore = 70.0;
    svc_->addProject(data);

    ProjectDocReport r = svc_->generatePerformanceReport(1);
    QVERIFY(r.recommendations.size() >= 2);
  }

  // Verify slow-progress project generates progress recommendations
  // Test progress recommendations for slow project
  void testProgressRecommendations() {
    ReportProjectData data;
    data.projectId = 1;
    data.projectName = "Slow Project";
    data.completedTasks = 1;
    data.totalTasks = 10;
    data.totalHours = 80;
    data.budgetedHours = 100;
    svc_->addProject(data);

    ProjectDocReport r = svc_->generateProgressReport(1);
    QVERIFY(!r.recommendations.isEmpty());
  }

private:
  QScopedPointer<ProjectReportingService> svc_;
};

QTEST_MAIN(ProjectReportingServiceTest)
#include "project_reporting_service_test.moc"
