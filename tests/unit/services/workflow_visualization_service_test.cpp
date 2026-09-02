// WorkflowVisualizationServiceTest — Tests for Workflow Visualization Service
//
// Test coverage:
//   - Flowchart creation from configuration
//   - Gantt chart creation from task list
//   - Dependency graph creation from node/edge data
//   - Resource timeline visualization
//   - Signal emissions on visualization creation
//   - Scene cleanup and memory management

#include "services/WorkflowVisualizationService.h"
#include <QGraphicsScene>
#include <QJsonArray>
#include <QSignalSpy>
#include <QTest>

class WorkflowVisualizationServiceTest : public QObject {
    Q_OBJECT
private slots:
    // Create flowchart scene from config and verify signal and items
    // Flowchart creation emits signal and returns non-empty scene
    void testCreateFlowchart() {
        WorkflowVisualizationService svc;
        QSignalSpy spy(&svc, &WorkflowVisualizationService::visualizationCreated);
        WfVisualizationConfig config;
        config.viewType = WfVisualizationType::Flowchart;
        auto* scene = svc.createFlowchart(config);
        QVERIFY(scene != nullptr);
        QVERIFY(!scene->items().isEmpty());
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toString(), "flowchart");
        delete scene;
    }

    // Create Gantt chart scene from task list
    // Gantt chart creation emits signal with task timeline
    void testCreateGanttChart() {
        WorkflowVisualizationService svc;
        QSignalSpy spy(&svc, &WorkflowVisualizationService::visualizationCreated);
        QVector<WfVizTask> tasks;
        WfVizTask t1;
        t1.id = "t1";
        t1.name = "Setup";
        t1.startMs = 0;
        t1.durationMs = 100;
        tasks << t1;
        WfVizTask t2;
        t2.id = "t2";
        t2.name = "Process";
        t2.startMs = 100;
        t2.durationMs = 200;
        tasks << t2;
        auto* scene = svc.createGanttChart(tasks);
        QVERIFY(scene != nullptr);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toString(), "gantt");
        delete scene;
    }

    // Create dependency graph scene from node/edge JSON
    // Dependency graph creation from nodes and edges
    void testCreateDependencyGraph() {
        WorkflowVisualizationService svc;
        QSignalSpy spy(&svc, &WorkflowVisualizationService::visualizationCreated);
        QJsonObject graph;
        QJsonObject nodes;
        nodes["A"] = "Node A";
        nodes["B"] = "Node B";
        nodes["C"] = "Node C";
        graph["nodes"] = nodes;
        QJsonObject edges;
        edges["A"] = QJsonArray{"B"};
        edges["B"] = QJsonArray{"C"};
        graph["edges"] = edges;
        auto* scene = svc.createDependencyGraph(graph);
        QVERIFY(scene != nullptr);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toString(), "dependency_graph");
        delete scene;
    }

    // Create resource timeline scene from resource allocation data
    // Resource timeline creation from allocation data
    void testCreateResourceTimeline() {
        WorkflowVisualizationService svc;
        QSignalSpy spy(&svc, &WorkflowVisualizationService::visualizationCreated);
        QVector<WfVizResource> resources;
        WfVizResource r1;
        r1.id = "cpu";
        r1.name = "CPU";
        r1.allocations << QPair<double, double>(0, 50) << QPair<double, double>(100, 30);
        resources << r1;
        WfVizResource r2;
        r2.id = "mem";
        r2.name = "Memory";
        r2.allocations << QPair<double, double>(0, 80);
        resources << r2;
        auto* scene = svc.createResourceTimeline(resources);
        QVERIFY(scene != nullptr);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toString(), "resource_timeline");
        delete scene;
    }

    // Empty task list still returns valid scene
    void testEmptyGanttChart() {
        WorkflowVisualizationService svc;
        QVector<WfVizTask> tasks;
        auto* scene = svc.createGanttChart(tasks);
        QVERIFY(scene != nullptr);
        delete scene;
    }

    // Empty resource list still returns valid scene
    void testEmptyResourceTimeline() {
        WorkflowVisualizationService svc;
        QVector<WfVizResource> resources;
        auto* scene = svc.createResourceTimeline(resources);
        QVERIFY(scene != nullptr);
        delete scene;
    }

    // Cyclic dependency graph does not crash
    void testDependencyGraphWithCycles() {
        WorkflowVisualizationService svc;
        QJsonObject graph;
        QJsonObject nodes;
        nodes["A"] = "Node A";
        nodes["B"] = "Node B";
        graph["nodes"] = nodes;
        QJsonObject edges;
        edges["A"] = QJsonArray{"B"};
        edges["B"] = QJsonArray{"A"};
        graph["edges"] = edges;
        auto* scene = svc.createDependencyGraph(graph);
        QVERIFY(scene != nullptr);
        delete scene;
    }
};

QTEST_MAIN(WorkflowVisualizationServiceTest)
#include "workflow_visualization_service_test.moc"
