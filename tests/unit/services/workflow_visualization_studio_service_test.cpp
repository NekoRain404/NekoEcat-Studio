#include <QTest>
#include <QSignalSpy>
#include <QGraphicsScene>
#include "services/WorkflowVisualizationStudioService.h"

class WorkflowVisualizationStudioServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testCreateProject() {
    WorkflowVisualizationStudioService svc;
    QSignalSpy spy(&svc, &WorkflowVisualizationStudioService::projectCreated);
    QString id = svc.createProject("Test Project", "A test");
    QVERIFY(!id.isEmpty());
    QVERIFY(id.startsWith("proj_"));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), id);
  }

  void testRemoveProject() {
    WorkflowVisualizationStudioService svc;
    QString id = svc.createProject("ToRemove");
    QSignalSpy spy(&svc, &WorkflowVisualizationStudioService::projectRemoved);
    QVERIFY(svc.removeProject(id));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(svc.projectCount(), 0);
  }

  void testProjectLookup() {
    WorkflowVisualizationStudioService svc;
    QString id = svc.createProject("Lookup Test", "Desc");
    WfStudioProject p = svc.project(id);
    QCOMPARE(p.id, id);
    QCOMPARE(p.name, QString("Lookup Test"));
    QCOMPARE(p.description, QString("Desc"));
  }

  void testAllProjects() {
    WorkflowVisualizationStudioService svc;
    svc.createProject("P1");
    svc.createProject("P2");
    svc.createProject("P3");
    QCOMPARE(svc.allProjects().size(), 3);
  }

  void testAddScene() {
    WorkflowVisualizationStudioService svc;
    QString projId = svc.createProject("Proj");
    QSignalSpy spy(&svc, &WorkflowVisualizationStudioService::sceneAdded);
    QString sceneId = svc.addScene(projId, "Scene1");
    QVERIFY(!sceneId.isEmpty());
    QVERIFY(sceneId.startsWith("scene_"));
    QCOMPARE(spy.count(), 1);
    WfStudioProject p = svc.project(projId);
    QCOMPARE(p.sceneCount, 1);
  }

  void testRemoveScene() {
    WorkflowVisualizationStudioService svc;
    QString projId = svc.createProject("Proj");
    QString sceneId = svc.addScene(projId, "Scene1");
    QSignalSpy spy(&svc, &WorkflowVisualizationStudioService::sceneRemoved);
    QVERIFY(svc.removeScene(sceneId));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(svc.sceneCount(), 0);
    WfStudioProject p = svc.project(projId);
    QCOMPARE(p.sceneCount, 0);
  }

  void testScenesForProject() {
    WorkflowVisualizationStudioService svc;
    QString projId = svc.createProject("Proj");
    svc.addScene(projId, "S1");
    svc.addScene(projId, "S2");
    svc.addScene(projId, "S3");
    QCOMPARE(svc.scenesForProject(projId).size(), 3);
  }

  void testRenderScene() {
    WorkflowVisualizationStudioService svc;
    QString projId = svc.createProject("Proj");
    QString sceneId = svc.addScene(projId, "Render Me");
    QSignalSpy spy(&svc, &WorkflowVisualizationStudioService::sceneRendered);
    QGraphicsScene *gs = svc.renderScene(sceneId);
    QVERIFY(gs != nullptr);
    QCOMPARE(spy.count(), 1);
    delete gs;
  }

  void testProjectNotFound() {
    WorkflowVisualizationStudioService svc;
    WfStudioProject p = svc.project("nonexistent");
    QVERIFY(p.id.isEmpty());
  }

  void testSceneNotFound() {
    WorkflowVisualizationStudioService svc;
    WfStudioScene s = svc.scene("nonexistent");
    QVERIFY(s.id.isEmpty());
  }

  void testRemoveProjectCascades() {
    WorkflowVisualizationStudioService svc;
    QString projId = svc.createProject("Proj");
    svc.addScene(projId, "S1");
    svc.addScene(projId, "S2");
    QCOMPARE(svc.sceneCount(), 2);
    svc.removeProject(projId);
    QCOMPARE(svc.sceneCount(), 0);
  }

  void testProjectCount() {
    WorkflowVisualizationStudioService svc;
    QCOMPARE(svc.projectCount(), 0);
    svc.createProject("P1");
    QCOMPARE(svc.projectCount(), 1);
    svc.createProject("P2");
    QCOMPARE(svc.projectCount(), 2);
  }

  void testSceneCount() {
    WorkflowVisualizationStudioService svc;
    QCOMPARE(svc.sceneCount(), 0);
    QString projId = svc.createProject("Proj");
    svc.addScene(projId, "S1");
    QCOMPARE(svc.sceneCount(), 1);
    svc.addScene(projId, "S2");
    QCOMPARE(svc.sceneCount(), 2);
  }

  void testSignalEmissions() {
    WorkflowVisualizationStudioService svc;
    QSignalSpy pcSpy(&svc, &WorkflowVisualizationStudioService::projectCreated);
    QSignalSpy prSpy(&svc, &WorkflowVisualizationStudioService::projectRemoved);
    QSignalSpy saSpy(&svc, &WorkflowVisualizationStudioService::sceneAdded);
    QSignalSpy srSpy(&svc, &WorkflowVisualizationStudioService::sceneRemoved);
    QSignalSpy renSpy(&svc, &WorkflowVisualizationStudioService::sceneRendered);

    QString projId = svc.createProject("Proj");
    svc.addScene(projId, "Scene2");
    QString sceneId = svc.addScene(projId, "Scene");
    QGraphicsScene *gs = svc.renderScene(sceneId);
    svc.removeScene(sceneId);
    svc.removeProject(projId); // cascades: removes Scene2

    QCOMPARE(pcSpy.count(), 1);
    QCOMPARE(prSpy.count(), 1);
    QCOMPARE(saSpy.count(), 2);
    QCOMPARE(srSpy.count(), 2); // explicit removeScene + project cascade
    QCOMPARE(renSpy.count(), 1);
    delete gs;
  }
};

QTEST_MAIN(WorkflowVisualizationStudioServiceTest)
#include "workflow_visualization_studio_service_test.moc"
