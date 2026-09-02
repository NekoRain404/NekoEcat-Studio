#pragma once

// TopologyGraphWidget — QGraphicsView that visualizes the EtherCAT bus
// topology as a graph of SlaveNodeItem nodes with connecting lines.
// Supports linear (horizontal chain) and tree (hierarchical) layouts,
// zoom via mouse wheel, and pan via middle-mouse drag.

#include <QGraphicsView>
#include <QVector>

class QGraphicsScene;
class SlaveNodeItem;
struct SlaveInfo;

class TopologyGraphWidget : public QGraphicsView {
    Q_OBJECT
public:
    enum class Layout { Linear, Tree };

    explicit TopologyGraphWidget(QWidget* parent = nullptr);

    void setSlaves(const QVector<SlaveInfo>& slaves);
    void setLayoutMode(Layout mode);
    Layout layoutMode() const { return layoutMode_; }

    void zoomIn();
    void zoomOut();
    void fitToView();

signals:
    void slaveClicked(int position);
    void slaveDoubleClicked(int position);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void rebuildGraph();
    void applyLinearLayout();
    void applyTreeLayout();
    void drawConnections();

    QGraphicsScene* scene_;
    QVector<SlaveNodeItem*> nodes_;
    Layout layoutMode_ = Layout::Linear;
    bool panning_ = false;
    QPoint panStart_;
};
