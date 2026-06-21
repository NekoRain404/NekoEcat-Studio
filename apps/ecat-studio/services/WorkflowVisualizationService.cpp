#include "WorkflowVisualizationService.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QJsonArray>
#include <QtMath>

// WorkflowVisualizationService.cpp — QGraphicsScene-based workflow visualization builder
//
// Implementation notes:
//   - Four visualization types: flowchart, Gantt chart, dependency graph, resource timeline
//   - Dependency graph uses circular layout with color-coded nodes and dashed edges
//   - Gantt bars scaled at 0.5px/ms with a 4px minimum width

WorkflowVisualizationService::WorkflowVisualizationService(QObject *parent)
    : QObject(parent) {}

QGraphicsScene *WorkflowVisualizationService::createFlowchart(
    const WfVisualizationConfig &config) {
  auto *scene = makeScene(config);
  QStringList nodes = {"Start", "Process", "Decision", "Action", "End"};
  double x = 50.0;
  for (int i = 0; i < nodes.size(); ++i) {
    double y = 100.0;
    addFlowchartNode(scene, QString::number(i), nodes[i], x, y);
    if (i > 0)
      addFlowchartEdge(scene, x - 80, y, x, y);
    x += 120.0;
  }
  emit visualizationCreated("flowchart");
  return scene;
}

QGraphicsScene *WorkflowVisualizationService::createGanttChart(
    const QVector<WfVizTask> &tasks) {
  WfVisualizationConfig config;
  config.viewType = WfVisualizationType::GanttChart;
  auto *scene = makeScene(config);
  for (int i = 0; i < tasks.size(); ++i) {
    WfGanttEntry entry;
    entry.taskId = tasks[i].id;
    entry.taskName = tasks[i].name;
    entry.startMs = tasks[i].startMs;
    entry.durationMs = tasks[i].durationMs;
    entry.color = tasks[i].color.isValid() ? tasks[i].color : config.primaryColor;
    addGanttBar(scene, entry, i);
  }
  emit visualizationCreated("gantt");
  return scene;
}

QGraphicsScene *WorkflowVisualizationService::createDependencyGraph(
    const QJsonObject &graph) {
  WfVisualizationConfig config;
  config.viewType = WfVisualizationType::DependencyGraph;
  auto *scene = makeScene(config);
  QJsonObject nodes = graph.value("nodes").toObject();
  QJsonObject edges = graph.value("edges").toObject();
  double angleStep = 2.0 * M_PI / qMax(nodes.size(), 1);
  double cx = 250.0, cy = 200.0, radius = 150.0;
  int idx = 0;
  QHash<QString, QPointF> positions;
  for (auto it = nodes.begin(); it != nodes.end(); ++it) {
    double angle = angleStep * idx;
    double x = cx + radius * qCos(angle);
    double y = cy + radius * qSin(angle);
    positions[it.key()] = QPointF(x, y);
    QColor color = QColor::fromHsv((idx * 60) % 360, 180, 220);
    addGraphNode(scene, it.key(), x, y, color);
    ++idx;
  }
  for (auto it = edges.begin(); it != edges.end(); ++it) {
    QString from = it.key();
    QJsonArray targets = it.value().toArray();
    for (const auto &t : targets) {
      QString to = t.toString();
      if (positions.contains(from) && positions.contains(to)) {
        addGraphEdge(scene, positions[from].x(), positions[from].y(),
                     positions[to].x(), positions[to].y());
      }
    }
  }
  emit visualizationCreated("dependency_graph");
  return scene;
}

QGraphicsScene *WorkflowVisualizationService::createResourceTimeline(
    const QVector<WfVizResource> &resources) {
  WfVisualizationConfig config;
  config.viewType = WfVisualizationType::ResourceTimeline;
  auto *scene = makeScene(config);
  double maxTime = 0.0;
  for (const auto &r : resources) {
    for (const auto &a : r.allocations) {
      double end = a.first + a.second;
      if (end > maxTime)
        maxTime = end;
    }
  }
  if (maxTime <= 0.0)
    maxTime = 100.0;
  for (int i = 0; i < resources.size(); ++i) {
    addTimelineRow(scene, resources[i], i, maxTime);
  }
  emit visualizationCreated("resource_timeline");
  return scene;
}

QGraphicsScene *WorkflowVisualizationService::makeScene(
    const WfVisualizationConfig &config) {
  auto *scene = new QGraphicsScene();
  scene->setBackgroundBrush(config.backgroundColor);
  scene->setSceneRect(0, 0, 600, 400);
  return scene;
}

void WorkflowVisualizationService::addFlowchartNode(QGraphicsScene *scene,
                                                    const QString &id,
                                                    const QString &label,
                                                    double x, double y) {
  auto *rect = scene->addRect(x - 35, y - 20, 70, 40);
  rect->setBrush(QColor(200, 220, 255));
  auto *text = scene->addText(label);
  text->setPos(x - text->boundingRect().width() / 2, y - 10);
}

void WorkflowVisualizationService::addFlowchartEdge(QGraphicsScene *scene,
                                                    double x1, double y1,
                                                    double x2, double y2) {
  scene->addLine(x1, y1, x2, y2, QPen(Qt::black, 2));
}

void WorkflowVisualizationService::addGanttBar(QGraphicsScene *scene,
                                              const WfGanttEntry &entry,
                                              int row) {
  double y = 30.0 + row * 35.0;
  double x = 120.0 + entry.startMs * 0.5;
  double w = entry.durationMs * 0.5;
  if (w < 4.0)
    w = 4.0;
  auto *bar = scene->addRect(x, y, w, 25);
  bar->setBrush(entry.color);
  bar->setToolTip(entry.taskName + " (" + QString::number(entry.durationMs) + "ms)");
  auto *text = scene->addText(entry.taskName);
  text->setPos(5, y + 2);
}

void WorkflowVisualizationService::addGraphNode(QGraphicsScene *scene,
                                                const QString &id, double x,
                                                double y, const QColor &color) {
  auto *ellipse = scene->addEllipse(x - 20, y - 20, 40, 40);
  ellipse->setBrush(color);
  auto *text = scene->addText(id);
  text->setPos(x - text->boundingRect().width() / 2, y - 10);
}

void WorkflowVisualizationService::addGraphEdge(QGraphicsScene *scene, double x1,
                                                double y1, double x2,
                                                double y2) {
  QPen pen(Qt::gray, 1, Qt::DashLine);
  scene->addLine(x1, y1, x2, y2, pen);
}

void WorkflowVisualizationService::addTimelineRow(QGraphicsScene *scene,
                                                  const WfVizResource &resource,
                                                  int row, double maxTime) {
  double y = 30.0 + row * 35.0;
  auto *text = scene->addText(resource.name);
  text->setPos(5, y + 2);
  double scaleX = 450.0 / maxTime;
  for (const auto &alloc : resource.allocations) {
    double x = 120.0 + alloc.first * scaleX;
    double w = alloc.second * scaleX;
    if (w < 3.0)
      w = 3.0;
    QColor color = QColor::fromHsv((row * 50) % 360, 180, 220);
    auto *bar = scene->addRect(x, y, w, 25);
    bar->setBrush(color);
  }
}
