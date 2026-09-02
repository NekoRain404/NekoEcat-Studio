#pragma once

// EtherCATVisualizationService — creates graphical views for topology,
// data, performance, and error visualization.
//
// Returns QGraphicsScene instances for embedding in workspace widgets.
//
// Thread safety: main (GUI) thread only.

#include <QColor>
#include <QMap>
#include <QObject>
#include <QString>
#include <QVector>

class QGraphicsScene;
class EcatClient;
class EventBus;
struct SlaveInfo;
struct DataPoint;
struct PerformanceMetrics;
struct ErrorInfo;

struct VisualizationConfig {
    QString viewType;
    QString dataSource;
    QString layout;
    QMap<QString, QColor> colors;
    bool animations = false;
    bool interactions = true;
};

class EtherCATVisualizationService : public QObject {
    Q_OBJECT
public:
    explicit EtherCATVisualizationService(EventBus* bus, EcatClient* client, QObject* parent = nullptr);

    QGraphicsScene* createTopologyView(const QVector<SlaveInfo>& slaves);
    QGraphicsScene* createDataView(const QVector<DataPoint>& data);
    QGraphicsScene* createPerformanceView(const PerformanceMetrics& metrics);
    QGraphicsScene* createErrorView(const QVector<ErrorInfo>& errors);

    void setConfig(const VisualizationConfig& config);
    VisualizationConfig config() const { return config_; }

signals:
    void viewCreated(const QString& viewType);

private:
    QGraphicsScene* makeScene();

    EventBus* bus_;
    EcatClient* client_;
    VisualizationConfig config_;
};
