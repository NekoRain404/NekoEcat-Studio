#include "WorkflowVisualizationStudioService.h"
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>

WorkflowVisualizationStudioService::WorkflowVisualizationStudioService(QObject* parent) : QObject(parent) {}

QString WorkflowVisualizationStudioService::createProject(const QString& name, const QString& description) {
    QString id = QString("proj_%1").arg(nextProjectId_++);
    WfStudioProject p;
    p.id = id;
    p.name = name;
    p.description = description;
    p.createdAt = QDateTime::currentDateTime();
    p.modifiedAt = p.createdAt;
    projects_[id] = p;
    emit projectCreated(id);
    return id;
}

bool WorkflowVisualizationStudioService::removeProject(const QString& projectId) {
    if (!projects_.contains(projectId))
        return false;
    QList<QString> toRemove;
    for (auto it = scenes_.begin(); it != scenes_.end(); ++it) {
        if (it.value().projectId == projectId)
            toRemove << it.key();
    }
    for (const auto& sid : toRemove) {
        scenes_.remove(sid);
        emit sceneRemoved(sid);
    }
    projects_.remove(projectId);
    emit projectRemoved(projectId);
    return true;
}

WfStudioProject WorkflowVisualizationStudioService::project(const QString& projectId) const {
    return projects_.value(projectId, WfStudioProject{});
}

QVector<WfStudioProject> WorkflowVisualizationStudioService::allProjects() const {
    QVector<WfStudioProject> result;
    for (auto it = projects_.begin(); it != projects_.end(); ++it)
        result << it.value();
    return result;
}

int WorkflowVisualizationStudioService::projectCount() const {
    return projects_.size();
}

QString WorkflowVisualizationStudioService::addScene(const QString& projectId, const QString& name) {
    QString id = QString("scene_%1").arg(nextSceneId_++);
    WfStudioScene s;
    s.id = id;
    s.projectId = projectId;
    s.name = name;
    scenes_[id] = s;
    if (projects_.contains(projectId)) {
        projects_[projectId].sceneCount++;
        projects_[projectId].modifiedAt = QDateTime::currentDateTime();
    }
    emit sceneAdded(id);
    return id;
}

bool WorkflowVisualizationStudioService::removeScene(const QString& sceneId) {
    if (!scenes_.contains(sceneId))
        return false;
    QString projId = scenes_[sceneId].projectId;
    scenes_.remove(sceneId);
    if (projects_.contains(projId) && projects_[projId].sceneCount > 0) {
        projects_[projId].sceneCount--;
        projects_[projId].modifiedAt = QDateTime::currentDateTime();
    }
    emit sceneRemoved(sceneId);
    return true;
}

WfStudioScene WorkflowVisualizationStudioService::scene(const QString& sceneId) const {
    return scenes_.value(sceneId, WfStudioScene{});
}

QVector<WfStudioScene> WorkflowVisualizationStudioService::scenesForProject(const QString& projectId) const {
    QVector<WfStudioScene> result;
    for (auto it = scenes_.begin(); it != scenes_.end(); ++it) {
        if (it.value().projectId == projectId)
            result << it.value();
    }
    return result;
}

int WorkflowVisualizationStudioService::sceneCount() const {
    return scenes_.size();
}

QGraphicsScene* WorkflowVisualizationStudioService::renderScene(const QString& sceneId) {
    if (!scenes_.contains(sceneId))
        return nullptr;
    const WfStudioScene& s = scenes_[sceneId];
    auto* gs = new QGraphicsScene();
    gs->setSceneRect(0, 0, s.width, s.height);
    gs->addRect(0, 0, s.width, s.height);
    auto* text = gs->addText(s.name);
    text->setPos(s.width / 4, s.height / 2);
    emit sceneRendered(sceneId);
    return gs;
}
