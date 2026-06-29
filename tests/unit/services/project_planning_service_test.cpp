// ProjectPlanningServiceTest — Tests for ProjectPlanningService
//
// Test coverage:
//   - Milestone creation with priority, deliverables, criteria, dependencies
//   - Timeline creation with date range and milestone IDs
//   - Resource planning and over-allocation detection
//   - Risk assessment and risk report generation
//   - Milestone completion and priority updates
//   - Milestone and timeline querying
//   - Nonexistent milestone handling
// ProjectPlanningServiceTest — Tests for ProjectPlanningService
//
// Test coverage:
//   - Milestone creation with deliverables, criteria, dependencies
//   - Timeline creation with milestone references
//   - Resource planning (valid, over-allocated, mismatched)
//   - Risk assessment with scoring
//   - Milestone completion and priority updates
//   - Milestone and timeline queries
//   - Nonexistent milestone handling

#include <QTest>
#include <QSignalSpy>
#include "services/ProjectPlanningService.h"

class ProjectPlanningServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Test creating a milestone with full configuration
  void testCreateMilestone() {
    ProjectPlanningService svc;
    QSignalSpy spy(&svc, &ProjectPlanningService::milestoneCreated);

    MilestoneConfig cfg;
    cfg.name = "Alpha Release";
    cfg.description = "First alpha";
    cfg.deadline = QDateTime::currentDateTime().addDays(30);
    cfg.deliverables = {"Prototype", "Docs"};
    cfg.criteria = {"All tests pass"};
    cfg.dependencies = {"Design phase"};
    cfg.priority = MilestonePriority::High;

    PlannedMilestone m = svc.createMilestone(cfg);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(m.name, QString("Alpha Release"));
    QCOMPARE(m.priority, MilestonePriority::High);
    QVERIFY(!m.completed);
    QCOMPARE(m.deliverables.size(), 2);
    QCOMPARE(m.criteria.size(), 1);
    QCOMPARE(m.dependencies.size(), 1);
  }

  void testRejectEmptyMilestoneName() {
    ProjectPlanningService svc;
    QSignalSpy spy(&svc, &ProjectPlanningService::milestoneCreated);

    MilestoneConfig cfg;
    cfg.name = "   ";
    PlannedMilestone m = svc.createMilestone(cfg);

    QCOMPARE(m.id, 0);
    QCOMPARE(svc.milestoneCount(), 0);
    QCOMPARE(spy.count(), 0);
  }

  // Create timeline and verify signal, name, and milestone IDs
  // Test creating a timeline with milestone references
  void testCreateTimeline() {
    ProjectPlanningService svc;
    QSignalSpy spy(&svc, &ProjectPlanningService::timelineCreated);

    MilestoneConfig milestoneCfg;
    milestoneCfg.name = "Milestone";
    PlannedMilestone m1 = svc.createMilestone(milestoneCfg);
    milestoneCfg.name = "Milestone 2";
    PlannedMilestone m2 = svc.createMilestone(milestoneCfg);

    TimelineConfig cfg;
    cfg.name = "Sprint 1";
    cfg.startDate = QDateTime::currentDateTime();
    cfg.endDate = QDateTime::currentDateTime().addDays(14);
    cfg.milestoneIds = {m1.id, m2.id};
    cfg.description = "First sprint";

    Timeline t = svc.createTimeline(cfg);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(t.name, QString("Sprint 1"));
    QCOMPARE(t.milestoneIds.size(), 2);
  }

  void testRejectInvalidTimelineConfig() {
    ProjectPlanningService svc;
    QSignalSpy spy(&svc, &ProjectPlanningService::timelineCreated);

    TimelineConfig cfg;
    cfg.name = "   ";
    cfg.startDate = QDateTime::currentDateTime();
    cfg.endDate = cfg.startDate.addDays(1);
    Timeline t = svc.createTimeline(cfg);
    QCOMPARE(t.id, 0);
    QCOMPARE(svc.timelineCount(), 0);

    cfg.name = "Invalid range";
    cfg.endDate = cfg.startDate.addSecs(-1);
    t = svc.createTimeline(cfg);
    QCOMPARE(t.id, 0);
    QCOMPARE(svc.timelineCount(), 0);

    cfg.endDate = cfg.startDate.addDays(1);
    cfg.milestoneIds = {999};
    t = svc.createTimeline(cfg);
    QCOMPARE(t.id, 0);
    QCOMPARE(svc.timelineCount(), 0);
    QCOMPARE(spy.count(), 0);
  }

  // Plan resources within allocation limits and verify success
  // Test valid resource allocation plan
  void testPlanResources() {
    ProjectPlanningService svc;

    ResourcePlan plan;
    plan.projectId = 1;
    plan.resourceNames = {"Engineer A", "Engineer B"};
    plan.allocationPercent = {50, 30};

    QVERIFY(svc.planResources(plan));
  }

  // Verify over-allocated resource plan is rejected
  // Verify over-allocated resources are rejected
  void testPlanResourcesOverAllocated() {
    ProjectPlanningService svc;

    ResourcePlan plan;
    plan.projectId = 1;
    plan.resourceNames = {"A", "B"};
    plan.allocationPercent = {60, 50};

    QVERIFY(!svc.planResources(plan));
  }

  // Verify mismatched resource/allocation list sizes are rejected
  // Verify mismatched resource/allocation count is rejected
  void testPlanResourcesMismatched() {
    ProjectPlanningService svc;

    ResourcePlan plan;
    plan.projectId = 1;
    plan.resourceNames = {"A", "B"};
    plan.allocationPercent = {50};

    QVERIFY(!svc.planResources(plan));
  }

  void testRejectInvalidResourcePlanPercentages() {
    ProjectPlanningService svc;

    ResourcePlan plan;
    plan.projectId = 0;
    plan.resourceNames = {"A"};
    plan.allocationPercent = {50};
    QVERIFY(!svc.planResources(plan));

    plan.projectId = 1;
    plan.allocationPercent = {-10};
    QVERIFY(!svc.planResources(plan));

    plan.allocationPercent = {0};
    QVERIFY(!svc.planResources(plan));

    plan.allocationPercent = {101};
    QVERIFY(!svc.planResources(plan));
  }

  // Assess risks and verify risk counts and overall score
  // Test risk assessment with scoring
  void testAssessRisks() {
    ProjectPlanningService svc;

    RiskAssessment assessment;
    assessment.projectId = 1;

    RiskItem r1;
    r1.description = "Supply delay";
    r1.level = RiskLevel::High;
    r1.probability = 0.7;
    r1.impact = 0.8;
    r1.mitigation = "Order early";
    assessment.risks.append(r1);

    RiskItem r2;
    r2.description = "Staff shortage";
    r2.level = RiskLevel::Medium;
    r2.probability = 0.5;
    r2.impact = 0.6;
    assessment.risks.append(r2);

    RiskReport report = svc.assessRisks(assessment);
    QCOMPARE(report.projectId, 1);
    QCOMPARE(report.risks.size(), 2);
    QCOMPARE(report.highRiskCount, 1);
    QCOMPARE(report.mediumRiskCount, 1);
    QCOMPARE(report.lowRiskCount, 0);
    QVERIFY(report.overallRiskScore > 0.0);
    QVERIFY(report.generatedAt.isValid());
  }

  // Complete a milestone and verify completed flag
  // Test milestone completion
  void testCompleteMilestone() {
    ProjectPlanningService svc;
    MilestoneConfig cfg;
    cfg.name = "M1";
    PlannedMilestone m = svc.createMilestone(cfg);

    QVERIFY(svc.completeMilestone(m.id));
    PlannedMilestone fetched = svc.milestone(m.id);
    QVERIFY(fetched.completed);
  }

  // Update milestone priority and verify the change persists
  // Test milestone priority update
  void testUpdateMilestonePriority() {
    ProjectPlanningService svc;
    MilestoneConfig cfg;
    cfg.name = "M1";
    PlannedMilestone m = svc.createMilestone(cfg);

    QVERIFY(svc.updateMilestonePriority(m.id, MilestonePriority::Critical));
    PlannedMilestone fetched = svc.milestone(m.id);
    QCOMPARE(fetched.priority, MilestonePriority::Critical);
  }

  // Query milestones and verify count and allMilestones list
  // Test querying milestones
  void testMilestoneQuery() {
    ProjectPlanningService svc;
    MilestoneConfig cfg;
    cfg.name = "M1";
    svc.createMilestone(cfg);
    cfg.name = "M2";
    svc.createMilestone(cfg);

    QCOMPARE(svc.milestoneCount(), 2);
    QCOMPARE(svc.allMilestones().size(), 2);
  }

  // Query timelines and verify count and allTimelines list
  // Test querying timelines
  void testTimelineQuery() {
    ProjectPlanningService svc;
    TimelineConfig cfg;
    cfg.name = "T1";
    svc.createTimeline(cfg);

    QCOMPARE(svc.timelineCount(), 1);
    QCOMPARE(svc.allTimelines().size(), 1);
  }

  // Verify nonexistent milestone returns zeroed data and failed operations
  // Verify nonexistent milestone returns defaults
  void testNonexistentMilestone() {
    ProjectPlanningService svc;
    PlannedMilestone m = svc.milestone(999);
    QCOMPARE(m.id, 0);
    QVERIFY(!svc.completeMilestone(999));
    QCOMPARE(svc.milestoneCount(), 0);
  }
};

QTEST_MAIN(ProjectPlanningServiceTest)
#include "project_planning_service_test.moc"
