// ResourceManagementServiceTest — Tests for Resource Management Service
//
// Test coverage:
//   - Resource allocation with configuration
//   - Resource tracking and status monitoring
//   - Project allocation and release
//   - Resource optimization suggestions
//   - Report generation
//   - Status updates and filtering by type
//   - Available resources query
//   - Nonexistent resource handling
//   - Multiple allocations per resource
#include <QTest>
#include <QSignalSpy>
#include "services/ResourceManagementService.h"

class ResourceManagementServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Allocate a resource with full configuration and verify signal
  void testAllocateResource() {
    ResourceManagementService svc;
    QSignalSpy spy(&svc, &ResourceManagementService::resourceAllocated);

    ResourceConfig cfg;
    cfg.name = "Engineer A";
    cfg.type = ResourceType::Human;
    cfg.capacity = 100;
    cfg.costPerHour = 50.0;
    cfg.location = "Office 1";
    cfg.skills = {"ethercat", "c++"};

    Resource r = svc.allocateResource(cfg);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(r.name, QString("Engineer A"));
    QCOMPARE(r.type, ResourceType::Human);
    QCOMPARE(r.status, ResourceStatus::Available);
    QCOMPARE(r.skills.size(), 2);
  }

  void testRejectInvalidResourceConfig() {
    ResourceManagementService svc;
    QSignalSpy spy(&svc, &ResourceManagementService::resourceAllocated);

    ResourceConfig cfg;
    cfg.name = "   ";
    Resource r = svc.allocateResource(cfg);
    QCOMPARE(r.id, 0);
    QCOMPARE(svc.resourceCount(), 0);
    QCOMPARE(spy.count(), 0);

    cfg.name = "Invalid capacity";
    cfg.capacity = 0;
    r = svc.allocateResource(cfg);
    QCOMPARE(r.id, 0);
    QCOMPARE(svc.resourceCount(), 0);

    cfg.capacity = 100;
    cfg.costPerHour = -1.0;
    r = svc.allocateResource(cfg);
    QCOMPARE(r.id, 0);
    QCOMPARE(svc.resourceCount(), 0);
  }

  // Track resource status and utilization
  void testTrackResource() {
    ResourceManagementService svc;
    ResourceConfig cfg;
    cfg.name = "Track Test";
    Resource r = svc.allocateResource(cfg);

    ResourceStatusInfo info = svc.trackResource(r.id);
    QCOMPARE(info.resourceId, r.id);
    QCOMPARE(info.status, ResourceStatus::Available);
    QCOMPARE(info.utilizationPercent, 0);
  }

  // Allocate resource to a project with load percentage
  void testAllocateToProject() {
    ResourceManagementService svc;
    ResourceConfig cfg;
    cfg.name = "Alloc Test";
    Resource r = svc.allocateResource(cfg);

    QSignalSpy spy(&svc, &ResourceManagementService::resourceUpdated);
    QVERIFY(svc.allocateToProject(r.id, 1, 50));
    QCOMPARE(spy.count(), 1);

    ResourceStatusInfo info = svc.trackResource(r.id);
    QCOMPARE(info.status, ResourceStatus::Allocated);
    QCOMPARE(info.currentLoad, 50);
    QCOMPARE(info.utilizationPercent, 50);
  }

  void testRejectInvalidProjectAllocation() {
    ResourceManagementService svc;
    ResourceConfig cfg;
    cfg.name = "Invalid Alloc Test";
    Resource r = svc.allocateResource(cfg);

    QSignalSpy spy(&svc, &ResourceManagementService::resourceUpdated);
    QVERIFY(!svc.allocateToProject(r.id, 0, 50));
    QVERIFY(!svc.allocateToProject(r.id, 1, 0));
    QVERIFY(!svc.allocateToProject(r.id, 1, -10));
    QVERIFY(!svc.allocateToProject(r.id, 1, 101));

    ResourceStatusInfo info = svc.trackResource(r.id);
    QCOMPARE(info.status, ResourceStatus::Available);
    QCOMPARE(info.currentLoad, 0);
    QCOMPARE(svc.resourceAllocations(r.id).size(), 0);
    QCOMPARE(spy.count(), 0);
  }

  // Release resource from project and verify availability
  void testReleaseFromProject() {
    ResourceManagementService svc;
    ResourceConfig cfg;
    cfg.name = "Release Test";
    Resource r = svc.allocateResource(cfg);

    svc.allocateToProject(r.id, 1, 50);
    QVERIFY(svc.releaseFromProject(r.id, 1));

    ResourceStatusInfo info = svc.trackResource(r.id);
    QCOMPARE(info.currentLoad, 0);
    QCOMPARE(info.status, ResourceStatus::Available);
  }

  // Optimize resources and verify suggestions
  void testOptimizeResources() {
    ResourceManagementService svc;

    ResourceConfig cfg;
    cfg.name = "Underused";
    cfg.capacity = 100;
    cfg.costPerHour = 100.0;
    Resource r = svc.allocateResource(cfg);
    svc.updateLoad(r.id, 10);

    cfg.name = "Overloaded";
    cfg.costPerHour = 50.0;
    Resource r2 = svc.allocateResource(cfg);
    svc.updateLoad(r2.id, 95);

    ResourceOptimizationResult result = svc.optimizeResources();
    QVERIFY(result.suggestions.size() >= 2);
    QVERIFY(result.totalPotentialSaving > 0.0);
    QVERIFY(result.generatedAt.isValid());
  }

  // Generate resource report with totals
  void testGenerateReport() {
    ResourceManagementService svc;
    ResourceConfig cfg;
    cfg.name = "R1";
    cfg.costPerHour = 10.0;
    svc.allocateResource(cfg);
    cfg.name = "R2";
    cfg.costPerHour = 20.0;
    svc.allocateResource(cfg);

    ResourceReport report = svc.generateResourceReport();
    QCOMPARE(report.totalResources, 2);
    QCOMPARE(report.availableResources, 2);
    QCOMPARE(report.totalCostPerHour, 30.0);
    QVERIFY(report.generatedAt.isValid());
  }

  // Update resource status to maintenance
  void testUpdateStatus() {
    ResourceManagementService svc;
    ResourceConfig cfg;
    cfg.name = "Status Test";
    Resource r = svc.allocateResource(cfg);

    QVERIFY(svc.updateResourceStatus(r.id, ResourceStatus::Maintenance));
    ResourceStatusInfo info = svc.trackResource(r.id);
    QCOMPARE(info.status, ResourceStatus::Maintenance);
  }

  void testRejectInvalidLoadUpdates() {
    ResourceManagementService svc;
    ResourceConfig cfg;
    cfg.name = "Load Test";
    cfg.capacity = 80;
    Resource r = svc.allocateResource(cfg);

    QVERIFY(!svc.updateLoad(r.id, -1));
    QVERIFY(!svc.updateLoad(r.id, 81));

    ResourceStatusInfo info = svc.trackResource(r.id);
    QCOMPARE(info.currentLoad, 0);
    QCOMPARE(info.utilizationPercent, 0);
  }

  // Filter resources by type
  void testResourcesByType() {
    ResourceManagementService svc;
    ResourceConfig cfg;
    cfg.name = "Human";
    cfg.type = ResourceType::Human;
    svc.allocateResource(cfg);
    cfg.name = "Hardware";
    cfg.type = ResourceType::Hardware;
    svc.allocateResource(cfg);
    cfg.name = "Human2";
    cfg.type = ResourceType::Human;
    svc.allocateResource(cfg);

    QCOMPARE(svc.resourcesByType(ResourceType::Human).size(), 2);
    QCOMPARE(svc.resourcesByType(ResourceType::Hardware).size(), 1);
  }

  // Query available resources excluding maintenance
  void testAvailableResources() {
    ResourceManagementService svc;
    ResourceConfig cfg;
    cfg.name = "R1";
    Resource r = svc.allocateResource(cfg);
    cfg.name = "R2";
    svc.allocateResource(cfg);

    svc.updateResourceStatus(r.id, ResourceStatus::Maintenance);
    QCOMPARE(svc.availableResources().size(), 1);
  }

  // Handle operations on nonexistent resource gracefully
  void testNonexistentResource() {
    ResourceManagementService svc;
    ResourceStatusInfo info = svc.trackResource(999);
    QCOMPARE(info.resourceId, 0);
    QVERIFY(!svc.allocateToProject(999, 1, 50));
    QCOMPARE(svc.resourceCount(), 0);
  }

  // Multiple project allocations for single resource
  void testResourceAllocations() {
    ResourceManagementService svc;
    ResourceConfig cfg;
    cfg.name = "Multi Alloc";
    Resource r = svc.allocateResource(cfg);

    svc.allocateToProject(r.id, 1, 30);
    svc.allocateToProject(r.id, 2, 40);

    auto allocs = svc.resourceAllocations(r.id);
    QCOMPARE(allocs.size(), 2);

    ResourceStatusInfo info = svc.trackResource(r.id);
    QCOMPARE(info.currentLoad, 70);
  }
};

QTEST_MAIN(ResourceManagementServiceTest)
#include "resource_management_service_test.moc"
