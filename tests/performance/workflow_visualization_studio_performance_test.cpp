#include "services/WorkflowVisualizationStudioService.h"
#include <QElapsedTimer>
#include <QGraphicsScene>
#include <QTest>

class WorkflowVisualizationStudioPerformanceTest : public QObject {
    Q_OBJECT
private slots:
    void testCreateProjectThroughput() {
        WorkflowVisualizationStudioService svc;
        QElapsedTimer timer;
        timer.start();
        const int count = 10000;
        for (int i = 0; i < count; i++) {
            svc.createProject(QString("Project_%1").arg(i), "Test");
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 5000);
        qDebug() << "CreateProject throughput:" << count << "projects in" << elapsed << "ms";
    }

    void testAddSceneThroughput() {
        WorkflowVisualizationStudioService svc;
        QString projId = svc.createProject("PerfTest");
        QElapsedTimer timer;
        timer.start();
        const int count = 10000;
        for (int i = 0; i < count; i++) {
            svc.addScene(projId, QString("Scene_%1").arg(i));
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 5000);
        qDebug() << "AddScene throughput:" << count << "scenes in" << elapsed << "ms";
    }

    void testRenderSceneThroughput() {
        WorkflowVisualizationStudioService svc;
        QString projId = svc.createProject("RenderTest");
        QVector<QString> sceneIds;
        for (int i = 0; i < 100; i++) {
            sceneIds << svc.addScene(projId, QString("Scene_%1").arg(i));
        }
        QElapsedTimer timer;
        timer.start();
        for (const auto& id : sceneIds) {
            auto* scene = svc.renderScene(id);
            delete scene;
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 5000);
        qDebug() << "RenderScene throughput:" << sceneIds.size() << "scenes in" << elapsed << "ms";
    }

    void testQueryThroughput() {
        WorkflowVisualizationStudioService svc;
        for (int i = 0; i < 100; i++) {
            QString projId = svc.createProject(QString("Proj_%1").arg(i));
            for (int j = 0; j < 10; j++) {
                svc.addScene(projId, QString("Scene_%1").arg(j));
            }
        }
        QElapsedTimer timer;
        timer.start();
        const int count = 10000;
        for (int i = 0; i < count; i++) {
            svc.allProjects();
            svc.sceneCount();
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 5000);
        qDebug() << "Query throughput:" << count << "queries in" << elapsed << "ms";
    }
};

QTEST_MAIN(WorkflowVisualizationStudioPerformanceTest)
#include "workflow_visualization_studio_performance_test.moc"
