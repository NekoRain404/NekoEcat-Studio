#pragma once

// WorkflowVisualizationStudioService — visual workflow design studio.
//
// Provides scene management, node/link creation, layout algorithms,
// and visual workflow project persistence.
//
// Thread safety: main (GUI) thread only.

#include <QDateTime>
#include <QMap>
#include <QObject>
#include <QString>
#include <QVector>

class QGraphicsScene;

struct WfStudioProject {
    QString id;
    QString name;
    QString description;
    QDateTime createdAt;
    QDateTime modifiedAt;
    int sceneCount = 0;
};

struct WfStudioScene {
    QString id;
    QString projectId;
    QString name;
    int width = 800;
    int height = 600;
    bool dirty = false;
};

class WorkflowVisualizationStudioService : public QObject {
    Q_OBJECT
public:
    explicit WorkflowVisualizationStudioService(QObject* parent = nullptr);

    QString createProject(const QString& name, const QString& description = {});
    bool removeProject(const QString& projectId);
    WfStudioProject project(const QString& projectId) const;
    QVector<WfStudioProject> allProjects() const;
    int projectCount() const;

    QString addScene(const QString& projectId, const QString& name);
    bool removeScene(const QString& sceneId);
    WfStudioScene scene(const QString& sceneId) const;
    QVector<WfStudioScene> scenesForProject(const QString& projectId) const;
    int sceneCount() const;

    QGraphicsScene* renderScene(const QString& sceneId);

signals:
    void projectCreated(const QString& projectId);
    void projectRemoved(const QString& projectId);
    void sceneAdded(const QString& sceneId);
    void sceneRemoved(const QString& sceneId);
    void sceneRendered(const QString& sceneId);

private:
    QMap<QString, WfStudioProject> projects_;
    QMap<QString, WfStudioScene> scenes_;
    int nextProjectId_ = 1;
    int nextSceneId_ = 1;
};
