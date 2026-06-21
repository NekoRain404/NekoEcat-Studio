#include "DiagramWidget.h"

#include <QPainter>

DiagramWidget::DiagramWidget(QWidget *parent) : QWidget(parent) {
  setMinimumSize(400, 300);
}

void DiagramWidget::addNode(double x, double y, const QString &name, const QString &type) {
  nodes_.append({x, y, name, type});
  update();
}

void DiagramWidget::clearNodes() {
  nodes_.clear();
  update();
}

int DiagramWidget::nodeCount() const { return nodes_.size(); }

const QVector<DiagramNode> &DiagramWidget::nodes() const { return nodes_; }

void DiagramWidget::paintEvent(QPaintEvent * /*event*/) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);

  p.fillRect(rect(), Qt::white);

  for (int i = 1; i < nodes_.size(); ++i) {
    const auto &a = nodes_[0];
    const auto &b = nodes_[i];
    p.setPen(QPen(Qt::darkGray, 2));
    p.drawLine(QPointF(a.x, a.y), QPointF(b.x, b.y));
  }

  constexpr double kRadius = 20.0;
  for (const auto &n : nodes_) {
    QColor fill = (n.type == "master") ? QColor(70, 130, 200) : QColor(80, 180, 120);
    p.setPen(QPen(Qt::black, 2));
    p.setBrush(fill);
    p.drawEllipse(QPointF(n.x, n.y), kRadius, kRadius);

    p.setPen(Qt::black);
    QFont f = p.font();
    f.setPointSize(9);
    p.setFont(f);
    p.drawText(QRectF(n.x - 40, n.y + kRadius + 2, 80, 20),
               Qt::AlignHCenter | Qt::AlignTop, n.name);
  }
}
