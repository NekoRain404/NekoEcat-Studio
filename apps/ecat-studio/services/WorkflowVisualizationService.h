#pragma once

// WorkflowVisualizationService — creates graphical visualizations of workflow
// structures including flowcharts, Gantt charts, dependency graphs, and
// resource timelines.
//
// Thread safety: main (GUI) thread only.

#include <QColor>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QVector>

class QGraphicsScene;

enum class WfVisualizationType { Flowchart, GanttChart, DependencyGraph, ResourceTimeline };
enum class WfLayoutAlgorithm { Automatic, Horizontal, Vertical, Circular, Layered };

struct WfVisualizationConfig {
    WfVisualizationType viewType = WfVisualizationType::Flowchart;
    QString dataSource;
    WfLayoutAlgorithm layout = WfLayoutAlgorithm::Automatic;
    QColor primaryColor = QColor(66, 133, 244);
    QColor secondaryColor = QColor(234, 67, 53);
    QColor backgroundColor = QColor(255, 255, 255);
    bool animations = true;
    bool interactions = true;
};

struct WfVizTask {
    QString id;
    QString name;
    double startMs = 0.0;
    double durationMs = 0.0;
    QColor color;
    QStringList dependencies;
};

struct WfVizResource {
    QString id;
    QString name;
    double capacity = 1.0;
    QVector<QPair<double, double>> allocations;
};

struct WfGanttEntry {
    QString taskId;
    QString taskName;
    double startMs = 0.0;
    double durationMs = 0.0;
    QColor color;
};

class WorkflowVisualizationService : public QObject {
    Q_OBJECT
public:
    explicit WorkflowVisualizationService(QObject* parent = nullptr);

    QGraphicsScene* createFlowchart(const WfVisualizationConfig& config);
    QGraphicsScene* createGanttChart(const QVector<WfVizTask>& tasks);
    QGraphicsScene* createDependencyGraph(const QJsonObject& graph);
    QGraphicsScene* createResourceTimeline(const QVector<WfVizResource>& resources);

signals:
    void visualizationCreated(const QString& type);

private:
    QGraphicsScene* makeScene(const WfVisualizationConfig& config);
    void addFlowchartNode(QGraphicsScene* scene, const QString& id, const QString& label, double x, double y);
    void addFlowchartEdge(QGraphicsScene* scene, double x1, double y1, double x2, double y2);
    void addGanttBar(QGraphicsScene* scene, const WfGanttEntry& entry, int row);
    void addGraphNode(QGraphicsScene* scene, const QString& id, double x, double y, const QColor& color);
    void addGraphEdge(QGraphicsScene* scene, double x1, double y1, double x2, double y2);
    void addTimelineRow(QGraphicsScene* scene, const WfVizResource& resource, int row, double maxTime);
};
