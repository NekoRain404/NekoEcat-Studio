#include "TopologyGraphWidget.h"
#include "SlaveNodeItem.h"

#include "EthercatTypes.h"

#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QScrollBar>
#include <QWheelEvent>

static constexpr qreal kNodeSpacingX = 220.0;
static constexpr qreal kNodeSpacingY = 120.0;
static constexpr qreal kMargin = 40.0;

TopologyGraphWidget::TopologyGraphWidget(QWidget* parent) : QGraphicsView(parent) {
    scene_ = new QGraphicsScene(this);
    setScene(scene_);
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::NoDrag);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setStyleSheet("QGraphicsView { background-color: #1e1e1e; border: none; }");
}

void TopologyGraphWidget::setSlaves(const QVector<SlaveInfo>& slaves) {
    // Store slave data by rebuilding node metadata, then refresh the graph.
    // We temporarily store position/state/name in the nodes themselves.
    scene_->clear();
    nodes_.clear();

    for (const auto& s : slaves) {
        auto* node = new SlaveNodeItem(s.position, s.name, s.state);
        connect(node, &SlaveNodeItem::clicked, this, &TopologyGraphWidget::slaveClicked);
        connect(node, &SlaveNodeItem::doubleClicked, this, &TopologyGraphWidget::slaveDoubleClicked);
        scene_->addItem(node);
        nodes_.append(node);
    }

    rebuildGraph();
}

void TopologyGraphWidget::setLayoutMode(Layout mode) {
    if (layoutMode_ == mode)
        return;
    layoutMode_ = mode;
    rebuildGraph();
}

void TopologyGraphWidget::zoomIn() {
    scale(1.15, 1.15);
}

void TopologyGraphWidget::zoomOut() {
    scale(1.0 / 1.15, 1.0 / 1.15);
}

void TopologyGraphWidget::fitToView() {
    if (nodes_.isEmpty())
        return;
    fitInView(scene_->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void TopologyGraphWidget::wheelEvent(QWheelEvent* event) {
    const double factor = event->angleDelta().y() > 0 ? 1.12 : 1.0 / 1.12;
    scale(factor, factor);
}

void TopologyGraphWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        panning_ = true;
        panStart_ = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void TopologyGraphWidget::mouseMoveEvent(QMouseEvent* event) {
    if (panning_) {
        const QPoint delta = event->pos() - panStart_;
        panStart_ = event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void TopologyGraphWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        panning_ = false;
        unsetCursor();
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void TopologyGraphWidget::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent(event);
    if (!nodes_.isEmpty()) {
        fitToView();
    }
}

void TopologyGraphWidget::rebuildGraph() {
    switch (layoutMode_) {
        case Layout::Linear:
            applyLinearLayout();
            break;
        case Layout::Tree:
            applyTreeLayout();
            break;
    }
    drawConnections();
    if (!nodes_.isEmpty()) {
        fitToView();
    }
}

void TopologyGraphWidget::applyLinearLayout() {
    for (int i = 0; i < nodes_.size(); ++i) {
        nodes_[i]->setPos(kMargin + i * kNodeSpacingX, kMargin);
    }
}

void TopologyGraphWidget::applyTreeLayout() {
    if (nodes_.isEmpty())
        return;

    // Simple tree layout: root at left, children stacked vertically.
    // For a linear bus topology, arrange in a staggered pattern.
    const int cols = static_cast<int>(std::ceil(std::sqrt(nodes_.size())));
    for (int i = 0; i < nodes_.size(); ++i) {
        const int col = i % cols;
        const int row = i / cols;
        nodes_[i]->setPos(kMargin + col * kNodeSpacingX, kMargin + row * kNodeSpacingY);
    }
}

void TopologyGraphWidget::drawConnections() {
    for (int i = 0; i + 1 < nodes_.size(); ++i) {
        const QPointF p1 = nodes_[i]->pos() + QPointF(180.0, 40.0);
        const QPointF p2 = nodes_[i + 1]->pos() + QPointF(0.0, 40.0);

        auto* line = scene_->addLine(QLineF(p1, p2), QPen(QColor(100, 100, 100), 2.0));
        line->setZValue(-1);

        // Arrow head
        const QLineF segment(p1, p2);
        const double angle = std::atan2(segment.dy(), segment.dx());
        const QPointF arrowP2 = p2 - QPointF(8.0 * std::cos(angle - 0.4), 8.0 * std::sin(angle - 0.4));
        const QPointF arrowP3 = p2 - QPointF(8.0 * std::cos(angle + 0.4), 8.0 * std::sin(angle + 0.4));
        scene_->addPolygon({p2, arrowP2, arrowP3}, QPen(Qt::NoPen), QBrush(QColor(100, 100, 100)));
    }
}
