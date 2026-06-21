// ProjectManagementServiceTest — Tests for ProjectManagementService
//
// Test coverage:
//   - Project creation with milestones and deliverables
//   - Project tracking and status updates
//   - Report generation
//   - Collaboration entry logging
//   - Milestone completion and item delivery
//   - Nonexistent project handling
//   - Project listing and count
// ProjectManagementServiceTest — Tests for ProjectManagementService
//
// Test coverage:
//   - Project creation with milestones and deliverables
//   - Project tracking (status, completion percent)
//   - Report generation
//   - Collaboration entries and history
//   - Status updates
//   - Milestone completion
//   - Deliverable delivery
//   - Nonexistent project handling
//   - Multiple project listing

#include <QTest>
#include <QSignalSpy>
#include "services/ProjectManagementService.h"

class ProjectManagementServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Test creating a project with milestones and deliverables
  void testCreateProject() {
    ProjectManagementService svc;
    QSignalSpy spy(&svc, &ProjectManagementService::projectCreated);

    ProjectConfig cfg;
    cfg.name = "Test Project";
    cfg.description = "A test project";
    cfg.scope = "Unit testing";

    Milestone m;
    m.name = "M1";
    m.description = "First milestone";
    cfg.milestones.append(m);

    Deliverable d;
    d.name = "D1";
    d.description = "First deliverable";
    cfg.deliverables.append(d);

    Project p = svc.createProject(cfg);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(p.name, QString("Test Project"));
    QCOMPARE(p.status, ProjectStatus::NotStarted);
    QCOMPARE(p.milestones.size(), 1);
    QCOMPARE(p.deliverables.size(), 1);
    QVERIFY(p.milestones[0].id > 0);
    QVERIFY(p.deliverables[0].id > 0);
  }

  // Track project by ID and verify status and completion percent
  // Test tracking project status and completion
  void testTrackProject() {
    ProjectManagementService svc;
    ProjectConfig cfg;
    cfg.name = "Track Test";
    Project p = svc.createProject(cfg);

    ProjectStatusInfo info = svc.trackProject(p.id);
    QCOMPARE(info.projectId, p.id);
    QCOMPARE(info.status, ProjectStatus::NotStarted);
    QCOMPARE(info.completionPercent, 0);
  }

  // Generate report for a project and verify name and timestamp
  // Test generating a project report
  void testGenerateReport() {
    ProjectManagementService svc;
    ProjectConfig cfg;
    cfg.name = "Report Test";
    svc.createProject(cfg);

    ProjectReport report = svc.generateReport(1);
    QCOMPARE(report.projectName, QString("Report Test"));
    QVERIFY(report.generatedAt.isValid());
  }

  // Log collaboration entry and verify signal and project history
  // Test collaboration entry with history tracking
  void testCollaborate() {
    ProjectManagementService svc;
    ProjectConfig cfg;
    cfg.name = "Collab Test";
    Project p = svc.createProject(cfg);

    QSignalSpy spy(&svc, &ProjectManagementService::projectUpdated);
    CollaborationEntry entry;
    entry.user = "user1";
    entry.action = "created";
    entry.details = "initial setup";

    QVERIFY(svc.collaborate(p.id, entry));
    QCOMPARE(spy.count(), 1);

    auto history = svc.projectHistory(p.id);
    QCOMPARE(history.size(), 1);
    QCOMPARE(history[0].user, QString("user1"));
  }

  // Update project status and verify tracking reflects the change
  // Test project status update
  void testUpdateStatus() {
    ProjectManagementService svc;
    ProjectConfig cfg;
    cfg.name = "Status Test";
    Project p = svc.createProject(cfg);

    QVERIFY(svc.updateProjectStatus(p.id, ProjectStatus::InProgress));
    ProjectStatusInfo info = svc.trackProject(p.id);
    QCOMPARE(info.status, ProjectStatus::InProgress);
  }

  // Complete a milestone and verify completedMilestones count
  // Test milestone completion tracking
  void testCompleteMilestone() {
    ProjectManagementService svc;
    ProjectConfig cfg;
    cfg.name = "Milestone Test";
    Milestone m;
    m.name = "M1";
    cfg.milestones.append(m);
    Project p = svc.createProject(cfg);

    int mid = p.milestones[0].id;
    QVERIFY(svc.completeMilestone(p.id, mid));

    ProjectStatusInfo info = svc.trackProject(p.id);
    QCOMPARE(info.completedMilestones, 1);
  }

  // Deliver an item and verify deliveredItems count
  // Test deliverable delivery tracking
  void testDeliverItem() {
    ProjectManagementService svc;
    ProjectConfig cfg;
    cfg.name = "Deliver Test";
    Deliverable d;
    d.name = "D1";
    cfg.deliverables.append(d);
    Project p = svc.createProject(cfg);

    int did = p.deliverables[0].id;
    QVERIFY(svc.deliverItem(p.id, did));

    ProjectStatusInfo info = svc.trackProject(p.id);
    QCOMPARE(info.deliveredItems, 1);
  }

  // Verify nonexistent project returns zeroed info and failed operations
  // Verify nonexistent project returns defaults
  void testNonexistentProject() {
    ProjectManagementService svc;
    ProjectStatusInfo info = svc.trackProject(999);
    QCOMPARE(info.projectId, 0);
    QVERIFY(!svc.collaborate(999, {}));
    QCOMPARE(svc.projectCount(), 0);
  }

  // Create multiple projects and verify projectCount and allProjects
  // Test listing all projects
  void testAllProjects() {
    ProjectManagementService svc;
    ProjectConfig cfg;
    cfg.name = "P1";
    svc.createProject(cfg);
    cfg.name = "P2";
    svc.createProject(cfg);

    QCOMPARE(svc.projectCount(), 2);
    QCOMPARE(svc.allProjects().size(), 2);
  }
};

QTEST_MAIN(ProjectManagementServiceTest)
#include "project_management_service_test.moc"
